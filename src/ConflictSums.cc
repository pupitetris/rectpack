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
#include "ConflictSums.h"
#include "SimpleSums.h"
#include "SpaceFill.h"

ConflictSums::ConflictSums() :
  m_pWidths(NULL),
  m_pHeights(NULL),
  m_bUseMutexConstraints(true) {
}

ConflictSums::~ConflictSums() {
}

bool ConflictSums::packX() {
  m_nDeepestConflict = 0;
  UInt nTrash(0);
  return(nextVariable(m_vRectPtrs.begin(), nTrash));
}

void ConflictSums::initialize(const Parameters* pParams) {
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

  if(pParams->m_s1.find_first_of('m') < pParams->m_s1.size())
    m_bUseMutexConstraints = false;
}

void ConflictSums::initialize(const HeapBox* pBox) {
  RatPack::initialize(pBox);
  m_vDomains.initialize(this);
  m_vDomains.initialize(&pBox->m_Box);
  if(m_bUseMutexConstraints) {
    m_iWidth = m_pBoxes->m_Widths.find(pBox->m_Box.m_nWidth);
    m_iHeight = pBox->m_iHeight;
  }
}

void ConflictSums::initialize(const BoundingBoxes* pBoxes) {
  RatPack::initialize(pBoxes);
}

bool ConflictSums::packInterval(RectPtrArray::iterator i,
				UInt& nDeepest) {
  Rectangle* r(*i);
  nDeepest = std::max(nDeepest, r->m_nID);
  const DomConfig* dc(&m_vDomains.get(r)[0]); // Change 0 to note dominance rules.
  for(DomConfig::const_iterator j = dc->begin(); j != dc->end(); ++j) {
#ifdef DEBUG
    if(r->m_bFixed && !j->overlaps(r->m_nFixX))
      continue;
#endif // DEBUG
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

bool ConflictSums::packSingle(const Compulsory* c,
			      RectPtrArray::iterator i,
			      UInt& nDeepest) {

  /**
   * Generate the subset sums of importance within the purview of the
   * committed compulsory part and also excluding the current
   * rectangle. Process rectangles that have their coordinates
   * completely fixed.
   */

  SimpleSums ss;
  ss.insert(0);
  for(SingleStack::const_iterator j = m_vX.singles().begin();
      j != m_vX.singles().end(); ++j)
    if((*j)->m_nValue.right() <= c->left())
      ss.insert((*j)->m_nValue.right());
  
  UInt nNext;
  {

    /**
     * Add in rectangles who have location constraints due to their
     * compulsory parts. We only consider those compulsory parts which
     * are to the left of our current rectangle's range.
     */

    SimpleSums ss2(ss);
    for(FastRemove<CompulsoryAssign*>::const_iterator j = m_vX.intervals().begin();
	j != m_vX.intervals().end(); ++j)
      if((*j)->m_nValue.right() <= c->left())
	ss2.add((*j)->m_nValue.m_pRect->m_nWidth, c->left());

    /**
     * Add in rectangles with no location constraints.
     */

    for(RectPtrArray::iterator j = i; j != m_iFirstUnit; ++j) {
      if((*j)->rotatable())
	ss2.add((*j)->m_nWidth, (*j)->m_nHeight,
		c->m_nStart.m_nRight);
      else
	ss2.add((*j)->m_nWidth, c->m_nStart.m_nRight);
    }

    SimpleSums::const_iterator j(ss2.lower_bound(c->m_nStart.m_nLeft));
    if(j == ss2.end())
      nNext = c->m_nStart.m_nRight + 1;
    else
      nNext = *j;
  }

#if DEBUG
  if(c->m_pRect->m_bFixed)
    nNext = c->m_pRect->m_nFixX;
#endif // DEBUG

  nDeepest = std::max(nDeepest, c->m_pRect->m_nID);
  while(nNext <= c->m_nStart.m_nRight) {
    UInt nLocalDeepest = c->m_pRect->m_nID;
    m_Nodes.tick(XF);
    m_vX.push(c->m_pRect, nNext);
    if(m_vX.propagate())
      if(nextVariable(i, nLocalDeepest))
	return(true);
    m_vX.pop();
    nDeepest = std::max(nDeepest, nLocalDeepest);

    {
      SimpleSums ss2(ss); 
      for(FastRemove<CompulsoryAssign*>::const_iterator j = m_vX.intervals().begin();
	  j != m_vX.intervals().end(); ++j)
	if((*j)->m_nValue.right() <= c->left())
	  ss2.add((*j)->m_nValue.m_pRect->m_nWidth, c->left());
      for(RectPtrArray::iterator j = i;
	  j != m_iFirstUnit && (*j)->m_nID <= nLocalDeepest; ++j) {
	if((*j)->rotatable())
	  ss2.add((*j)->m_nWidth, (*j)->m_nHeight,
		  c->m_nStart.m_nRight);
	else
	  ss2.add((*j)->m_nWidth, c->m_nStart.m_nRight);
      }
      SimpleSums::const_iterator k(ss2.upper_bound(nNext));
      if(k == ss2.end())
	nNext = c->m_nStart.m_nRight + 1;
      else
	nNext = *k;
    }
  }
  return(false);
}

bool ConflictSums::orientation(RectPtrArray::iterator i,
			       UInt& nDeepest) {
  if(packInterval(i, nDeepest)) return(true);
  if((*i)->rotatable()) {
    (*i)->rotate();
    if(packInterval(i, nDeepest)) return(true);
  }
  return(false);
}

bool ConflictSums::nextVariable(RectPtrArray::iterator i, UInt& nDeepest) {
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

bool ConflictSums::packY() {
  if(m_pParams->m_bScheduling)
    return(true);
  else
    return(m_pSpaceFill->packY(m_vX, m_pHeights));
}
