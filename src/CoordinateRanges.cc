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

#include "Bins.h"
#include "BoxDimensions.h"
#include "CoordinateRanges.h"
#include "Domination.h"
#include "Parameters.h"
#include "Rectangle.h"
#include "RectPtrArray.h"

CoordinateRanges::CoordinateRanges() :
  m_pArray(NULL) {
}

CoordinateRanges::~CoordinateRanges() {
}

void CoordinateRanges::initialize(const Parameters* pParams) {
  m_pParams = pParams;
  m_vDomLeft.clear();
  m_vDomLeft.resize(pParams->m_vInstance.size(), true);
  m_vDomRight.clear();
  m_vDomRight.resize(pParams->m_vInstance.size(), true);
}

void CoordinateRanges::initialize(Rectangle* r,
				  std::vector<std::vector<Range> >* t,
				  BoxDimensions& b, UInt nGap) {
  std::vector<Range>* u(NULL);
    
  /**
   * Process the first case where both dominated positions are
   * available. We assert the left and right ranges first, and if
   * they overlap we prefer exploration in the left range as much as
   * possible. However, if the left gap completely accounts for the
   * total range, we'll give the right side a single value (taking
   * it from the left). Finally we set the Interior Interval.
   */

  u = &((*t)[TT]);
  (*u)[LEFT].lb() = 0;
  (*u)[LEFT].ub() = nGap;
  (*u)[RIGHT].ub() = b.m_nHeight - r->m_nHeight;
  (*u)[RIGHT].lb() = b.m_nHeight - r->m_nHeight - nGap;
  if(r->m_nHeight == b.m_nHeight) { // Height takes entire range.
    (*u)[LEFT].ub() = 0;
    (*u)[RIGHT].setInvalid();
    (*u)[MIDDLE].setInvalid();
  }
  else if(nGap + r->m_nHeight >= b.m_nHeight) { // Left gap takes entire range.
    (*u)[LEFT].ub() = b.m_nHeight - r->m_nHeight;
    (*u)[RIGHT].setInvalid();
    (*u)[MIDDLE].setInvalid();
  }
  else if((nGap + r->m_nHeight + nGap) >= b.m_nHeight) { // Both gaps overlap.
    (*u)[RIGHT].lb() = (*u)[LEFT].ub() + 1;
    (*u)[MIDDLE].setInvalid();
  }
  else { // General case where there's enough room for both gaps.
    (*u)[MIDDLE].lb() = (*u)[LEFT].ub() + 1;
    (*u)[MIDDLE].ub() = (*u)[RIGHT].lb() - 1;
  }

  /**
   * The second case is where the left gap is valid but the right
   * gap is not. This immediately means that we'll allow the
   * Interval to handle the right side then and make the right side
   * invalid. As an optimization, since we've already checked that
   * right dominated position, we won't explore a value that
   * includes that.
   */

  u = &((*t)[TF]);
  (*u)[LEFT].lb() = 0;
  (*u)[MIDDLE].ub() = b.m_nHeight - r->m_nHeight - 1;
  (*u)[RIGHT].setInvalid();
  if(r->m_nHeight == b.m_nHeight) {
    (*u)[LEFT].setInvalid();
    (*u)[MIDDLE].setInvalid();
  }
  else if(r->m_nHeight + 1 == b.m_nHeight) {
    (*u)[LEFT].setInvalid();
    (*u)[MIDDLE].lb() = 0;
  }
  else if((nGap + r->m_nHeight) >= (b.m_nHeight - 1)) { // Limiting factor is box.
    (*u)[LEFT].ub() = b.m_nHeight - 1 - r->m_nHeight;
    (*u)[MIDDLE].setInvalid();
  }
  else {
    (*u)[LEFT].ub() = nGap;
    (*u)[MIDDLE].lb() = nGap + 1;
    (*u)[MIDDLE].ub() = b.m_nHeight - r->m_nHeight - 1;
  }

  /**
   * Consider the case where the left dominated position is
   * unavailable but the right dominated position is still
   * available. In that case the middle Interval section should
   * completely take over for the left side coordinates (excluding
   * y=0, of course).
   */

  u = &((*t)[FT]);
  (*u)[LEFT].setInvalid();
  (*u)[MIDDLE].lb() = 1;
  (*u)[RIGHT].ub() = b.m_nHeight - r->m_nHeight;
  if(r->m_nHeight == b.m_nHeight) {
    (*u)[MIDDLE].setInvalid();
    (*u)[RIGHT].setInvalid();
  }
  else if(r->m_nHeight + 1 == b.m_nHeight) {
    (*u)[MIDDLE].ub() = 1;
    (*u)[RIGHT].setInvalid();
  }
  else if(r->m_nHeight + nGap >= b.m_nHeight - 1) {
    (*u)[RIGHT].lb() = 1;
    (*u)[MIDDLE].setInvalid();
  }
  else {
    (*u)[RIGHT].lb() = b.m_nHeight - r->m_nHeight - nGap;
    (*u)[MIDDLE].lb() = 1;
    (*u)[MIDDLE].ub() = (*u)[RIGHT].lb() - 1;
  }

  /**
   * In this final case, both dominated positions are not
   * available. Therefore we'll simply define the middle to take
   * over the entire range (minus the two unavailable dominated
   * positions). The math naturally works out fine even if we have
   * no possible values (in which case we'll automatically backtrack
   * during the search anyway!
   */

  u = &((*t)[FF]);
  (*u)[LEFT].setInvalid();
  (*u)[RIGHT].setInvalid();
  if(b.m_nHeight - r->m_nHeight < 2) {
    (*u)[MIDDLE].setInvalid();
  }
  else {
    (*u)[MIDDLE].lb() = 1;
    (*u)[MIDDLE].ub() = b.m_nHeight - r->m_nHeight - 1;
  }
}

void CoordinateRanges::initialize(RectPtrArray& ra, BoxDimensions& b,
				  UInt nGap) {
  m_pArray = &ra;
 
  /**
   * Allocate data structures.
   */

  m_vTable.resize(ra.size());
  m_vRTable.resize(ra.size());
  for(RectPtrArray::iterator i = ra.begin(); i != ra.end(); ++i) {
    Rectangle* r = *i;
    m_vTable[r->m_nID].clear();
    m_vTable[r->m_nID].resize(4, std::vector<Range>(3));
    m_vRTable[r->m_nID].clear();
    m_vRTable[r->m_nID].resize(4, std::vector<Range>(3));

    /**
     * Fill in the entries for the given rectangle r and table t.
     */
    
    std::vector<std::vector<Range> > *t1, *t2;
    if(r->m_bRotated) {
      t1 = &m_vRTable[r->m_nID];
      t2 = &m_vTable[r->m_nID];
    }
    else {
      t1 = &m_vTable[r->m_nID];
      t2 = &m_vRTable[r->m_nID];
    }

    initialize(r, t1, b, nGap);
    r->rotate();
    initialize(r, t2, b, nGap);
    r->rotate();
  }
}

void CoordinateRanges::initialize(Rectangle* r,
				  std::vector<std::vector<Range> >* t,
				  BoxDimensions& b, const Domination* pDomination) {
  std::vector<Range>* u(NULL);

  /**
   * Like the previous function, we'll have gaps on the left and
   * right side. However, these gaps will be dynamically determined
   * by the domination table and differ depending on the rectangle
   * we're examining.
   */
    
  UInt nLeftGap(0), nRightGap(0);
  if(pDomination->entriesw1(r) > 0)
    nLeftGap = pDomination->entriesw1(r) - 1;
  if(pDomination->entriesw2(r) > 0)
    nRightGap = pDomination->entriesw2(r) - 1;

  /**
   * Process the first case where both dominated positions are
   * available. We assert the left and right ranges first, and if
   * they overlap we prefer exploration in the left range as much as
   * possible. However, if the left gap completely accounts for the
   * total range, we'll give the right side a single value (taking
   * it from the left). Finally we set the Interior Interval.
   */

  u = &((*t)[TT]);
  (*u)[LEFT].lb() = 0;
  (*u)[LEFT].ub() = nLeftGap;
  (*u)[RIGHT].ub() = b.m_nHeight - r->m_nHeight;
  (*u)[RIGHT].lb() = b.m_nHeight - r->m_nHeight - nRightGap;
  if(r->m_nHeight == b.m_nHeight) { // Height takes entire range.
    (*u)[LEFT].ub() = 0;
    (*u)[RIGHT].setInvalid();
    (*u)[MIDDLE].setInvalid();
  }
  else if(nLeftGap + r->m_nHeight >= b.m_nHeight) { // Left gap takes entire range.
    (*u)[LEFT].ub() = b.m_nHeight - r->m_nHeight;
    (*u)[RIGHT].setInvalid();
    (*u)[MIDDLE].setInvalid();
  }
  else if((nLeftGap + r->m_nHeight + nRightGap) >= b.m_nHeight) { // Both gaps overlap.
    (*u)[RIGHT].lb() = (*u)[LEFT].ub() + 1;
    (*u)[MIDDLE].setInvalid();
  }
  else { // General case where there's enough room for both gaps.
    (*u)[MIDDLE].lb() = (*u)[LEFT].ub() + 1;
    (*u)[MIDDLE].ub() = (*u)[RIGHT].lb() - 1;
  }

  /**
   * The second case is where the left gap is valid but the right
   * gap is not. This immediately means that we'll allow the
   * Interval to handle the right side then and make the right side
   * invalid. As an optimization, since we've already checked that
   * right dominated position, we won't explore a value that
   * includes that.
   */

  u = &((*t)[TF]);
  (*u)[LEFT].lb() = 0;
  (*u)[MIDDLE].ub() = b.m_nHeight - r->m_nHeight - 1;
  (*u)[RIGHT].setInvalid();
  if(r->m_nHeight == b.m_nHeight) {
    (*u)[LEFT].setInvalid();
    (*u)[MIDDLE].setInvalid();
  }
  else if(r->m_nHeight + 1 == b.m_nHeight) {
    (*u)[LEFT].setInvalid();
    (*u)[MIDDLE].lb() = 0;
  }
  else if((nLeftGap + r->m_nHeight) >= (b.m_nHeight - 1)) { // Limiting factor is box.
    (*u)[LEFT].ub() = b.m_nHeight - 1 - r->m_nHeight;
    (*u)[MIDDLE].setInvalid();
  }
  else {
    (*u)[LEFT].ub() = nLeftGap;
    (*u)[MIDDLE].lb() = nLeftGap + 1;
    (*u)[MIDDLE].ub() = b.m_nHeight - r->m_nHeight - 1;
  }

  /**
   * Consider the case where the left dominated position is
   * unavailable but the right dominated position is still
   * available. In that case the middle Interval section should
   * completely take over for the left side coordinates (excluding
   * y=0, of course).
   */

  u = &((*t)[FT]);
  (*u)[LEFT].setInvalid();
  (*u)[MIDDLE].lb() = 1;
  (*u)[RIGHT].ub() = b.m_nHeight - r->m_nHeight;
  if(r->m_nHeight == b.m_nHeight) {
    (*u)[MIDDLE].setInvalid();
    (*u)[RIGHT].setInvalid();
  }
  else if(r->m_nHeight + 1 == b.m_nHeight) {
    (*u)[MIDDLE].ub() = 1;
    (*u)[RIGHT].setInvalid();
  }
  else if(r->m_nHeight + nRightGap >= b.m_nHeight - 1) {
    (*u)[RIGHT].lb() = 1;
    (*u)[MIDDLE].setInvalid();
  }
  else {
    (*u)[RIGHT].lb() = b.m_nHeight - nRightGap - r->m_nHeight;
    (*u)[MIDDLE].lb() = 1;
    (*u)[MIDDLE].ub() = (*u)[RIGHT].lb() - 1;
  }

  /**
   * In this final case, both dominated positions are not
   * available. Therefore we'll simply define the middle to take
   * over the entire range (minus the two unavailable dominated
   * positions). The math naturally works out fine even if we have
   * no possible values (in which case we'll automatically backtrack
   * during the search anyway!
   */

  u = &((*t)[FF]);
  (*u)[LEFT].setInvalid();
  (*u)[RIGHT].setInvalid();
  if(b.m_nHeight - r->m_nHeight < 2) {
    (*u)[MIDDLE].setInvalid();
  }
  else {
    (*u)[MIDDLE].lb() = 1;
    (*u)[MIDDLE].ub() = b.m_nHeight - r->m_nHeight - 1;
  }

  /**
   * If any of the gaps were size 0, then we can include that side in
   * with the Interval, so long as the Interval is valid, of course.
   */

  if(nLeftGap == 0)
    for(UInt i = 0; i < 4; ++i) {
      if((*t)[i][LEFT].nonempty()) {
	if((*t)[i][MIDDLE].nonempty())
	  (*t)[i][MIDDLE].lb() = (*t)[i][LEFT].lb();
	else (*t)[i][MIDDLE] = (*t)[i][LEFT];
	(*t)[i][LEFT].setInvalid();
      }
    }
  if(nRightGap == 0)
    for(UInt i = 0; i < 4; ++i) {
      if((*t)[i][RIGHT].nonempty()) {
	if((*t)[i][MIDDLE].nonempty())
	  (*t)[i][MIDDLE].ub() = (*t)[i][RIGHT].ub();
	else (*t)[i][MIDDLE] = (*t)[i][RIGHT];
	(*t)[i][RIGHT].setInvalid();
      }
    }
}

void CoordinateRanges::initialize(RectPtrArray& ra, BoxDimensions& b,
				  const Domination* pDomination) {
  m_pArray = &ra;
  
  /**
   * Allocate data structures.
   */

  m_vTable.resize(ra.size());
  m_vRTable.resize(ra.size());
  for(RectPtrArray::iterator i = ra.begin(); i != ra.end(); ++i) {
    Rectangle* r = *i;
    m_vTable[r->m_nID].clear();
    m_vTable[r->m_nID].resize(4, std::vector<Range>(3));
    m_vRTable[r->m_nID].clear();
    m_vRTable[r->m_nID].resize(4, std::vector<Range>(3));

    /**
     * Fill in the entries for the given rectangle r and table t.
     */
    
    std::vector<std::vector<Range> > *t1, *t2;
    if(r->m_bRotated) {
      t1 = &m_vRTable[r->m_nID];
      t2 = &m_vTable[r->m_nID];
    }
    else {
      t1 = &m_vTable[r->m_nID];
      t2 = &m_vRTable[r->m_nID];
    }

    if(b.canFit(r)) initialize(r, t1, b, pDomination);
    else setInvalid(t1);

    if(r->rotatable()) {
      r->rotate();
      if(b.canFit(r)) initialize(r, t2, b, pDomination);
      else setInvalid(t2);
      r->rotate();
    }
    else setInvalid(t2);
  }
}

void CoordinateRanges::breakVerticalSymmetry(Rectangle* r,
					     const BoxDimensions& b,
					     const Domination* pDomination) {
  UInt nGap(0);
  if(pDomination && pDomination->entriesw1(r) > 0)
    nGap = pDomination->entriesw1(r) - 1;
  if(r->m_bRotated)
    breakVerticalSymmetry(r, &m_vRTable[r->m_nID], b, nGap);
  else
    breakVerticalSymmetry(r, &m_vTable[r->m_nID], b, nGap);
}

void CoordinateRanges::breakAllSymmetry(Rectangle* r,
					const BoxDimensions& b,
					const Domination* pDomination) {
  breakVerticalSymmetry(r, b, pDomination);
  if(r->rotatable()) {
    r->rotate();
    breakVerticalSymmetry(r, b, pDomination);
    r->rotate();
  }
}

void CoordinateRanges::initializeSimple(RectPtrArray& ra, BoxDimensions& b,
					bool bBreakSymmetry) {
  m_pArray = &ra;
  
  /**
   * Allocate data structures.
   */

  std::vector<std::vector<std::vector<Range> > > *t1, *t2;
  m_vTable.resize(ra.size());
  m_vRTable.resize(ra.size());
  for(RectPtrArray::iterator i = ra.begin(); i != ra.end(); ++i) {
    Rectangle* r = *i;
    m_vTable[r->m_nID].clear();
    m_vTable[r->m_nID].resize(4, std::vector<Range>(3));
    m_vRTable[r->m_nID].clear();
    m_vRTable[r->m_nID].resize(4, std::vector<Range>(3));
    if(r->m_bRotated) {
      t1 = &m_vRTable;
      t2 = &m_vTable;
    }
    else {
      t1 = &m_vTable;
      t2 = &m_vRTable;
    }

    for(UInt j = 0; j < 4; ++j) {
      (*t1)[r->m_nID][j][LEFT].setInvalid();
      if(r->m_nHeight <= b.m_nHeight) {
	(*t1)[r->m_nID][j][MIDDLE].lb() = 0;
	(*t1)[r->m_nID][j][MIDDLE].ub() = b.m_nHeight - r->m_nHeight;
      }
      else (*t1)[r->m_nID][j][MIDDLE].setInvalid();
      (*t1)[r->m_nID][j][RIGHT].setInvalid();

      (*t2)[r->m_nID][j][LEFT].setInvalid();
      if(r->m_nWidth <= b.m_nHeight) {
	(*t2)[r->m_nID][j][MIDDLE].lb() = 0;
	(*t2)[r->m_nID][j][MIDDLE].ub() = b.m_nHeight - r->m_nWidth;
      }
      else (*t2)[r->m_nID][j][MIDDLE].setInvalid();
      (*t2)[r->m_nID][j][RIGHT].setInvalid();
    }
  }

  if(bBreakSymmetry) {
    Rectangle* r = ra[0];
    if(r->m_bRotated) {
      t1 = &m_vRTable;
      t2 = &m_vTable;
    }
    else {
      t1 = &m_vTable;
      t2 = &m_vRTable;
    }
    for(UInt j = 0; j < 4; ++j) {
      if(r->m_nHeight <= b.m_nHeight)
	if((*t1)[r->m_nID][j][MIDDLE].nonempty())
	  (*t1)[r->m_nID][j][MIDDLE].ub() /= 2;
      if(ra[0]->m_nWidth <= b.m_nHeight)
	if((*t2)[r->m_nID][j][MIDDLE].nonempty())
	  (*t2)[r->m_nID][j][MIDDLE].ub() /= 2;
    }
  }
}

const Range& CoordinateRanges::left(const Rectangle* r) const {
  return(left(r, m_vDomLeft[r->m_nID], m_vDomRight[r->m_nID]));
}

const Range& CoordinateRanges::left(const Rectangle* r, bool bLeftDom,
				    bool bRightDom) const {
  const std::vector<std::vector<std::vector<Range> > >* t =
    r->m_bRotated ? &m_vRTable : &m_vTable;
  if(bLeftDom)
    return((*t)[r->m_nID][bRightDom ? TT : TF][LEFT]);
  else
    return((*t)[r->m_nID][bRightDom ? FT : FF][LEFT]);
}

const Range& CoordinateRanges::middle(const Rectangle* r, bool bLeftDom,
				      bool bRightDom) const {
  const std::vector<std::vector<std::vector<Range> > >* t =
    r->m_bRotated ? &m_vRTable : &m_vTable;
  if(bLeftDom)
    return((*t)[r->m_nID][bRightDom ? TT : TF][MIDDLE]);
  else
    return((*t)[r->m_nID][bRightDom ? FT : FF][MIDDLE]);
}

const Range& CoordinateRanges::middle(const Rectangle* r) const {
  return(middle(r, m_vDomLeft[r->m_nID], m_vDomRight[r->m_nID]));
}

const Range& CoordinateRanges::right(const Rectangle* r) const {
  return(right(r, m_vDomLeft[r->m_nID], m_vDomRight[r->m_nID]));
}

const Range& CoordinateRanges::right(const Rectangle* r, bool bLeftDom,
				     bool bRightDom) const {
  const std::vector<std::vector<std::vector<Range> > >* t =
    r->m_bRotated ? &m_vRTable : &m_vTable;
  if(bLeftDom)
    return((*t)[r->m_nID][bRightDom ? TT : TF][RIGHT]);
  else
    return((*t)[r->m_nID][bRightDom ? FT : FF][RIGHT]);
}

void CoordinateRanges::breakVerticalSymmetry(Rectangle* r,
					     const BoxDimensions& b,
					     UInt nGap) {
  std::vector<std::vector<Range> > *u1, *u2;
  if(r->m_bRotated) {
    u1 = &m_vRTable[r->m_nID];
    u2 = &m_vTable[r->m_nID];
  }
  else {
    u1 = &m_vTable[r->m_nID];
    u2 = &m_vRTable[r->m_nID];
  }
  breakVerticalSymmetry(r, u1, b, nGap);
  r->rotate();
  breakVerticalSymmetry(r, u2, b, nGap);
  r->rotate();
}

void CoordinateRanges::breakVerticalSymmetry(Rectangle* r,
					     std::vector<std::vector<Range> >* t,
					     const BoxDimensions& b, UInt nGap) {
  UInt nLastY = (b.m_nHeight - r->m_nHeight) / 2;
  for(UInt i = 0; i < 4; ++i)
    for(UInt j = 0; j < 3; ++j)
      if((*t)[i][j].nonempty() && (nLastY < (*t)[i][j].ub()))
	(*t)[i][j].ub() = nLastY;
  
#if 0
  /**
   * Note that for the first rectangle, the left as well as the right
   * side dominant positions will always be available because we're
   * starting off with an empty bounding box. However, due to symmetry
   * breaking, we disallow the right side.
   */

  UInt nLastY = (b.m_nHeight - r->m_nHeight) / 2;
  (*u)[TT][RIGHT].setInvalid();
  (*u)[TF][RIGHT].setInvalid();

  /**
   * These cases will never be true. The left dominant position will
   * always be available.
   */

  (*u)[FT][LEFT].setInvalid();
  (*u)[FT][MIDDLE].setInvalid();
  (*u)[FT][RIGHT].setInvalid();
  (*u)[FF][LEFT].setInvalid();
  (*u)[FF][MIDDLE].setInvalid();
  (*u)[FF][RIGHT].setInvalid();

  /**
   * Since the dominated left position will always be available, we
   * will always explore this subtree.
   */

  (*u)[TT][LEFT].lb() = 0;
  (*u)[TF][LEFT].lb() = 0;

  if(nGap >= nLastY) { // Last coordinate can be 0.
    (*u)[TT][LEFT].ub() = 0;
    (*u)[TF][LEFT].ub() = 0;
    (*u)[TT][MIDDLE].setInvalid();
    (*u)[TF][MIDDLE].setInvalid();
  }
  else {
    (*u)[TT][LEFT].ub() = nGap;
    (*u)[TF][LEFT].ub() = nGap;
    (*u)[TT][MIDDLE].lb() = nGap + 1;
    (*u)[TF][MIDDLE].lb() = nGap + 1;
    (*u)[TT][MIDDLE].ub() = nLastY;
    (*u)[TF][MIDDLE].ub() = nLastY;
  }

  /**
   * Finally, if the gap itself is 0 we simply include the left side
   * with the Interior.
   */

  if(nGap == 0 && (*u)[TT][LEFT].nonempty()) {
    if((*u)[TT][MIDDLE].nonempty()) {
      (*u)[TT][MIDDLE].lb() = (*u)[TT][LEFT].lb();
      (*u)[TF][MIDDLE].lb() = (*u)[TF][LEFT].lb();
    }
    else {
      (*u)[TT][MIDDLE] = (*u)[TT][LEFT];
      (*u)[TF][MIDDLE] = (*u)[TF][LEFT];
    }
    (*u)[TT][LEFT].setInvalid();
    (*u)[TF][LEFT].setInvalid();
  }

  /**
   * Another check we perform is to see if the left side is the only
   * available Interval. If so, move it to the middle so that we can
   * explore it in Intervals.
   */

  if((*u)[TT][LEFT].nonempty() && (*u)[TT][MIDDLE].empty())
    std::swap((*u)[TT][MIDDLE], (*u)[TT][LEFT]);
  if((*u)[TF][LEFT].nonempty() && (*u)[TF][MIDDLE].empty())
    std::swap((*u)[TF][MIDDLE], (*u)[TF][LEFT]);
#endif
}

void CoordinateRanges::print() const {
  std::cout << "Unrotated table:" << std::endl;
  for(UInt i = 0; i < m_vTable.size(); ++i) {
    if(m_pArray->operator[](i)->m_bRotated)
      m_pArray->operator[](i)->printHW();
    else
      m_pArray->operator[](i)->printWH();
    std::cout << "\tTT: ";
    m_vTable[i][TT][LEFT].printEmpty(); std::cout << ", ";
    m_vTable[i][TT][MIDDLE].printEmpty(); std::cout << ", ";
    m_vTable[i][TT][RIGHT].printEmpty(); std::cout << std::endl;
    std::cout << "\tTF: ";
    m_vTable[i][TF][LEFT].printEmpty(); std::cout << ", ";
    m_vTable[i][TF][MIDDLE].printEmpty(); std::cout << ", ";
    m_vTable[i][TF][RIGHT].printEmpty(); std::cout << std::endl;
    std::cout << "\tFT: ";
    m_vTable[i][FT][LEFT].printEmpty(); std::cout << ", ";
    m_vTable[i][FT][MIDDLE].printEmpty(); std::cout << ", ";
    m_vTable[i][FT][RIGHT].printEmpty(); std::cout << std::endl;
    std::cout << "\tFF: ";
    m_vTable[i][FF][LEFT].printEmpty(); std::cout << ", ";
    m_vTable[i][FF][MIDDLE].printEmpty(); std::cout << ", ";
    m_vTable[i][FF][RIGHT].printEmpty(); std::cout << std::endl;
  }

  std::cout << std::endl;
  std::cout << "Rotated table:" << std::endl;
  for(UInt i = 0; i < m_vRTable.size(); ++i) {
    if(m_pArray->operator[](i)->m_bRotated)
      m_pArray->operator[](i)->printWH();
    else
      m_pArray->operator[](i)->printHW();
    std::cout << "\tTT: ";
    m_vRTable[i][TT][LEFT].printEmpty(); std::cout << ", ";
    m_vRTable[i][TT][MIDDLE].printEmpty(); std::cout << ", ";
    m_vRTable[i][TT][RIGHT].printEmpty(); std::cout << std::endl;
    std::cout << "\tTF: ";
    m_vRTable[i][TF][LEFT].printEmpty(); std::cout << ", ";
    m_vRTable[i][TF][MIDDLE].printEmpty(); std::cout << ", ";
    m_vRTable[i][TF][RIGHT].printEmpty(); std::cout << std::endl;
    std::cout << "\tFT: ";
    m_vRTable[i][FT][LEFT].printEmpty(); std::cout << ", ";
    m_vRTable[i][FT][MIDDLE].printEmpty(); std::cout << ", ";
    m_vRTable[i][FT][RIGHT].printEmpty(); std::cout << std::endl;
    std::cout << "\tFF: ";
    m_vRTable[i][FF][LEFT].printEmpty(); std::cout << ", ";
    m_vRTable[i][FF][MIDDLE].printEmpty(); std::cout << ", ";
    m_vRTable[i][FF][RIGHT].printEmpty(); std::cout << std::endl;
  }
}

void CoordinateRanges::checkDomLeft(const Rectangle* r, const Bins& v,
				    const BoxDimensions& b) {
  m_vDomLeft[r->m_nID] = v.canFit(r->m_nWidth, 0, r->m_nHeight);
}

void CoordinateRanges::checkDom(const Rectangle* r, const Bins& v,
				const BoxDimensions& b) {
  m_vDomLeft[r->m_nID] = v.canFit(r->m_nWidth, 0, r->m_nHeight);
  m_vDomRight[r->m_nID] = v.canFit(r->m_nWidth, b.m_nHeight - r->m_nHeight, b.m_nHeight);
}

void CoordinateRanges::setInvalid(std::vector<std::vector<Range> >* t) {
  for(UInt i = 0; i < 4; ++i)
    for(UInt j = 0; j < 3; ++j)
      (*t)[i][j].setInvalid();
}

void CoordinateRanges::setInvalid(std::vector<Range>* t) {
  for(UInt i = 0; i < 3; ++i)
    (*t)[i].setInvalid();
}

const std::vector<Range>& CoordinateRanges::get(const Rectangle* r,
						UInt nConfig) const {
  return(r->m_bRotated ?
	 m_vRTable[r->m_nID][nConfig] :
	 m_vTable[r->m_nID][nConfig]);
}
