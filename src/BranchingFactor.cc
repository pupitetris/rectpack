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

#include "BoxDimensions.h"
#include "BranchingFactor.h"
#include "CoordinateRanges.h"
#include "Domination.h"
#include "Range.h"

BranchingFactor::BranchingFactor() {
}

BranchingFactor::~BranchingFactor() {
}

void BranchingFactor::initialize(const Domination* pDomination,
				 const BoxDimensions* pBox,
				 RectPtrArray::iterator iBegin,
				 RectPtrArray::iterator iEnd,
				 const CoordinateRanges* pRanges) {
  clear();
  resize(iEnd - iBegin);
  for(; iBegin != iEnd; ++iBegin) {
    Rectangle* r = *iBegin;
    BranchDesc* bd = &operator[](r->m_nID);
    bd->m_pRect = r;
    for(UInt j = 0; j < 4; ++j) {
      UInt *pTotal, *pBranches;
      if(r->m_bRotated) {
	pTotal = &bd->m_pRTotal[j];
	pBranches = bd->m_pRBranches[j];
      }
      else {
	pTotal = &bd->m_pTotal[j];
	pBranches = bd->m_pBranches[j];
      }
      if(pDomination && pDomination->enabled())
	initialize(pDomination, pBox, pRanges->get(r, j), r, pTotal,
		   pBranches);
      else initialize(pBox, pRanges->get(r, j), r, pTotal,
		      pBranches);

      if(r->rotatable()) {
	r->rotate();
	if(r->m_bRotated) {
	  pTotal = &bd->m_pRTotal[j];
	  pBranches = bd->m_pRBranches[j];
	}
	else {
	  pTotal = &bd->m_pTotal[j];
	  pBranches = bd->m_pBranches[j];
	}
	if(pDomination && pDomination->enabled())
	  initialize(pDomination, pBox, pRanges->get(r, j), r, pTotal,
		     pBranches);
	else initialize(pBox, pRanges->get(r, j), r, pTotal,
			pBranches);
	r->rotate();
      }
    }
  }
  //std::stable_sort(begin(), end());
}

void BranchingFactor::initialize(const Domination* pDomination,
				 const BoxDimensions* pBox,
				 const std::vector<Range>& vRanges,
				 const Rectangle* r,
				 UInt* pTotal,
				 UInt* pBranches) const {
  /**
   * Compute the interval size.
   */

  UInt nInterval =  r->m_nHeight *
    r->scale().get_num() / r->scale().get_den();
  if(nInterval < 1) nInterval = 1;

  pBranches[0] = 0;
  if(vRanges[0].nonempty()) {
    for(UInt y = vRanges[0].lb(); y <= vRanges[0].ub(); ++y)
      if(y == 0 || !pDomination->dominatedw1(r, y))
	++pBranches[0];
  }

  if(vRanges[1].nonempty()) {
    pBranches[1] = vRanges[1].size() / nInterval;
    if(vRanges[1].size() % nInterval > 0) ++pBranches[1];
    if((int) pBranches[1] > vRanges[1].size())
      pBranches[1] = vRanges[1].size();
  }

  pBranches[2] = 0;
  if(vRanges[2].nonempty()) {
    for(UInt y = vRanges[2].lb(); y <= vRanges[2].ub(); ++y) {
      UInt nGap = pBox->m_nHeight - (y + r->m_nHeight);
      if(y == 0 || nGap >= pDomination->entriesw2(r) ||
	 !pDomination->dominatedw2(r, nGap))
	++pBranches[2];
    }
  }
  (*pTotal) = pBranches[0] + pBranches[1] + pBranches[2];
}

void BranchingFactor::initialize(const BoxDimensions* pBox,
				 const std::vector<Range>& vRanges,
				 const Rectangle* r,
				 UInt* pTotal,
				 UInt* pBranches) const {
  /**
   * Compute the interval size.
   */

  UInt nInterval =  r->m_nHeight *
    r->scale().get_num() / r->scale().get_den();
  if(nInterval < 1) nInterval = 1;

  pBranches[0] = 0;
  if(vRanges[0].nonempty()) pBranches[0] = vRanges[0].size();

  pBranches[1] = 0;
  if(vRanges[1].nonempty()) {
    pBranches[1] = vRanges[1].size() / nInterval;
    if(vRanges[1].size() % nInterval > 0) ++pBranches[1];
  }

  pBranches[2] = 0;
  if(vRanges[2].nonempty()) pBranches[2] = vRanges[2].size();

  (*pBranches) = pBranches[0] + pBranches[1] + pBranches[2];
}

void BranchingFactor::print() const {
  for(UInt i = 0; i < size(); ++i)
    std::cout << operator[](i);
}

Rectangle* BranchingFactor::firstSymmetry(const BoxDimensions& b) const {

  /**
   * The optimistic case is that we break symmetry for both
   * orientations of the rectangle.
   */

  for(const_iterator i = begin(); i != end(); ++i)
    if(i->m_pTotal[0] != 1 && i->m_pRTotal[0] != 1) {
      //std::cout << "Breaking symmetry at rectangle " << i - begin() << "." << std::endl;
      return(i->m_pRect);
    }

  /**
   * If there are no such rectangles we will now fall back to the less
   * optimistic case where there is just one orientation which would
   * benefit.
   */

  for(const_iterator i = begin(); i != end(); ++i)
    if(i->m_pTotal[0] > 1 || i->m_pRTotal[0] > 1) {
      //std::cout << "Breaking symmetry at rectangle " << i - begin() << "." << std::endl;
      return(i->m_pRect);
    }

  /**
   * If there are only single branches in all rectangles, we'll just
   * break symmetry at the first rectangle that has more than one
   * singular value.
   */

  for(const_iterator i = begin(); i != end(); ++i)
    if(i->m_pRect->m_nHeight < b.m_nHeight) {
      //std::cout << "Breaking symmetry at rectangle " << i - begin() << "." << std::endl;
      return(i->m_pRect);
    }

  /**
   * Just break symmetry at the root.
   */

  //std::cout << "Breaking symmetry at rectangle 0." << std::endl;
  return(front().m_pRect);
}
