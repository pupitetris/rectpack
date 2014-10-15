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
#include "Duplicate.h"
#include <iostream>
#include <algorithm>
#include <iomanip>

Duplicate::Duplicate() :
  m_bVarOrder(true),
  m_bHandleDuplicates(true) {
}

Duplicate::~Duplicate() {
}

void Duplicate::initialize(const Parameters* pParams) {
  IntPack::initialize(pParams);

  /**
   * Disable any features as requested by the user.
   */

  m_bVarOrder = (pParams->m_s1.find_first_of('v') >= pParams->m_s1.size());
  m_bHandleDuplicates = (pParams->m_s1.find_first_of('D') >= pParams->m_s1.size());

#ifdef VIZ
  m_State.set(&m_vY, YI);
#endif // VIZ

  m_vDominationLeft.resize(pParams->m_vInstance.size());
  m_vDominationRight.resize(pParams->m_vInstance.size());
}

void Duplicate::initialize(const HeapBox* pBox) {
  IntPack::initialize(pBox);

  /**
   * Precompute some values that depend only on the bounding box.
   */

  inferCoordinateConstraints();
}

bool Duplicate::pack() {

  /**
   * The actual packing attempt.
   */

  Rectangle* r = m_vRectPtrs.front();
  if(m_Box.canFit(r) && packY(m_vRectPtrs.begin(), m_iFirstUnit))
    return(true);

  if(r->rotatable() && !m_Box.square()) {
    rotate(r);
    m_vLastBinYI.front() = (m_Box.m_nHeight - r->m_nHeight) / 2;
    if(packY(m_vRectPtrs.begin(), m_iFirstUnit)) return(true);
  }
  return(false);
}

void Duplicate::inferCoordinateConstraints() {

  /**
   * Precompute some values that depend only on the bounding box.
   */

  m_vLastBinYI.resize(m_vRects.size());
  m_vForbiddenStartYI.resize(m_vRects.size());
  for(std::vector<Rectangle>::iterator i = m_vRects.begin();
      i != m_vRects.end(); ++i) {

    /**
     * The absolute last bin is initially just the last coordinate
     * that keeps the rectangle inside of the bounding box.
     */

    m_vLastBinYI[i->m_nID] = m_Box.m_nHeight - i->m_nHeight;

    /**
     * The ForbiddenStartYI determines the coordinate at which we
     * start considering our packing attempt on the right side of the
     * box. If the forbidden region coming from the first edge
     * overlaps those from the second edge, then we assume that we'll
     * explore the forbidden region in totality from the first edge,
     * and simply shave off a little bit from the region coming from
     * the second edge. Of course, we need to also allow the very last
     * (against the wall) value, so we can't prune that value out.
     */

    if(m_vLastBinYI[i->m_nID] <= m_pDomination->entriesw1(&(*i)))
      m_vForbiddenStartYI[i->m_nID] = m_vLastBinYI[i->m_nID]; // Should this be 0?
    else
      m_vForbiddenStartYI[i->m_nID] =
	(m_vLastBinYI[i->m_nID] - m_pDomination->entriesw2(&(*i)) + 1);
  }
  m_vLastBinYI.front() = (m_Box.m_nHeight - m_vRects.front().m_nHeight) / 2;
}

bool Duplicate::packY(RectPtrArray::iterator iBegin,
		      const RectPtrArray::const_iterator& iEnd) {
  if(iBegin == iEnd) return(packX());
  Rectangle* r(*iBegin);

  /**
   * Checks whether or not the domination entries are valid from the
   * left.
   */
  
  checkDominationLeft(r);

  /**
   * We don't have to run compression because we know that no strips
   * will be derived with the placement of just the first
   * square. However in the recursive calls, we have to make the
   * compression call.
   */

  m_vY.m_vRectangles.push_back(r);
  UInt nRealLast = std::min(m_vLastBinYI[r->m_nID],
			    m_pDomination->entriesw1(r) - 1);
  for(r->yi.init(0, r->m_nHeight, 0, nRealLast); r->yi.valid(nRealLast);
      r->yi.next(r->m_nHeight, nRealLast)) {
    if(r->yi.m_nEnd > 0 && m_vDominationLeft[r->m_nID] &&
       m_pDomination->dominatedw1(r, r->yi.m_nEnd)) continue;
    m_Nodes.tick(YI);
    m_vY.m_vDomain[r->m_nID] = r->yi;
    m_vY.pushyi(r);
    if(waste(iBegin + 1, iEnd))
      return(true);
    m_vY.popyi(r);
  }

  for(r->yi.init(m_pDomination->entriesw1(r), r->m_nHeight,
		 m_pParams->m_nYScale, m_vLastBinYI[r->m_nID]);
      r->yi.valid(m_vLastBinYI[r->m_nID]);
      r->yi.next(r->m_nHeight, m_vLastBinYI[r->m_nID])) {
    m_Nodes.tick(YI);
    m_vY.m_vDomain[r->m_nID] = r->yi;
    m_vY.pushyi(r);
    if(waste(iBegin + 1, iEnd))
      return(true);
    m_vY.popyi(r);
  }
  return(false);
}

bool Duplicate::location(RectPtrArray::iterator iBegin,
			 const RectPtrArray::const_iterator& iEnd) {
  Rectangle* r(*iBegin);
  checkDomination(r);
  if(m_pParams->m_vInstance.m_bUnique || !m_bHandleDuplicates) {
    if(locLeft(r, iBegin, iEnd))
      return(true);
    if(locMid(r, iBegin, iEnd))
      return(true);
  }
  else {
    const Rectangle* pDuplicate = m_Duplicates.prev(r);
    if(pDuplicate == NULL || pDuplicate->yi.m_nBegin == 0)
      if(locLeft(r, iBegin, iEnd))
	return(true);
    if(pDuplicate) {
      if(locMid(r, pDuplicate->yi.m_nBegin, iBegin, iEnd))
	return(true);
    }
    else if(locMid(r, iBegin, iEnd))
      return(true);
  }
  if(locRight(r, iBegin, iEnd))
    return(true);
  return(false);
}

bool Duplicate::waste(RectPtrArray::iterator i,
		      const RectPtrArray::const_iterator& j) {
  if(m_vY.m_vWaste.canFit())
    if(searchSpace(i, j)) return(true);
  return(false);
}

bool Duplicate::locLeft(Rectangle* r, RectPtrArray::iterator& iBegin,
			const RectPtrArray::const_iterator& iEnd) {
  
  /**
   * Compute the last coordinate value representing the fact that
   * we're in the forbidden region (we still have to iterate over this
   * region in case there are some non-dominated positions), before
   * the coordinate value is considered to be in the interior of the
   * bins.
   */

  int nRealLast = std::min(m_vLastBinYI[r->m_nID],
			   m_pDomination->entriesw1(r) - 1);
  for(r->yi.init(0, r->m_nHeight, 0, nRealLast); r->yi.valid(nRealLast);
      r->yi.next(r->m_nHeight, nRealLast)) {
    if(m_vDominationLeft[r->m_nID] &&
       m_pDomination->dominatedw1(r, r->yi.m_nEnd)) continue;
    if(r->yi.m_nEnd > 0 && m_vDominationRight[r->m_nID] &&
       m_Box.m_nHeight - (r->yi.m_nEnd + r->m_nHeight) < m_pDomination->entriesw2(r) &&
       m_pDomination->dominatedw2(r, m_Box.m_nHeight - (r->yi.m_nEnd + r->m_nHeight)))
      continue;
    if((!m_vDominationLeft[r->m_nID] && r->yi.m_nEnd == 0) ||
       !m_vY.canFityi(r)) continue;
    m_Nodes.tick(YI);
    m_vY.m_vDomain[r->m_nID] = r->yi;
    m_vY.pushyi(r);
    if(m_vY.compress(*iBegin))
      if(waste(iBegin + 1, iEnd))
	return(true);
    m_vY.popyi(r);
  }
  return(false);
}

bool Duplicate::locMid(Rectangle* r, RectPtrArray::iterator& iBegin,
		       const RectPtrArray::const_iterator& iEnd) {
  for(r->yi.init(m_pDomination->entriesw1(r), r->m_nHeight,
		 m_pParams->m_nYScale,
		 m_vForbiddenStartYI[r->m_nID] - 1);
      r->yi.valid(m_vForbiddenStartYI[r->m_nID] - 1);
      r->yi.next(r->m_nHeight, m_vForbiddenStartYI[r->m_nID] - 1)) {
    if(!m_vY.canFityi(r)) continue;
    m_Nodes.tick(YI);
    m_vY.m_vDomain[r->m_nID] = r->yi;
    m_vY.pushyi(r);
    if(m_vY.compress(*iBegin))
      if(waste(iBegin + 1, iEnd))
	return(true);
    m_vY.popyi(r);
  }
  return(false);
}

bool Duplicate::locMid(Rectangle* r, UInt nMinY,
		       RectPtrArray::iterator& iBegin,
		       const RectPtrArray::const_iterator& iEnd) {
  UInt nStart = std::max(m_pDomination->entriesw1(r), nMinY);
  for(r->yi.init(nStart, r->m_nHeight, m_pParams->m_nYScale,
		 m_vForbiddenStartYI[r->m_nID] - 1);
      r->yi.valid(m_vForbiddenStartYI[r->m_nID] - 1);
      r->yi.next(r->m_nHeight, m_vForbiddenStartYI[r->m_nID] - 1)) {
    if(!m_vY.canFityi(r)) continue;
    m_Nodes.tick(YI);
    m_vY.m_vDomain[r->m_nID] = r->yi;
    m_vY.pushyi(r);
    if(m_vY.compress(*iBegin))
      if(waste(iBegin + 1, iEnd))
	return(true);
    m_vY.popyi(r);
  }
  return(false);
}

bool Duplicate::locRight(Rectangle* r, RectPtrArray::iterator& iBegin,
			 const RectPtrArray::const_iterator& iEnd) {
  int nStart = std::max(m_vForbiddenStartYI[r->m_nID],
			m_pDomination->entriesw1(r));
  for(r->yi.init(nStart, r->m_nHeight, 0, m_vLastBinYI[r->m_nID]);
      r->yi.valid(m_vLastBinYI[r->m_nID]);
      r->yi.next(r->m_nHeight, m_vLastBinYI[r->m_nID])) {
    if(r->yi.m_nEnd > 0 && m_vDominationRight[r->m_nID] &&
       m_Box.m_nHeight - (r->yi.m_nEnd + r->m_nHeight) < m_pDomination->entriesw2(r) &&
       m_pDomination->dominatedw2(r, m_Box.m_nHeight - (r->yi.m_nEnd + r->m_nHeight))) continue;
    if((!m_vDominationRight[r->m_nID] && r->yi.m_nEnd + r->m_nHeight == m_Box.m_nHeight) ||
       !m_vY.canFityi(r)) continue;
    m_Nodes.tick(YI);
    m_vY.m_vDomain[r->m_nID] = r->yi;
    m_vY.pushyi(r);
    if(m_vY.compress(*iBegin))
      if(waste(iBegin + 1, iEnd))
	return(true);
    m_vY.popyi(r);
  }
  return(false);
}

bool Duplicate::packcra() {
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

bool Duplicate::searchSpace(RectPtrArray::iterator iBegin,
			    const RectPtrArray::const_iterator& iEnd) {
  if(bQuit) return(false);
  if(iBegin == iEnd) {

    /**
     * Place all remaining strips that have not yet been placed.
     */

    RectPtrArray::iterator i = m_vRectPtrs.begin();
    for(; i != iBegin; ++i) {
      if(m_vY.m_vDomain[(*i)->m_nID].m_nWidth < (Int) (*i)->m_nHeight)
	return(strips(*i, iBegin, iEnd));
    }
    return(packX());
  }
  
  Rectangle* r(NULL);
  UInt nStripArea(bestStripArea(r));
  if(r && nStripArea > (*iBegin)->m_nArea)
    return(strips(r, iBegin, iEnd));
  else {
    r = *iBegin;
    m_vLastBinYI[r->m_nID] = m_Box.m_nHeight - r->m_nHeight;

    /**
     * The ForbiddenStartYI determines the coordinate at which we
     * start considering our packing attempt on the right side of the
     * box. If the forbidden region coming from the first edge
     * overlaps those from the second edge, then we assume that we'll
     * explore the forbidden region in totality from the first edge,
     * and simply shave off a little bit from the region coming from
     * the second edge. Of course, we need to also allow the very last
     * (against the wall) value, so we can't prune that value out.
     */

    if(m_vLastBinYI[r->m_nID] <= m_pDomination->entriesw2(r))
      m_vForbiddenStartYI[r->m_nID] = m_vLastBinYI[r->m_nID]; // Should this be 0?
    else
      m_vForbiddenStartYI[r->m_nID] =
	(m_vLastBinYI[r->m_nID] - m_pDomination->entriesw2(r) + 1);

    if(m_Box.canFit(r) && location(iBegin, iEnd))
      return(true);

    /**
     * Handle the unoriented case, if applicable.
     */

    if(r->m_bRotatable) {
      rotate(r);
      m_vLastBinYI[r->m_nID] = m_Box.m_nHeight - r->m_nHeight;
      m_vForbiddenStartYI[r->m_nID] =
	(m_vLastBinYI[r->m_nID] <= m_pDomination->entriesw2(r)) ? 0 :
	(m_vLastBinYI[r->m_nID] - m_pDomination->entriesw2(r) + 1);
      if(location(iBegin, iEnd))
	return(true);
    }
    return(false);
  }
}

UInt Duplicate::bestStripArea(Rectangle*& r) {
  if(!m_bVarOrder) return(std::numeric_limits<UInt>::max());
  
  UInt nMax(0);
  for(std::vector<const Rectangle*>::const_iterator i = m_vY.m_vRectangles.begin();
      i != m_vY.m_vRectangles.end(); ++i) {
    UInt t = (*i)->m_nWidth *
      ((*i)->m_nHeight - m_vY.m_vDomain[(*i)->m_nID].m_nWidth);
    if(t > nMax) {
      nMax = t;
      r = m_vRectPtrs[(*i)->m_nID];
    }
  }
  return(nMax);
}

bool Duplicate::strips(Rectangle* r, RectPtrArray::iterator iBegin,
		       const RectPtrArray::const_iterator& iEnd) {
  for(r->y = (UInt) m_vY.m_vDomain[r->m_nID].m_nBegin;
      r->y <= (UInt) m_vY.m_vDomain[r->m_nID].m_nEnd; ++r->y) {
    m_Nodes.tick(YF);
    if(!m_vY.canFitStrips(r)) continue;
    m_vY.pushStripsy(r);
    if(m_vY.compress())
      if(waste(iBegin, iEnd)) return(true);
    m_vY.popStripsy(r);
  }
  return(false);
}

void Duplicate::rotate(Rectangle* r) {
  r->rotate();
}

void Duplicate::checkDominationLeft(const Rectangle* r) {
  m_vDominationLeft[r->m_nID] = m_vY.canFit(r->m_nWidth, 0, r->m_nHeight);
}

void Duplicate::checkDomination(const Rectangle* r) {
  m_vDominationLeft[r->m_nID] =
    m_vY.canFit(r->m_nWidth, 0, r->m_nHeight);
  m_vDominationRight[r->m_nID] =
    m_vY.canFit(r->m_nWidth, m_Box.m_nHeight - r->m_nHeight, m_Box.m_nHeight);
}
