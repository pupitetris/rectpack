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
#include "Parameters.h"
#include "Globals.h"
#include "Grid.h"
#include "GridViz.h"
#include "Perfect.h"
#include "IntBalOpt.h"
#include <iostream>
#include <algorithm>
#include <iomanip>

IntBalOpt::IntBalOpt() :
  m_bVarOrder(true),
  m_bDomBasedRanges(true) {
}

IntBalOpt::~IntBalOpt() {
}

void IntBalOpt::initialize(const Parameters* pParams) {
  IntPack::initialize(pParams);
  m_bVarOrder = (pParams->m_s1.find_first_of('v') >= pParams->m_s1.size());
  m_bDomBasedRanges = (pParams->m_s1.find_first_of('r') >= pParams->m_s1.size());

#ifdef VIZ
  m_State.set(&m_vY, YI);
#endif // VIZ
  m_Ranges.initialize(pParams);
}

void IntBalOpt::initialize(const HeapBox* pBox) {
  IntPack::initialize(pBox);
  
  /**
   * Precompute some values that depend only on the bounding box.
   */

  if(m_pDomination && m_pDomination->enabled() && m_bDomBasedRanges)
    m_Ranges.initialize(m_vRectPtrs, m_Box, m_pDomination);
  else
    m_Ranges.initializeSimple(m_vRectPtrs, m_Box, false);

  /**
   * Precompute the branching factor of all of the rectangles and
   * decide which rectangle we will be breaking symmetry for.
   */

  if(m_pParams->breakVerticalSymmetry())
    m_Ranges.breakAllSymmetry(m_vRectPtrs[0], m_Box, m_pDomination);
  m_Branches.initialize(m_pDomination, &m_Box, m_vRectPtrs.begin(),
			m_iFirstUnit, &m_Ranges);

  m_vDomains.initialize(this, &m_Ranges, &m_Branches);
}

bool IntBalOpt::pack() {

  /**
   * The actual packing attempt.
   */

  if(packY(0)) {
    return(true);
  }
  
  if(!m_Branches.empty()) {
    Rectangle* r = m_Branches.front().m_pRect;
    if(r->rotatable() && !m_Box.square()) {
      r->rotate();
      if(packY(0)) return(true);
    }
  }
  return(false);
}

bool IntBalOpt::packY(size_t i) {
  if(i == m_Branches.size()) return(packX());
  Rectangle* r(m_Branches[i].m_pRect);

  /**
   * Checks whether or not the domination entries are valid from the
   * left.
   */
  
  m_Ranges.checkDomLeft(r, m_vY, m_Box);

  /**
   * We don't have to run compression because we know that no strips
   * will be derived with the placement of just the first
   * square. However in the recursive calls, we have to make the
   * compression call.
   */

  m_vY.m_vRectangles.push_back(r);
  IntDomains* pDomain = &m_vDomains.get(r);
  for(IntDomains::iterator j = pDomain->begin();
      j != pDomain->end(); ++j) {
    m_Nodes.tick(YI);
    m_vY.m_vDomain[r->m_nID] = r->yi = *j;
    m_vY.pushyi(r);
    if(waste(i + 1))
      return(true);
    m_vY.popyi(r);
  }
  return(false);
}

bool IntBalOpt::location(size_t i) {
  Rectangle* r(m_Branches[i].m_pRect);
  m_Ranges.checkDom(r, m_vY, m_Box);
  size_t nMinY(0);
  const Rectangle* pDuplicate = m_Duplicates.prev(r);
  if(pDuplicate && pDuplicate->yi.m_nBegin > 0) {
    nMinY = pDuplicate->yi.m_nBegin;
    m_Ranges.m_vDomLeft[r->m_nID] = false;
  }
  
  size_t nConfig = m_Ranges.m_vDomLeft[r->m_nID] ? 0 : 2;
  nConfig += m_Ranges.m_vDomRight[r->m_nID] ? 0 : 1;
  IntDomains* pDomain = &m_vDomains.get(r, nConfig);
  for(IntDomains::iterator j = pDomain->begin();
      j != pDomain->end(); ++j) {

    /**
     * Adjust range for duplicates, if applicable.
     */

    r->yi = *j;
    if(nMinY) {
      r->yi.m_nBegin = std::max(r->yi.m_nBegin, (Int) nMinY);
      r->yi.m_nWidth = r->m_nHeight - (r->yi.m_nEnd - r->yi.m_nBegin);
      if(r->yi.empty()) continue;
    }
    if(!m_vY.canFityi(r)) continue;
    m_Nodes.tick(YI);
    m_vY.m_vDomain[r->m_nID] = r->yi;
    m_vY.pushyi(r);
    if(m_vY.compress(r))
      if(waste(i + 1))
	return(true);
    m_vY.popyi(r);
  }
  return(false);
}

bool IntBalOpt::waste(size_t i) {
  if(m_vY.m_vWaste.canFit())
    if(searchSpace(i)) return(true);
  return(false);
}

bool IntBalOpt::packcra() {
  m_vY.clear();
  m_vY.resizey(m_Box, m_vRectPtrs);
  m_pPerfect->initWastedSpace();
  for(RectPtrArray::iterator i = m_vRectPtrs.begin();
      i != m_iFirstUnit; ++i) {
    m_vY.m_vDomain[(*i)->m_nID] = *i;
    m_vY.pushy(*i);
  }
  return(packX());
}

bool IntBalOpt::searchSpace(size_t i) {
  if(bQuit) return(false);
  if(i == m_Branches.size()) {
    for(size_t i = 0; i < m_Branches.size(); ++i) {
      Rectangle* r = m_Branches[i].m_pRect;
      if(m_vY.m_vDomain[r->m_nID].m_nWidth < (int) r->m_nHeight)
	return(strips(r, m_Branches.size()));
    }
    return(packX());
  }
  
  Rectangle* r(NULL);
  size_t nStripArea(bestStripArea(r));
  if(r && nStripArea > m_Branches[i].m_pRect->m_nArea) {
    return(strips(r, i));
  }
  else {
    r = m_Branches[i].m_pRect;
    if(location(i)) return(true);

    /**
     * Handle the unoriented case, if applicable.
     */

    if(r->rotatable()) {
      r->rotate();
      if(location(i)) return(true);
      r->rotate();
    }
    return(false);
  }
}

size_t IntBalOpt::bestStripArea(Rectangle*& r) {
  if(!m_bVarOrder) return(std::numeric_limits<size_t>::max());
  
  size_t nMax(0);
  for(std::vector<const Rectangle*>::const_iterator i = m_vY.m_vRectangles.begin();
      i != m_vY.m_vRectangles.end(); ++i) {
    size_t t = (*i)->m_nWidth *
      ((*i)->m_nHeight - m_vY.m_vDomain[(*i)->m_nID].m_nWidth);
    if(t > nMax) {
      nMax = t;
      r = m_vRectPtrs[(*i)->m_nID];
    }
  }
  return(nMax);
}

bool IntBalOpt::strips(Rectangle* r, size_t i) {
  for(r->y = (size_t) m_vY.m_vDomain[r->m_nID].m_nBegin;
      r->y <= (size_t) m_vY.m_vDomain[r->m_nID].m_nEnd; ++r->y) {
    m_Nodes.tick(YF);
    if(!m_vY.canFitStrips(r)) continue;
    m_vY.pushStripsy(r);
    if(m_vY.compress())
      if(waste(i))
	return(true);
    m_vY.popStripsy(r);
  }
  return(false);
}
