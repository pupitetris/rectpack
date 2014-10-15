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
#include "CorrectedIjcai.h"
#include <iostream>
#include <algorithm>
#include <iomanip>

CorrectedIjcai::CorrectedIjcai() :
  m_bVarOrder(true),
  m_bDomBasedRanges(true) {
}

CorrectedIjcai::~CorrectedIjcai() {
}

void CorrectedIjcai::initialize(const Parameters* pParams) {
  IntPack::initialize(pParams);

  /**
   * Disable any features as requested by the user.
   */

  m_bVarOrder = (pParams->m_s1.find_first_of('v') >= pParams->m_s1.size());
  m_bDomBasedRanges = (pParams->m_s1.find_first_of('r') >= pParams->m_s1.size());

#ifdef VIZ
  m_State.set(&m_vY, YI);
#endif // VIZ
  m_Ranges.initialize(pParams);
}

void CorrectedIjcai::initialize(const HeapBox* pBox) {
  IntPack::initialize(pBox);

  /**
   * Precompute some values that depend only on the bounding box.
   */

  if(m_pDomination && m_pDomination->enabled() && m_bDomBasedRanges)
    m_Ranges.initialize(m_vRectPtrs, m_Box, m_pDomination);
  else
    m_Ranges.initializeSimple(m_vRectPtrs, m_Box,
			      m_pParams->breakVerticalSymmetry());
}

bool CorrectedIjcai::pack() {

  /**
   * The actual packing attempt.
   */

  Rectangle* r = m_vRectPtrs.front();
  if(packY(m_vRectPtrs.begin(), m_iFirstUnit)) return(true);
  if(r->rotatable() && !m_Box.square()) {
    rotate(r);
    if(packY(m_vRectPtrs.begin(), m_iFirstUnit)) return(true);
  }
  return(false);
}

bool CorrectedIjcai::packY(RectPtrArray::iterator iBegin,
			   const RectPtrArray::const_iterator& iEnd) {
  if(iBegin == iEnd) return(packX());
  Rectangle* r(*iBegin);

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
  if(m_pParams->breakVerticalSymmetry())
    m_Ranges.breakVerticalSymmetry(r, m_Box, m_pDomination);
  const Range* pRange = &m_Ranges.left(r, true, true);
  if(pRange->nonempty())
    for(r->yi.init(0, r->m_nHeight, 0, pRange->ub());
	r->yi.valid(pRange->ub());
	r->yi.next(r->m_nHeight, pRange->ub())) {
      if(r->yi.m_nEnd > 0 && m_Ranges.m_vDomLeft[r->m_nID] &&
	 m_pDomination->dominatedw1(r, r->yi.m_nEnd)) continue;
      m_Nodes.tick(YI);
      m_vY.m_vDomain[r->m_nID] = r->yi;
      m_vY.pushyi(r);
      if(waste(iBegin + 1, iEnd))
	return(true);
      m_vY.popyi(r);
    }

  pRange = &m_Ranges.middle(r, true, true);
  if(pRange->nonempty())
    for(r->yi.init(pRange->lb(), r->m_nHeight, r->scale().get_f(),
		   pRange->ub());
	r->yi.valid(pRange->ub());
	r->yi.next(r->m_nHeight, pRange->ub())) {
      m_Nodes.tick(YI);
      m_vY.m_vDomain[r->m_nID] = r->yi;
      m_vY.pushyi(r);
      if(waste(iBegin + 1, iEnd))
	return(true);
      m_vY.popyi(r);
    }

  if(!m_pParams->breakVerticalSymmetry()) {
    pRange = &m_Ranges.right(r, true, true);
    if(pRange->nonempty())
      for(r->yi.init(pRange->lb(), r->m_nHeight, 0, pRange->ub());
	  r->yi.valid(pRange->ub());
	  r->yi.next(r->m_nHeight, pRange->ub())) {
	if(r->yi.m_nEnd > 0 &&
	   m_Box.m_nHeight - (r->yi.m_nEnd + r->m_nHeight) < m_pDomination->entriesw2(r) &&
	   m_pDomination->dominatedw2(r, m_Box.m_nHeight - (r->yi.m_nEnd + r->m_nHeight))) continue;
	m_Nodes.tick(YI);
	m_vY.m_vDomain[r->m_nID] = r->yi;
	m_vY.pushyi(r);
	if(waste(iBegin + 1, iEnd))
	  return(true);
	m_vY.popyi(r);
      }
  }
  return(false);
}

bool CorrectedIjcai::location(RectPtrArray::iterator iBegin,
			      const RectPtrArray::const_iterator& iEnd) {
  Rectangle* r(*iBegin);
  m_Ranges.checkDom(r, m_vY, m_Box);
  UInt nMinY(0);
  const Rectangle* pDuplicate = m_Duplicates.prev(r);
  if(pDuplicate && pDuplicate->yi.m_nBegin > 0) {
    nMinY = pDuplicate->yi.m_nBegin;
    m_Ranges.m_vDomLeft[r->m_nID] = false;
  }
  if(locLeft(r, iBegin, iEnd))
    return(true);
  if(locMid(r, nMinY, iBegin, iEnd))
    return(true);
  if(locRight(r, nMinY, iBegin, iEnd))
    return(true);
  return(false);
}

bool CorrectedIjcai::waste(RectPtrArray::iterator i,
			   const RectPtrArray::const_iterator& j) {
  if(m_vY.m_vWaste.canFit())
    if(searchSpace(i, j)) return(true);
  return(false);
}

bool CorrectedIjcai::locLeft(Rectangle* r, RectPtrArray::iterator& iBegin,
			     const RectPtrArray::const_iterator& iEnd) {
  
  /**
   * Compute the last coordinate value representing the fact that
   * we're in the forbidden region (we still have to iterate over this
   * region in case there are some non-dominated positions), before
   * the coordinate value is considered to be in the interior of the
   * bins.
   */

  const Range* pRange = &m_Ranges.left(r);
  if(pRange->nonempty())
    for(r->yi.init(pRange->lb(), r->m_nHeight, 0, pRange->ub());
	r->yi.valid(pRange->ub());
	r->yi.next(r->m_nHeight, pRange->ub())) {
      if(m_Ranges.m_vDomLeft[r->m_nID] &&
	 m_pDomination->dominatedw1(r, r->yi.m_nEnd)) continue;
      if(r->yi.m_nEnd > 0 && m_Ranges.m_vDomRight[r->m_nID] &&
	 m_Box.m_nHeight - (r->yi.m_nEnd + r->m_nHeight) < m_pDomination->entriesw2(r) &&
	 m_pDomination->dominatedw2(r, m_Box.m_nHeight - (r->yi.m_nEnd + r->m_nHeight)))
	continue;
      if((!m_Ranges.m_vDomLeft[r->m_nID] && r->yi.m_nEnd == 0) ||
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

bool CorrectedIjcai::locMid(Rectangle* r, UInt nMinY,
			    RectPtrArray::iterator& iBegin,
			    const RectPtrArray::const_iterator& iEnd) {
  const Range* pRange = &m_Ranges.middle(r);
  if(pRange->nonempty()) {
    UInt nStart = std::max(pRange->lb(), nMinY);
    for(r->yi.init(nStart, r->m_nHeight, r->scale().get_f(),
		   pRange->ub());
	r->yi.valid(pRange->ub());
	r->yi.next(r->m_nHeight, pRange->ub())) {
      if(!m_vY.canFityi(r)) continue;
      m_Nodes.tick(YI);
      m_vY.m_vDomain[r->m_nID] = r->yi;
      m_vY.pushyi(r);
      if(m_vY.compress(*iBegin))
	if(waste(iBegin + 1, iEnd))
	  return(true);
      m_vY.popyi(r);
    }
  }
  return(false);
}

bool CorrectedIjcai::locRight(Rectangle* r, UInt nMinY,
			      RectPtrArray::iterator& iBegin,
			      const RectPtrArray::const_iterator& iEnd) {
  const Range* pRange = &m_Ranges.right(r);
  if(pRange->nonempty()) {
    UInt nStart = std::max(pRange->lb(), nMinY);
    for(r->yi.init(nStart, r->m_nHeight, 0, pRange->ub());
	r->yi.valid(pRange->ub());
	r->yi.next(r->m_nHeight, pRange->ub())) {
      if(r->yi.m_nEnd > 0 && m_Ranges.m_vDomRight[r->m_nID] &&
	 m_Box.m_nHeight - (r->yi.m_nEnd + r->m_nHeight) < m_pDomination->entriesw2(r) &&
	 m_pDomination->dominatedw2(r, m_Box.m_nHeight - (r->yi.m_nEnd + r->m_nHeight))) continue;
      if((!m_Ranges.m_vDomRight[r->m_nID] && r->yi.m_nEnd + r->m_nHeight == m_Box.m_nHeight) ||
	 !m_vY.canFityi(r)) continue;
      m_Nodes.tick(YI);
      m_vY.m_vDomain[r->m_nID] = r->yi;
      m_vY.pushyi(r);
      if(m_vY.compress(*iBegin))
	if(waste(iBegin + 1, iEnd))
	  return(true);
      m_vY.popyi(r);
    }
  }
  return(false);
}

bool CorrectedIjcai::packcra() {
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

bool CorrectedIjcai::searchSpace(RectPtrArray::iterator iBegin,
				 const RectPtrArray::const_iterator& iEnd) {
  if(bQuit) return(false);
  if(iBegin == iEnd) {
    RectPtrArray::iterator i = m_vRectPtrs.begin();
    for(; i != iBegin; ++i) {
      if(m_vY.m_vDomain[(*i)->m_nID].m_nWidth < (int) (*i)->m_nHeight)
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
    if(location(iBegin, iEnd))
      return(true);

    /**
     * Handle the unoriented case, if applicable.
     */

    if(r->m_bRotatable) {
      rotate(r);
      if(location(iBegin, iEnd))
	return(true);
      rotate(r);
    }
    return(false);
  }
}

UInt CorrectedIjcai::bestStripArea(Rectangle*& r) {
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

bool CorrectedIjcai::strips(Rectangle* r, RectPtrArray::iterator iBegin,
			    const RectPtrArray::const_iterator& iEnd) {
  for(r->y = (UInt) m_vY.m_vDomain[r->m_nID].m_nBegin;
      r->y <= (UInt) m_vY.m_vDomain[r->m_nID].m_nEnd; ++r->y) {
    m_Nodes.tick(YF);

    /**
     * If the left/middle/right ranges don't consider the dominated
     * regions, then it's possible that we may have a domain that
     * contains some dominated positions. In such a case we can skip
     * over certain value assignments if that position were dominated.
     */

    if(!m_bDomBasedRanges) {
      if(m_Ranges.m_vDomLeft[r->m_nID] &&
	 m_pDomination->dominatedw1(r, r->y)) continue;
      if(r->y > 0 && m_Ranges.m_vDomRight[r->m_nID] &&
	 m_Box.m_nHeight - (r->y + r->m_nHeight) < m_pDomination->entriesw2(r) &&
	 m_pDomination->dominatedw2(r, m_Box.m_nHeight - (r->y + r->m_nHeight)))
	continue;
    }

    if(!m_vY.canFitStrips(r)) continue;
    m_vY.pushStripsy(r);
    if(m_vY.compress())
      if(waste(iBegin, iEnd)) return(true);
    m_vY.popStripsy(r);
  }
  return(false);
}

void CorrectedIjcai::rotate(Rectangle* r) {
  r->rotate();
}
