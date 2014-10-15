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
#include "ConflictBT.h"
#include "SimpleSums.h"
#include "SpaceFill.h"

ConflictBT::ConflictBT() :
  m_pWidths(NULL),
  m_pHeights(NULL) {
}

ConflictBT::~ConflictBT() {
}

bool ConflictBT::packX() {
  m_nDeepestConflict = 0;
  UInt nTrash(0);
  m_vSums.clear();
  m_vSums.insert(0);
  m_vSums.push();
  return(nextVariable(m_vRectPtrs.begin(), nTrash));
}

void ConflictBT::initialize(const Parameters* pParams) {
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

void ConflictBT::initialize(const HeapBox* pBox) {
  RatPack::initialize(pBox);
  m_vDomains.initialize(this);
  m_vDomains.initialize(&pBox->m_Box);
}

void ConflictBT::initialize(const BoundingBoxes* pBoxes) {
  RatPack::initialize(pBoxes);
}

bool ConflictBT::packInterval(RectPtrArray::iterator i,
				UInt& nDeepest) {
  Rectangle* r(*i);
  const DomConfig* dc(&m_vDomains.get(r)[0]); // Change 0 to note dominance rules.
  for(DomConfig::const_iterator j = dc->begin(); j != dc->end(); ++j) {
    if(m_vX.canFit(*j)) {
      m_Nodes.tick(XI);
      m_vX.pushHinted(*j);
      if(m_vX.propagate())
	if(nextVariable(i + 1, nDeepest))
	  return(true);
      m_vX.pop();
    }
  }
  return(false);
}

bool ConflictBT::packSingle(const Compulsory* c,
			    RectPtrArray::iterator i,
			    UInt& nDeepest) {

  /**
   * Now process all rectangles that haven't gotten any commitments
   * made.
   */

  UInt nNext;
  {
    SimpleSums ss2(m_vSums);
    for(RectPtrArray::iterator i2 = i; i2 != m_iFirstUnit; ++i2) {
      if((*i2)->rotatable())
	ss2.add((*i2)->m_nWidth, c->m_nStart.m_nRight);
      else
	ss2.add((*i2)->m_nWidth, (*i2)->m_nHeight,
		c->m_nStart.m_nRight);
    }
    for(UInt m = c->m_pRect->m_nID + 1; m < (UInt) (i - m_vRectPtrs.begin()); ++m)
      ss2.add(m_vX.values()[m].m_nValue, c->m_nStart.m_nRight);
    SimpleSums::const_iterator j(ss2.lower_bound(c->m_nStart.m_nLeft));
    if(j == ss2.end())
      nNext = c->m_nStart.m_nRight + 1;
    else
      nNext = *j;
  }

  nDeepest = std::max(nDeepest, c->m_pRect->m_nID);
  while(nNext <= c->m_nStart.m_nRight) {
    UInt nLocalDeepest = c->m_pRect->m_nID;
    m_Nodes.tick(XF);
    m_vSums.push(nNext + c->m_pRect->m_nWidth);
    m_vX.push(c->m_pRect, nNext);
    if(m_vX.propagate())
      if(nextVariable(i, nLocalDeepest))
	return(true);
    m_vX.pop();
    m_vSums.pop();
    nDeepest = std::max(nDeepest, nLocalDeepest);

    {
      SimpleSums ss2(m_vSums);
      for(RectPtrArray::iterator i2 = i;
	  i2 != m_iFirstUnit && (*i2)->m_nID <= nLocalDeepest; ++i2) {
	if((*i2)->rotatable())
	  ss2.add((*i2)->m_nWidth, c->m_nStart.m_nRight);
	else
	  ss2.add((*i2)->m_nWidth, (*i2)->m_nHeight,
		  c->m_nStart.m_nRight);
      }
      for(UInt m = c->m_pRect->m_nID + 1; m < (UInt) (i - m_vRectPtrs.begin()); ++m)
	ss2.add(m_vX.values()[m].m_nValue, c->m_nStart.m_nRight);
      SimpleSums::const_iterator k(ss2.upper_bound(nNext));
      if(k == ss2.end())
	nNext = c->m_nStart.m_nRight + 1;
      else
	nNext = *k;
    }
  }
  return(false);
}

bool ConflictBT::orientation(RectPtrArray::iterator i,
			       UInt& nDeepest) {
  if(packInterval(i, nDeepest)) return(true);
  if((*i)->rotatable()) {
    (*i)->rotate();
    if(packInterval(i, nDeepest)) return(true);
  }
  return(false);
}

bool ConflictBT::nextVariable(RectPtrArray::iterator i, UInt& nDeepest) {
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
    if(c) return(packSingle(c, i, nDeepest));
    else return(packY());
  }

  m_nDeepestConflict = std::max(m_nDeepestConflict, (*i)->m_nID);
  const Compulsory* c(NULL);
  UInt nLargestArea(0);
  m_vX.largestAreaPlacement(nLargestArea, c);
  if(c && nLargestArea > m_vDomains.get(*i)[0][0].area())
    return(packSingle(c, i, nDeepest));
  else return(orientation(i, nDeepest));
}

bool ConflictBT::packY() {
  if(m_pParams->m_bScheduling)
    return(true);
  else
    return(m_pSpaceFill->packY(m_vX, m_pHeights));
}
