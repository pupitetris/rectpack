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

#ifndef INTERVAL_H
#define INTERVAL_H

#include "Integer.h"
#include "Rational.h"
#include <iostream>
#include <algorithm>

class Interval;
class Range;
class Rectangle;

class Interval {
  friend std::ostream& operator<<(std::ostream& os, const Interval& i);

 public:
  Interval() :
    m_nBegin(0),
    m_nEnd(0),
    m_nWidth(0) {
  }
  
  Interval(Int nBegin, Int nEnd) :
    m_nBegin(nBegin),
    m_nEnd(nEnd),
    m_nWidth(0) {
  }
    
  Interval(Int nBegin, Int nEnd, Int nWidth) :
    m_nBegin(nBegin),
    m_nEnd(nEnd),
    m_nWidth(nWidth) {
  }
      
  Interval(const Interval& src) :
    m_nBegin(src.m_nBegin),
    m_nEnd(src.m_nEnd),
    m_nWidth(src.m_nWidth) {
  }

  /**
   * Assigns the interval to whatever the currently set y coordinate
   * of the square is.
   */

  const Interval& operator=(const Rectangle* r);

  /**
   * Checks to see if a point is contained in this interval.
   */

  bool contains(Int x) {
    return(x >= m_nBegin && x <= m_nEnd);
  }
    
  /**
   * Checks if the intersection of two intervals is greater than 0.
   */

  bool overlaps(const Interval& i) const {
    return(overlap(i) > 0);
  }


  Int overlap(const Interval& i) const {
    return(std::min(m_nEnd, i.m_nEnd) -
	   std::max(m_nBegin, i.m_nBegin));
  }

  const Interval& operator=(const Interval& rhs) {
    m_nBegin = rhs.m_nBegin;
    m_nEnd = rhs.m_nEnd;
    m_nWidth = rhs.m_nWidth;
    return(*this);
  }

  const Interval& operator&=(const Interval& rhs) {
    m_nEnd = std::min(m_nEnd, rhs.m_nEnd);
    m_nBegin = std::max(m_nBegin, rhs.m_nBegin);
    return(*this);
  }

  Interval operator&(const Interval& rhs) const {
    Interval i(*this);
    i &= rhs;
    return(i);
  }

  const Interval& operator+=(Int nOffset) {
    m_nBegin += nOffset;
    m_nEnd += nOffset;
    return(*this);
  }

  const Interval& operator++() {
    ++m_nBegin;
    ++m_nEnd;
    return(*this);
  }

  bool operator<=(Int n) const {
    return(m_nEnd <= n);
  }

  bool operator<(Int n) const {
    return(m_nEnd < n);
  }

  bool empty() const {
    return(m_nEnd < m_nBegin);
  }

  Int size() const {
    return(m_nEnd - m_nBegin);
  }

  bool valid(Int nLastBin) const {
    return(m_nEnd <= nLastBin && m_nBegin <= m_nEnd);
  }

  ~Interval() {
  }

  /**
   * Given the size of the square and the last allowable coordinate
   * assignment, this clamps the end interval variable to this last
   * bin and recomputes the width of the compulsory part based on this
   * knowledge.
   *
   * @param nHeight the width of the rectangle, to be used to compute
   * the new width of the compulsory part.
   *
   * @param nLastBin the last allowable coordinate that will be
   * represented by the interval values.
   */

  void clamp(Int nHeight, Int nLastBin) {
    if(m_nEnd > nLastBin)
      m_nEnd = nLastBin;
    m_nWidth = nHeight - (m_nEnd - m_nBegin);
  };

  /**
   * Initializes the interval to represent the compulsory part when
   * given the starting interval coordinate x, the size of the square,
   * the desired scale, and the last possible coordinate assignment
   * (which we will clamp to).
   */

  void init(Int x, Int nHeight, float nScale, Int nLastBin) {
    m_nBegin = x;
    Int nInterval = (Int) (nHeight * nScale);
    m_nEnd = m_nBegin + (nInterval < 1 ? 0 : (nInterval - 1));
    clamp(nHeight, nLastBin);
  }

  void init(Int x, Int nHeight, const URational& nScale, Int nLastBin) {
    m_nBegin = x;
    Int nInterval = (Int) (nHeight * nScale.get_num() / nScale.get_den());
    m_nEnd = m_nBegin + (nInterval < 1 ? 0 : (nInterval - 1));
    clamp(nHeight, nLastBin);
  }

  void set(const Rectangle* r, const Range* pRange,
	   UInt nCurrent, UInt nBranches);
  void set(const Rectangle* r, UInt lb, UInt ub,
	   UInt nCurrent, UInt nBranches);

  void next(Int nHeight, Int nLastBin) {
    operator+=(m_nEnd - m_nBegin + 1);
    clamp(nHeight, nLastBin);
  }

  bool operator==(const Interval& rhs) const {
    return(m_nBegin == rhs.m_nBegin &&
	   m_nEnd == rhs.m_nEnd &&
	   m_nWidth == rhs.m_nWidth);
  }

  /**
   * The first coordinate included.
   */

  Int m_nBegin;

  /**
   * The last coordinate included.
   */

  Int m_nEnd;

  /**
   * The width of the compulsory part.
   */

  Int m_nWidth;
};

std::ostream& operator<<(std::ostream& os, const Interval& i);

#endif // INTERVAL_H
