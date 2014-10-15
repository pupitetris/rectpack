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
#include "BinsBase.h"
#include "Interval.h"
#include "Parameters.h"
#include "RectPtrArray.h"
#include <algorithm>
#include <iomanip>

BinsBase::BinsBase() :
  m_bPropagation(true) {
}

BinsBase::~BinsBase() {
}

void BinsBase::initialize(const Parameters* pParams) {
  if(pParams->m_s1.find_first_of('w') < pParams->m_s1.size())
    m_vWaste.disable();
  else
    m_vWaste.enable();
  m_bPropagation = (pParams->m_s1.find_first_of('p') >= pParams->m_s1.size());
  m_vPlaced.clear();
  m_vPlaced.resize(pParams->m_vInstance.size(), false);
}

void BinsBase::pushy(const Rectangle* r) {
  push(r->y, r->y + r->m_nHeight, r->m_nWidth);
}

void BinsBase::popy(const Rectangle* r) {
  pop(r->y, r->y + r->m_nHeight, r->m_nWidth);
}

void BinsBase::pushx(const Rectangle* r) {
  push(r->x, r->x + r->m_nWidth, r->m_nHeight);
}

void BinsBase::popx(const Rectangle* r) {
  pop(r->x, r->x + r->m_nWidth, r->m_nHeight);
}

void BinsBase::pushSimpley(const Rectangle* r) {
  pushSimple(r->y, r->y + r->m_nHeight, r->m_nWidth);
}

void BinsBase::popSimpley(const Rectangle* r) {
  popSimple(r->y, r->y + r->m_nHeight, r->m_nWidth);
}

void BinsBase::push(UInt nSize, const Interval& i) {

  /**
   * Draw in the Interval.
   */

  for(Int j = i.m_nEnd; j < i.m_nEnd + i.m_nWidth; ++j)
    sub(j, nSize, m_vWaste);

  /**
   * Add back in the area of the rectangle Into the wasted space
   * histogram.
   */

  if(i.m_nWidth > 0)
    m_vWaste[nSize] += nSize * i.m_nWidth;

  /**
   * Remember our current stack size.
   */

  m_vStackSize.push_back(m_vStack.size());
  m_vRectangles.push();
}

void BinsBase::pop(UInt nSize, const Interval& i) {

  /**
   * Perform any decompression necessary (if any).
   */

  while(m_vStack.size() > m_vStackSize.back()) {
    decompress(m_vStack.back());
    m_vStack.pop_back();
  }
  m_vStackSize.pop_back();

  /**
   * Restore square removals.
   */

  m_vRectangles.pop();

  /**
   * Subtract the rectangle's area out of the wasted space histogram.
   */

  if(i.m_nWidth > 0)
    m_vWaste[nSize] -= nSize * i.m_nWidth;

  /**
   * Remove the Interval.
   */

  for(Int j = i.m_nEnd; j < i.m_nEnd + i.m_nWidth; ++j)
    add(j, nSize, m_vWaste);
}

void BinsBase::push(UInt nSize, UInt nMinDim, UInt nArea,
		    const Interval& i) {

  /**
   * Draw in the Interval.
   */

  for(Int j = i.m_nEnd; j < i.m_nEnd + i.m_nWidth; ++j)
    sub(j, nSize, m_vWaste);

  /**
   * Add back in the area of the rectangle Into the wasted space
   * histogram.
   */

  m_vWaste[nMinDim] += nArea;
  if(i.m_nWidth > 0)
    m_vWaste[nSize] -= nSize * (i.m_nEnd - i.m_nBegin);

  /**
   * Remember our current stack size.
   */

  m_vStackSize.push_back(m_vStack.size());
  m_vRectangles.push();
}

void BinsBase::pop(UInt nSize, UInt nMinDim, UInt nArea,
		   const Interval& i) {

  /**
   * Perform any decompression necessary (if any).
   */

  while(m_vStack.size() > m_vStackSize.back()) {
    decompress(m_vStack.back());
    m_vStack.pop_back();
  }
  m_vStackSize.pop_back();

  /**
   * Restore square removals.
   */

  m_vRectangles.pop();

  /**
   * Subtract the rectangle's area out of the wasted space histogram.
   */

  m_vWaste[nMinDim] -= nArea;
  if(i.m_nWidth > 0)
    m_vWaste[nSize] += nSize * (i.m_nEnd - i.m_nBegin);

  /**
   * Remove the Interval.
   */

  for(Int j = i.m_nEnd; j < i.m_nEnd + i.m_nWidth; ++j)
    add(j, nSize, m_vWaste);
}

void BinsBase::pushSimple(Int nSize, const Interval& i) {
  for(Int j = i.m_nEnd; j < i.m_nEnd + i.m_nWidth; ++j)
    sub(j, nSize, m_vWaste);
}

void BinsBase::popSimple(Int nSize, const Interval& i) {
  for(Int j = i.m_nEnd; j < i.m_nEnd + i.m_nWidth; ++j)
    add(j, nSize, m_vWaste);
}

void BinsBase::decompress(const std::pair<const Rectangle*, Interval>& p) {

  /**
   * Do uncompression. p.second is the old Interval, and
   * m_vDomains[p.first] is the current Interval. We need to remove
   * all strips that we've added. When we revert to the old Intervals,
   * note that the old Intervals are probably always going to be
   * bigger than our new Intervals. Then current Intervals always
   * represents additional constraInt propagation work, so we would
   * have eliminated certain placements, hence shrinking the
   * Interval. The old Intervals are larger because they represent
   * more value possibilities that haven't yet been removed by
   * constraInt propagation.
   */

  /**
   * Handle the strips near the end. Previously we drew in the actual
   * strips due to constraInt propagation, and when we did that, we
   * returned the empty space (consumed by the residue) to the
   * histogram. So now we have to undraw the strips and remove that
   * empty space (since we're reapplying the residue).
   */

  for(Int j = p.second.m_nBegin; j < m_vDomain[p.first->m_nID].m_nBegin; ++j) {
    add(j + p.first->m_nHeight, p.first->m_nWidth, m_vWaste);
    m_vWaste[p.first->m_nWidth] -= p.first->m_nWidth; // Residue now consumes empty space.
  }
  for(Int j = m_vDomain[p.first->m_nID].m_nEnd; j < p.second.m_nEnd; ++j) {
    add(j, p.first->m_nWidth, m_vWaste);
    m_vWaste[p.first->m_nWidth] -= p.first->m_nWidth; // Residue now consumes empty space.
  }

  /**
   * Reset the domain size.
   */

  m_vDomain[p.first->m_nID] = p.second;
}

bool BinsBase::compress(const Rectangle* pJustPlaced, VectorStack<UInt>& v) {
  m_vRectangles.push_back(pJustPlaced);
  bool bContinue = true;
  while(bContinue) {
    bContinue = false;
    UInt i(0);
    while(i < m_vRectangles.size()) {
      Int nResult(adjustDomain(m_vRectangles[i], m_vDomain[m_vRectangles[i]->m_nID]));
      if(nResult == 2) return(false);
      if(nResult == 1) {
	bContinue = true;
	if(v[m_vRectangles[i]->m_nID] < (UInt) pJustPlaced->m_nID)
	  v.update(m_vRectangles[i]->m_nID, (UInt) pJustPlaced->m_nID);
      }
      if(m_vDomain[m_vRectangles[i]->m_nID].m_nWidth ==
	 (Int) m_vRectangles[i]->m_nHeight)
	m_vRectangles.remove(i);
      else ++i;
    }
  }
  return(true);
}

bool BinsBase::compress(const Rectangle* pJustPlaced) {
  m_vRectangles.push_back(pJustPlaced);
  bool bContinue = true;
  while(bContinue) {
    bContinue = false;
    UInt i(0);
    while(i < m_vRectangles.size()) {
      Int nResult(adjustDomain(m_vRectangles[i], m_vDomain[m_vRectangles[i]->m_nID]));
      if(nResult == 2) return(false);
      if(nResult == 1) bContinue = true;
      if(m_vDomain[m_vRectangles[i]->m_nID].m_nWidth ==
	 (Int) m_vRectangles[i]->m_nHeight)
	m_vRectangles.remove(i);
      else ++i;
    }
  }
  return(true);
}

bool BinsBase::compress(const Rectangle* pJustPlaced,
			std::deque<const Rectangle*>& v) {
  v.clear();
  m_vRectangles.push_back(pJustPlaced);
  bool bContinue = true;
  while(bContinue) {
    bContinue = false;
    UInt i(0);
    while(i < m_vRectangles.size()) {
      Int nResult(adjustDomain(m_vRectangles[i], m_vDomain[m_vRectangles[i]->m_nID]));
      if(nResult == 2) return(false);
      if(nResult == 1) bContinue = true;
      if(m_vDomain[m_vRectangles[i]->m_nID].m_nWidth ==
	 (Int) m_vRectangles[i]->m_nHeight) {
	v.push_back(m_vRectangles[i]);
	m_vRectangles.remove(i);
      }
      else ++i;
    }
  }
  return(true);
}

bool BinsBase::compress(std::deque<const Rectangle*>& v) {
  bool bContinue = true;
  while(bContinue) {
    bContinue = false;
    UInt i(0);
    while(i < m_vRectangles.size()) {
      Int nResult(adjustDomain(m_vRectangles[i], m_vDomain[m_vRectangles[i]->m_nID]));
      if(nResult == 2) return(false);
      if(nResult == 1) bContinue = true;
      if(m_vDomain[m_vRectangles[i]->m_nID].m_nWidth ==
	 (Int) m_vRectangles[i]->m_nHeight) {
	v.push_back(m_vRectangles[i]);
	m_vRectangles.remove(i);
      }
      else ++i;
    }
  }
  return(true);
}

bool BinsBase::compress() {
  bool bContinue = true;
  while(bContinue) {
    bContinue = false;
    UInt i(0);
    while(i < m_vRectangles.size()) {
      Int nResult(adjustDomain(m_vRectangles[i], m_vDomain[m_vRectangles[i]->m_nID]));
      if(nResult == 2) return(false);
      if(nResult == 1) bContinue = true;
      if(m_vDomain[m_vRectangles[i]->m_nID].m_nWidth ==
	 (Int) m_vRectangles[i]->m_nHeight)
	m_vRectangles.remove(i);
      else ++i;
    }
  }
  return(true);
}

Int BinsBase::domainSize(const Interval& i, UInt d1,
			 UInt d2) const {
  Int n;
  return(domainSize(i, d1, d2, n));
}

Int BinsBase::domainSize(const Interval& i, UInt d1,
			 UInt d2, Int& n) const {
  Int n2;
  for(n = i.m_nEnd - 1; n >= i.m_nBegin; --n)
    if(operator[](n) < (Int) d2)
      break;
  ++n;
  for(n2 = i.m_nBegin; n2 < i.m_nEnd; ++n2)
    if(operator[](n2 + d1) < (Int) d2)
      break;
  return(n2 - n + 1);
}

Int BinsBase::adjustDomain(const Rectangle* r, Interval& iOld) {
  if(!m_bPropagation) return(0);
  if(iOld.m_nEnd == iOld.m_nBegin) return(0);

  Interval iNew(iOld.m_nEnd - 1, iOld.m_nBegin);
  while(iNew.m_nBegin >= iOld.m_nBegin)
    if(operator[](iNew.m_nBegin) < (Int) r->m_nWidth) break;
    else --iNew.m_nBegin;
  ++iNew.m_nBegin;
  while(iNew.m_nEnd < iOld.m_nEnd)
    if(operator[](iNew.m_nEnd + (Int) r->m_nHeight) < (Int) r->m_nWidth) break;
    else ++iNew.m_nEnd;
  if(iNew.m_nBegin > iNew.m_nEnd) return(2);
  iNew.m_nWidth = r->m_nHeight + iNew.m_nBegin - iNew.m_nEnd;
  if(iNew.m_nWidth == iOld.m_nWidth) return(0);

  /**
   * Add the new strips and push onto our stack the previous values.
   */

  addStripsy(r, iOld, iNew);
  m_vStack.push_back(std::pair<const Rectangle*, Interval>(r, iOld));
  iOld = iNew;
  return(1);
}

bool BinsBase::yiValid(RectPtrArray::iterator iBegin,
		       const RectPtrArray::const_iterator& iEnd) const {
  Int n;
  for(; iBegin != iEnd; ++iBegin)
    if(domainSize((*iBegin)->yi, (*iBegin)->m_nHeight,
		  (*iBegin)->m_nWidth, n) < 1)
      return(false);
  return(true);
}

bool BinsBase::xiValid(RectPtrArray::iterator iBegin,
		       const RectPtrArray::const_iterator& iEnd) const {
  Int n;
  for(; iBegin != iEnd; ++iBegin)
    if(domainSize((*iBegin)->xi, (*iBegin)->m_nWidth,
		  (*iBegin)->m_nHeight, n) < 1)
      return(false);
  return(true);
}

void BinsBase::clear() {
  std::vector<Int>::clear();
  m_vWaste.clear();
  m_vDomain.clear();
  m_vRectangles.clear();
  m_vPlaced.reset();
#ifdef DEBUG
  m_vDebugStack.clear();
#endif // DEBUG
}

void BinsBase::print() const {
  std::cout << "  n  bin waste domain" << std::endl;
  UInt nMax = std::max(size(), m_vWaste.size());
  nMax = std::max(nMax, (UInt) m_vDomain.size());
  for(UInt i = 0; i < nMax; ++i) {
    std::cout << std::setw(3) << i;
    if(i < size()) std::cout << std::setw(5) << operator[](i);
    else std::cout << "     ";
    if(i < m_vWaste.size()) std::cout << std::setw(6) << m_vWaste[i];
    else std::cout << "      ";
    if(i < m_vDomain.size()) std::cout << " " << m_vDomain[i];
    std::cout << std::endl;
  }
}

void BinsBase::resizey(const BoxDimensions& b, const RectPtrArray& v) {
  std::vector<Int>::resize(b.m_nHeight, b.m_nWidth);
  m_vWaste.resize(b.m_nWidth + 1, 0);
  m_vWaste.back() = b.m_nArea;
  for(RectPtrArray::const_iterator i = v.begin(); i != v.end(); ++i) {
    if((*i)->m_bRotatable)
      m_vWaste[(*i)->m_nMinDim] -= (*i)->m_nArea;
    else
      m_vWaste[(*i)->m_nWidth] -= (*i)->m_nArea;
  }
  m_vDomain.resize(v.size());
  m_vRectangles.reserve(v.size());
  m_vStack.reserve(v.size() * v.size());
  m_vStackSize.reserve(v.size() * v.size());
}

bool BinsBase::operator==(const BinsBase& rhs) const {
  return((std::vector<Int>) (*this) == (std::vector<Int>) rhs &&
	 m_vWaste == rhs.m_vWaste &&
	 m_vRectangles == rhs.m_vRectangles &&
	 m_vStack == rhs.m_vStack &&
	 m_vStackSize == rhs.m_vStackSize);
}

bool BinsBase::canFit(UInt nSize, const Interval& i) const {
  for(Int j = i.m_nEnd; j < i.m_nEnd + i.m_nWidth; ++j)
    if(operator[](j) < (Int) nSize) return(false);
  return(true);
}

bool BinsBase::canFit(UInt nSize, UInt i, UInt j) const {
  for(; i < j; ++i)
    if(operator[](i) < (Int) nSize) return(false);
  return(true);
}

bool BinsBase::canFitx(const Rectangle* r) const {
  return(canFit(r->m_nHeight, r->x, r->x + r->m_nWidth));
}

bool BinsBase::canFity(const Rectangle* r) const {
  return(canFit(r->m_nWidth, r->y, r->y + r->m_nHeight));
}

bool BinsBase::canFitStrips(const Rectangle* r) const {
  if(m_bPropagation) return(true);

  for(Int j = r->y; j < r->yi.m_nEnd; ++j)
    if(operator[](j) < (Int) r->m_nWidth) return(false);
  for(Int j = r->yi.m_nEnd + r->yi.m_nWidth;
      j < (Int) (r->yi.m_nEnd + r->m_nWidth); ++j)
    if(operator[](j) < (Int) r->m_nWidth) return(false);
  return(true);
}

bool BinsBase::verifyBins(const BoxDimensions& b) const {
  std::vector<Int> v(b.m_nHeight, b.m_nWidth);
#ifdef DEBUG
  for(UInt i = 0; i < m_vDebugStack.size(); ++i) {
    const Interval* pi = &m_vDomain[m_vDebugStack[i]->m_nID];
    for(UInt j = pi->m_nEnd; j < (UInt) (pi->m_nEnd + pi->m_nWidth); ++j)
      v[j] -= m_vDebugStack[i]->m_nWidth;
  }
  for(UInt i = 0; i < v.size(); ++i)
    if(v[i] != operator[](i)) return(false);
#endif // DEBUG
  return(true);
}

bool BinsBase::verifyWaste() const {
  WastedBins b(m_vWaste.size(), 0);
  for(std::vector<Int>::const_iterator i = begin();
      i != end(); ++i)
    b[*i] += *i;
  return(b == m_vWaste);
}

void BinsBase::pushStripsy(const Rectangle* r) {

  /**
   * Save the old squares set before removing our current square. Then
   * save our stack.
   */

  m_vRectangles.push();
  UInt t = m_vRectangles.find(r);
  if(t < m_vRectangles.size())
    m_vRectangles.remove(t);
  m_vStackSize.push_back(m_vStack.size());

  /**
   * Add in new strips based on square location. Note that this is
   * treated as if it were an act of compression. So we'll let our
   * popping handle the undo for this.
   */

  Interval i(r->y, r->y, r->m_nHeight);
  addStripsy(r, m_vDomain[r->m_nID], i);

  /**
   * Save the old domain value, and assert the new one (single value).
   */

  m_vStack.push_back(std::pair<const Rectangle*, Interval>(r, m_vDomain[r->m_nID]));
  m_vDomain[r->m_nID] = i;
}

void BinsBase::popStripsy(const Rectangle* r) {

  /**
   * Decompress whatever we need to decompress. Note that when we
   * pushed the strips on, we pretended that it was a single iteration
   * of compression so therefore, decompressing also ends up removing
   * the strips assignment.
   */

  while(m_vStack.size() > m_vStackSize.back()) {
    decompress(m_vStack.back());
    m_vStack.pop_back();
  }
  m_vStackSize.pop_back();

  /**
   * Restore square removals.
   */

  m_vRectangles.pop();
}
