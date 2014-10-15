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

#include "BranchingFactor.h"
#include "CoordinateRanges.h"
#include "Domination.h"
#include "IntPack.h"
#include "RectDomains.h"
#include <iomanip>

RectDomains::RectDomains() :
  m_pPacker(NULL) {
}

RectDomains::~RectDomains() {
}

void RectDomains::initialize(IntPack* pPacker,
			     const CoordinateRanges* pRanges,
			     const BranchingFactor* pBranches) {
  m_pPacker = pPacker;

  /**
   * Resize the data structure.
   */

  clear();
  resize(pPacker->m_iFirstUnit - pPacker->m_vRectPtrs.begin(),
	 RectDomain(2, IntConfig(4)));

  /**
   * Loop over every rectangle, loop over every orientation, loop over
   * every domination case, to compute the different interval values.
   */

  for(iterator i = begin(); i != end(); ++i) {
    Rectangle* r = pPacker->m_vRectPtrs[i - begin()];
    IntConfig* ic = &(i->operator[](r->m_bRotated));
    initialize(pPacker, r, pRanges, pBranches, ic);
    if(r->rotatable()) {
      r->rotate();
      ic = &(i->operator[](r->m_bRotated));
      initialize(pPacker, r, pRanges, pBranches, ic);
      r->rotate();
    }
  }
}

void RectDomains::initialize(IntPack* pPacker, Rectangle* r,
			     const CoordinateRanges* pRanges,
			     const BranchingFactor* pBranches,
			     IntConfig* ic) {
  for(size_t nConfig = 0; nConfig < 4; ++nConfig) {

    /**
     * Retrieve the appropriate branch and the data structure in which
     * we will be inserting our computed values.
     */

    const std::vector<Range>* pRanges2 = &pRanges->get(r, nConfig);
    IntDomains* id = &ic->operator[](nConfig);
    bool bDomLeft(nConfig <= 2), bDomRight(nConfig % 2 == 0);

    /**
     * Left.
     */

    Interval i;
    const Range* pRange = &pRanges2->operator[](0);
    if(pRange->nonempty())
      for(i.init(pRange->lb(), r->m_nHeight, 0, pRange->ub());
	  i.valid(pRange->ub());
	  i.next(r->m_nHeight, pRange->ub())) {
	if(bDomLeft && pPacker->m_pDomination->dominatedw1(r, i.m_nEnd)) continue;
	if(i.m_nEnd > 0 && bDomRight &&
	   pPacker->m_Box.m_nHeight - (i.m_nEnd + r->m_nHeight) <
	   pPacker->m_pDomination->entriesw2(r) &&
	   pPacker->m_pDomination->dominatedw2(r, pPacker->m_Box.m_nHeight -
					       (i.m_nEnd + r->m_nHeight)))
	  continue;
	if(!bDomLeft && i.m_nEnd == 0) continue;
	id->push_back(i);
      }
    
    /**
     * Middle.
     */

    pRange = &pRanges2->operator[](1);
    if(pRange->nonempty()) {
      size_t nBranches = r->m_bRotated ?
	pBranches->operator[](r->m_nID).m_pRBranches[nConfig][1] :
	pBranches->operator[](r->m_nID).m_pBranches[nConfig][1];
      if(pRange->size() < (int) nBranches) nBranches = pRange->size();
      for(size_t j = 0; j < nBranches; ++j) {
	i.set(r, pRange->lb(), pRange->ub(), j, nBranches);
	id->push_back(i);
      }
    }

    /**
     * Right.
     */
    
    pRange = &pRanges2->operator[](2);
    if(pRange->nonempty())
      for(i.init(pRange->lb(), r->m_nHeight, 0, pRange->ub());
	  i.valid(pRange->ub());
	  i.next(r->m_nHeight, pRange->ub())) {
	if(i.m_nEnd > 0 && bDomRight &&
	   pPacker->m_Box.m_nHeight - (i.m_nEnd + r->m_nHeight) <
	   pPacker->m_pDomination->entriesw2(r) &&
	   pPacker->m_pDomination->dominatedw2(r, pPacker->m_Box.m_nHeight -
					       (i.m_nEnd + r->m_nHeight))) continue;
	if(!bDomRight && i.m_nEnd + r->m_nHeight == pPacker->m_Box.m_nHeight) continue;
	id->push_back(i);
      }
  }
}

std::vector<Interval>& RectDomains::get(const Rectangle* r, size_t nConfig) {
  return((*this)[r->m_nID][r->m_bRotated][nConfig]);
}

void RectDomains::print() const {
  for(size_t i = 0; i < size(); ++i) {
    const Rectangle* r = m_pPacker->m_vRectPtrs[i];
    std::cout << std::setw(2) << r->m_nWidth << "x"
	      << std::setw(2) << r->m_nHeight << ", ";
    print(operator[](i)[r->m_bRotated]);
    if(r->rotatable()) {
      std::cout << std::setw(2) << r->m_nHeight << "x"
		<< std::setw(2) << r->m_nWidth << ", ";
      print(operator[](i)[!r->m_bRotated]);
    }
  }
}

void RectDomains::print(const IntConfig& ic) const {
  const char s[4][3] = {"TT", "TF", "FT", "FF"};
  for(size_t i = 0; i < 4; ++i) {
    if(i > 0) std::cout << "       ";
    std::cout << s[i] << ":";
    print(ic[i]);
    std::cout << std::endl;
  }
}

void RectDomains::print(const IntDomains& id) const {
  for(IntDomains::const_iterator i = id.begin(); i != id.end(); ++i)
    std::cout << "  [" << std::setw(2) << i->m_nBegin
	      << "," << std::setw(2) << i->m_nEnd << "]";
}
