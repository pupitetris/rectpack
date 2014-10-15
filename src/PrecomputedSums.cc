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
#include "Globals.h"
#include "HeapBox.h"
#include "Parameters.h"
#include "PrecomputedSums.h"
#include "SpaceFill.h"

PrecomputedSums::PrecomputedSums() :
  m_pWidths(NULL),
  m_pHeights(NULL) {
}

PrecomputedSums::~PrecomputedSums() {
}

bool PrecomputedSums::packX() {
  m_nDeepestConflict = 0;
  return(nextVariable(m_vRectPtrs.begin()));
}

void PrecomputedSums::initialize(const Parameters* pParams) {
  RatPack::initialize(pParams);
  m_vDomains.initialize(pParams);

  /**
   * If the subset sums computed by the bounding boxes is invalid,
   * we'll have to compute our own. Otherwise, we can use the
   * precomputed sums.
   */

  if(m_pBoxes == NULL ||
     m_pBoxes->m_bAllIntegralBoxes ||
     m_pBoxes->m_Widths.empty() ||
     m_pBoxes->m_Heights.empty()) {
    m_Widths.initializeW(m_vRects.begin(), m_vRects.end());
    m_pWidths = &m_Widths;
    m_Heights.initializeH(m_vRects.begin(), m_vRects.end());
    m_pHeights = &m_Heights;
  }
  else {
    m_pWidths = &m_pBoxes->m_Widths;
    m_pHeights = &m_pBoxes->m_Heights;
  }
}

void PrecomputedSums::initialize(const HeapBox* pBox) {
  RatPack::initialize(pBox);
  m_vDomains.initialize(this);
  m_vDomains.initialize(&pBox->m_Box);
}

void PrecomputedSums::initialize(const BoundingBoxes* pBoxes) {
  RatPack::initialize(pBoxes);
}

bool PrecomputedSums::packInterval(RectPtrArray::iterator i) {
  Rectangle* r(*i);
  const DomConfig* dc(&m_vDomains.get(r)[0]); // Change 0 to note dominance rules.
  for(DomConfig::const_iterator j = dc->begin(); j != dc->end(); ++j) {
    if(m_vX.canFit(*j)) {
      m_Nodes.tick(XI);
      m_vX.pushHinted(*j);
      if(m_vX.propagate())
	if(nextVariable(i + 1))
	  return(true);
      m_vX.pop();
    }
  }
  return(false);
}

bool PrecomputedSums::packSingle(const Compulsory* c,
				 RectPtrArray::iterator i) {
  SubsetSums::const_iterator j(m_pWidths->lower_bound(c->m_nStart.m_nLeft));
  SubsetSums::const_iterator k(m_pWidths->lower_bound(c->m_nStart.m_nRight));
  if(k != m_pWidths->end() && k->first == c->m_nStart.m_nRight) ++k;
  for(; j != k; ++j) {
    
    /**
     * If this rectangle is required to form this subset sum, then we
     * move on to the next value.
     */

    if(j->second.find(c->m_pRect) != j->second.end())
      continue;

    /**
     * Otherwise, try to place this rectangle.
     */

    m_Nodes.tick(XF);
    m_vX.push(c->m_pRect, j->first);
    if(m_vX.propagate())
      if(nextVariable(i))
	return(true);
    m_vX.pop();
  }
  return(false);
}

bool PrecomputedSums::orientation(RectPtrArray::iterator i) {
  if(packInterval(i)) return(true);
  if((*i)->rotatable()) {
    (*i)->rotate();
    if(packInterval(i)) return(true);
  }
  return(false);
}

bool PrecomputedSums::nextVariable(RectPtrArray::iterator i) {
  if(bQuit) return(false);

  /**
   * In the base case we're at the end of our packing sequence.
   */

  if(i == m_iFirstUnit) {
    m_nDeepestConflict = m_vRectPtrs.size() - 1;

    /**
     * We're potentially done with everything. Before we pack the
     * other dimension, however, we must ensure that we have
     * completely finished with packing even the singular values.
     */

    const Compulsory* c(NULL);
    UInt nLargestArea(0);
    m_vX.largestAreaPlacement(nLargestArea, c);
    if(c) return(packSingle(c, i));
    else return(packY());
  }

  m_nDeepestConflict = std::max(m_nDeepestConflict, (*i)->m_nID);
  const Compulsory* c(NULL);
  UInt nLargestArea(0);
  m_vX.largestAreaPlacement(nLargestArea, c);
  if(c && nLargestArea > m_vDomains.get(*i)[0][0].area())
    return(packSingle(c, i));
  else return(orientation(i));
}

bool PrecomputedSums::packY() {
  if(m_pParams->m_bScheduling)
    return(true);
  else
    return(m_pSpaceFill->packY(m_vX, m_pHeights));
}
