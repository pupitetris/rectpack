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

#ifndef BINSBASE_H
#define BINSBASE_H

#include "FastRemove.h"
#include "Integer.h"
#include "Rectangle.h"
#include "VectorStack.h"
#include "WastedBins.h"
#include <assert.h>
#include <boost/dynamic_bitset.hpp>
#include <deque>
#include <vector>

/**
 * This class represents the amount of space that is currently
 * available in a particular bin. This is what the Interface is coded
 * to do.
 */

class BinsBase;
class BoxDimensions;
class Interval;
class Parameters;
class RectPtrArray;

class BinsBase : public std::vector<Int> {
 public:
  BinsBase();
  ~BinsBase();
  void initialize(const Parameters* pParams);
  void clear();
  void resizey(const BoxDimensions& b, const RectPtrArray& v);

  bool canFit(UInt nSize, const Interval& i) const;
  bool canFit(UInt nSize, UInt i, UInt j) const;
  bool canFitx(const Rectangle* r) const;
  bool canFity(const Rectangle* r) const;
  bool canFitxi(const Rectangle* r) const {
    return(canFit(r->m_nHeight, r->xi));
  }
  bool canFityi(const Rectangle* r) const {
    return(canFit(r->m_nWidth, r->yi));
  }
  bool canFitStrips(const Rectangle* r) const;

  void pushy(const Rectangle* r);
  void popy(const Rectangle* r);
  void pushx(const Rectangle* r);
  void popx(const Rectangle* r);
  void push(UInt nSize, const Interval& i);
  void pop(UInt nSize, const Interval& i);
  void push(UInt nSize, UInt nMinDim,
	    UInt nArea, const Interval& i);
  void pop(UInt nSize, UInt nMinDim,
	   UInt nArea, const Interval& i);

  void pushyi(const Rectangle* r) {
    if(r->rotatable())
      push(r->m_nWidth, r->m_nMinDim, r->m_nArea, r->yi);
    else push(r->m_nWidth, r->yi);
#ifdef DEBUG
    m_vDebugStack.push_back(r);
#endif // DEBUG
    m_vPlaced.set(r->m_nID);
  }

  void pushyi(const Rectangle* r, UInt nOldSize) {
    if(r->m_bRotatable)
      push(r->m_nWidth, nOldSize, r->m_nArea, r->yi);
    else push(r->m_nWidth, r->yi);
#ifdef DEBUG
    m_vDebugStack.push_back(r);
#endif // DEBUG
    m_vPlaced.set(r->m_nID);
  }

  void popyi(const Rectangle* r, UInt nOldSize) {
    if(r->m_bRotatable)
      pop(r->m_nWidth, nOldSize, r->m_nArea, r->yi);
    else pop(r->m_nWidth, r->yi);
#ifdef DEBUG
    m_vDebugStack.pop_back();
#endif // DEBUG
    m_vPlaced.reset(r->m_nID);
  }

  void popyi(const Rectangle* r) {
    if(r->m_bRotatable)
      pop(r->m_nWidth, r->m_nMinDim, r->m_nArea, r->yi);
    else pop(r->m_nWidth, r->yi);
#ifdef DEBUG
    m_vDebugStack.pop_back();
#endif // DEBUG
    m_vPlaced.reset(r->m_nID);
  }

  void pushSimpley(const Rectangle* r);
  void popSimpley(const Rectangle* r);
  void pushSimple(Int nSize, const Interval& i);
  void popSimple(Int nSize, const Interval& i);

  /**
   * Do not call this function if the domain is already a single
   * value. Also, absolutely do not call this function unless you've
   * already added the obligatory part of square s Into this bin.
   */

  void pushStripsy(const Rectangle* r);
  void popStripsy(const Rectangle* r);

  /**
   * Goes over each square in the sequence, and loops adjusting
   * domains until no more adjustments are possible. If at any time it
   * detects an empty domain, we return false.
   *
   * @return true if all domains are still non-empty, or false if the
   * domain is empty.
   */

  bool compress();
  bool compress(std::deque<const Rectangle*>& v);
  bool compress(const Rectangle* pJustPlaced);
  bool compress(const Rectangle* pJustPlaced, VectorStack<UInt>& v);
  bool compress(const Rectangle* pJustPlaced, std::deque<const Rectangle*>& v);
  void decompress(const std::pair<const Rectangle*, Interval>& p);
  bool yiValid(const RectPtrArray::iterator iBegin,
	       const RectPtrArray::const_iterator& iEnd) const;
  bool xiValid(const RectPtrArray::iterator iBegin,
	       const RectPtrArray::const_iterator& iEnd) const;

  bool yiCompress(const RectPtrArray::iterator iBegin,
		  const RectPtrArray::const_iterator& iEnd);
  bool xiCompress(const RectPtrArray::iterator iBegin,
		  const RectPtrArray::const_iterator& iEnd);

  /**
   * Determines the domain size, and sets n to be one of those valid
   * values. The Interval is the one for whose domain values we want
   * to analyze. d1 refers to the same dimension of the Interval but
   * should specify the size of the rectangle in the same
   * dimension. d2 is the other dimension orthogonal to the Interval's
   * dimension.
   */
    
  Int domainSize(const Interval& i, UInt d1, UInt d2) const;
  Int domainSize(const Interval& i, UInt d1, UInt d2, Int& n) const;

  /**
   * Given a square and its respective Interval, we attempt to shrink
   * the domain based on constraInts in our bins.
   *
   * @param s the square whose size we will use a a reference.
   *
   * @param i the Interval that we should try shrinking based on what
   * we've got in our set of bins.
   *
   * @return 0 if no work was done, 1 if work was done, and 2 if you
   * should backtrack due to a conflict in the constraInt propagation.
   */

  Int adjustDomain(const Rectangle* r, Interval& iOld);
  bool operator==(const BinsBase& rhs) const;
  void print() const;

  /**
   * Verifies the wasted space heuristic.
   */

  bool verifyWaste() const;
  bool verifyBins(const BoxDimensions& b) const;

  WastedBins m_vWaste;
  std::vector<Interval> m_vDomain;
  FastRemove<const Rectangle*> m_vRectangles;
  std::vector<std::pair<const Rectangle*, Interval> > m_vStack;
  std::vector<UInt> m_vStackSize;

  /**
   * Records whether or not a particular rectangle has already been
   * represented (in either Intervals or exact location) in the bins
   * data structure.
   */

  bool placed(const Rectangle* r) const {
    return(placed(r->m_nID));
  }

  bool placed(UInt nID) const {
    return(m_vPlaced.test(nID));
  }

  boost::dynamic_bitset<> m_vPlaced;

 public:

  void add(Int j, UInt nSize, std::vector<Int>& v) {
    v[operator[](j)] -= operator[](j);
    operator[](j) += nSize;
    v[operator[](j)] += operator[](j);
  }

  void sub(Int j, UInt nSize, std::vector<Int>& v) {
    v[operator[](j)] -= operator[](j);
    operator[](j) -= nSize;
    v[operator[](j)] += operator[](j);
  }

 private:
  void addStripsy(const Rectangle* r, const Interval& iOld,
		  const Interval& iNew) {
    for(Int j = iNew.m_nEnd; j < iOld.m_nEnd; ++j) {
      m_vWaste[r->m_nWidth] += r->m_nWidth; // Add back in the residue.
      sub(j, r->m_nWidth, m_vWaste);
    }
    for(Int j = iOld.m_nBegin; j < iNew.m_nBegin; ++j) {
      m_vWaste[r->m_nWidth] += r->m_nWidth; // Add back in the residue.
      sub(j + r->m_nHeight, r->m_nWidth, m_vWaste);
    }
  }

  void push(UInt i, UInt j, UInt k) {
    for(UInt l = i; l < j; ++l)
      sub(l, k, m_vWaste);
  };

  void pop(UInt i, UInt j, UInt k) {
    for(UInt l = i; l < j; ++l)
      add(l, k, m_vWaste);
  };

  void pushSimple(UInt i, UInt j, UInt k) {
    for(UInt l = i; l < j; ++l)
      operator[](l) += k;
  };

  void popSimple(UInt i, UInt j, UInt k) {
    for(UInt l = i; l < j; ++l)
      operator[](l) += k;
  };

  bool m_bPropagation;
#ifdef DEBUG
  std::deque<const Rectangle*> m_vDebugStack;
#endif // DEBUG
};

#endif // BINSBASE_H
