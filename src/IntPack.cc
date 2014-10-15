/**
 * Copyright (C) 2011 Eric Huang
 *
 * This file is part of rectpack.
 *
 * rectpack is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * rectpack is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with rectpack. If not, see <http://www.gnu.org/licenses/>.
 */

#include "BoundingBoxes.h"
#include "Domination.h"
#include "IntPack.h"
#include "IntPlacements.h"
#include "Packer.h"
#include "GridViz.h"
#include "Grid.h"
#include "Parameters.h"
#include "Perfect.h"
#include "Placements.h"
#include "Rectangle.h"
#include "SearchControl.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <assert.h>

IntPack::IntPack() :
  m_pGrid(NULL),
  m_pDomination(NULL),
  m_pDomTemplate(NULL),
  m_pPerfect(NULL) {
  m_bYSmallerThanX = true;
  m_bCanBuildDomination = true;
  m_pPerfect = new Perfect();
}

IntPack::~IntPack() {
  delete m_pGrid;
  delete m_pDomination;
  delete m_pDomTemplate;
  delete m_pPerfect;
}

void IntPack::initialize(const Parameters* pParams) {
  Packer::initialize(pParams);

  /**
   * Initialize the 1D cumulative scheduling data structures.
   */

  m_vY.initialize(pParams);

  /**
   * Initialize the appropriate (non)visualized grid.
   */

  delete m_pGrid;
#ifdef VIZ
  m_pGrid = new GridViz(this);
  m_pGrid->m_nType = XF;
#else
  m_pGrid = new Grid(this);
#endif
  m_pGrid->initialize(m_pParams);

#ifdef VIZ
  if(!m_pParams->m_bScheduling)
    m_State.set(m_pGrid, XF);
#endif

  /**
   * Initialize the domination tables.
   */

  delete m_pDomination;
  m_pDomination = NULL;
  delete m_pDomTemplate;
  m_pDomTemplate = Domination::create(m_pParams, &m_vRectPtrs,
				      &m_Duplicates, m_bCanBuildDomination);

  /**
   * Initialize the perfect packing engine. This call passes over the
   * pointers on data structures guaranteed to not be destructed. That
   * is, they'll always represent the most updated data whenever we
   * end up calling the 2D packing engine (which is why we pass things
   * like the bounding box pointer, etc.)
   */

  m_pPerfect->initialize(pParams->m_s2);
  m_pPerfect->initialize(&m_nCuSP, &m_Box, &m_XTime, m_pGrid,
			 &m_Nodes);
}

void IntPack::initialize(const HeapBox* pBox) {
  Packer::initialize(pBox);

  /**
   * Initialize the domination tables.
   */
  
  delete m_pDomination;
  m_pDomination = m_pDomTemplate->clone();
  finalizeDomination();

  /**
   * Initialize the cumulative constraint bins data structure.
   */

  m_vY.clear();
  m_vY.resizey(m_Box, m_vRectPtrs);

  /**
   * Initialize the grids data structure.
   */

  if(m_pParams->m_nYScale < (URational) 0 &&
     m_pDomination && m_pDomination->enabled()) {
    for(size_t i = 0; i < m_vRects.size(); ++i) {
      m_vRects[i].gutsyScale(m_Box);
      m_vRects[i].printWH();
      std::cout << " assigned scale " << m_vRects[i].scale() << std::endl;
    }
  }
  m_pGrid->initialize(&m_Box);
  assert(m_Box.m_nWidth <= m_pGrid->size() &&
	 m_Box.m_nHeight <= m_pGrid->front().size());
#ifdef VIZ
  if(m_pControl)
    m_pControl->postClear();
#endif // VIZ

  /**
   * Initialize the perfect packing data structures.
   */

  m_pPerfect->initWastedSpace();
}

void IntPack::initialize(const BoundingBoxes* pBoxes) {
  Packer::initialize(pBoxes);
}

void IntPack::finalizeDomination() {
  if(m_pParams->breakVerticalSymmetry()) {
    if(m_vRects[0].rotatable()) {
      if(m_vRectPtrs.front()->m_nMaxDim < m_Box.m_nHeight)
	m_pDomination->preserveTopSymmetry((m_Box.m_nHeight - m_vRectPtrs.front()->m_nMaxDim) / 2);
      else
	m_pDomination->preserveTopSymmetry((m_Box.m_nHeight - m_vRectPtrs.front()->m_nMinDim) / 2);
    }
    else
      m_pDomination->preserveTopSymmetry((m_Box.m_nHeight - m_vRectPtrs.front()->m_nHeight) / 2);
  }
  m_pDomination->buildTableStage2();
}

void IntPack::placeUnitRectangles() {

  /**
   * If we're only placing rectangles for purposes of cumulative
   * scheduling, we can do that easily and simply return.
   */

  if(m_pParams->m_bScheduling) {
    UInt nIndex(0);
    for(RectPtrArray::iterator i = m_iFirstUnit;
	i != m_vRectPtrs.end(); ++i) {
      while(m_vY[nIndex] == 0)
	++nIndex;
      (*i)->y = nIndex;
      --m_vY[nIndex];
    }
  }
  else {

    /**
     * Compute the number of unit rectangles we must place.
     */

    UInt nUnits(m_vRectPtrs.end() - m_iFirstUnit);

    /**
     * First copy over the rectangles for whom we have both x and y
     * coordinates.
     */

    IntPlacements ip;
    for(RectPtrArray::const_iterator i = m_vRectPtrs.begin();
	i != m_iFirstUnit; ++i)
      ip.push_back(IntPlacement(*i, (*i)->x, (*i)->y));

    /**
     * Now place all of the unit rectangles.
     */

    RectPtrArray::const_iterator r = m_iFirstUnit;
    UInt y(0);
    while(nUnits) {

      /**
       * Find the next y-coordinate that can accommodate some unit
       * rectangles.
       */

      while(m_vY[y] == 0) ++y;

      /**
       * Set up data structures for placing a local set of unit
       * rectangles just on this column. nLocal is the number of unit
       * rectangles we will be placing at the current moment.
       */

      UInt nLocal = std::min(nUnits, (UInt) m_vY[y]);
      IntPlacements ipCopy(ip);
      IntPlacements ipLocal;

      /**
       * Assign the y-coordinates since this is the same across all
       * unit rectangles at this point.
       */

      ipLocal.resize(nLocal);
      for(IntPlacements::iterator i = ipLocal.begin();
	  i != ipLocal.end(); ++i) {
	i->m_nY = y;
	i->m_pRect = *r;
	++r;
      }

      /**
       * Make the actual x-coordinate assignments.
       */

      ipCopy.assignX(ipLocal, y);
      nUnits -= nLocal;

      /**
       * Insert the induced coordinates back into the original set of
       * rectangles.
       */

      for(IntPlacements::const_iterator i = ipLocal.begin();
	  i != ipLocal.end(); ++i) {
	m_vRectPtrs[i->m_pRect->m_nID]->x = i->m_nX;
	m_vRectPtrs[i->m_pRect->m_nID]->y = i->m_nY;
      }
    }
  }
}

TimeSpec& IntPack::timeDomination() {
  return(m_pDomination->m_Time);
}

bool IntPack::packX() {
  if(m_pParams->m_bScheduling) {
    for(RectPtrArray::iterator i = m_vRectPtrs.begin();
	i != m_iFirstUnit; ++i)
      (*i)->y = (*i)->yi.m_nBegin;
    return(true);
  }
  else {
    if(m_pPerfect->packX(&m_vY, m_vRectPtrs.begin(), m_iFirstUnit)) {
      m_pPerfect->m_Inferences.m_vComponents.instantiateX(m_vRectPtrs.begin(),
							  m_iFirstUnit);
      return(true);
    }
  }
  return(false);
}

void IntPack::get(Placements& v) const {
  v.clear();
  v.m_Box = m_Box;
  v.reserve(m_vRects.size());
  for(RectPtrArray::const_iterator i = m_vRectPtrs.begin();
      i != m_vRectPtrs.end(); ++i)
    v.push_back(Placement(*i, Coordinates((*i)->x, (*i)->y)));
  if(m_pParams->m_bScheduling) v.assignX();
}
