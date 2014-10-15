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

#ifndef COORDINATERANGES_H
#define COORDINATERANGES_H

#include "Bins.h"
#include "Integer.h"
#include "Range.h"
#include <deque>
#include <vector>

class BoxDimensions;
class Domination;
class Parameters;
class Rectangle;
class RectPtrArray;

/**
 * Depending on whether or not the actual dominated positions were
 * available, this class precomputes the left, middle, and right side
 * ranges in a way that attempts to capture the values that must be
 * tested.
 */

class CoordinateRanges {
  
  enum Side {
    LEFT,
    MIDDLE,
    RIGHT
  };

  enum Config {
    TT,
    TF,
    FT,
    FF
  };

 public:
  CoordinateRanges();
  ~CoordinateRanges();

  void initialize(const Parameters* pParams);

  /**
   * Assumes that the range should be divided Into a left, middle, and
   * right region. The parameter nGap will specify the desired size of
   * the left and right regions, with the middle region to capture all
   * the leftover values.
   */

  void initialize(RectPtrArray& ra, BoxDimensions& b, UInt nGap);

  /**
   * Builds the given table entry for the given rectangle.
   */

  void initialize(Rectangle* r,
		  std::vector<std::vector<Range> >* t,
		  BoxDimensions& b, UInt nGap);

  /**
   * Does the same thing as the previous function except that we infer
   * the left and right regions from the domination
   * table. Essentially, we pick sizes for the left and right regions
   * such that the set left, middle, and right ranges excludes the
   * dominated positions as much as possible.
   */

  void initialize(RectPtrArray& ra, BoxDimensions& b,
		  const Domination* pDomination);
  void initializeSimple(RectPtrArray& ra, BoxDimensions& b, bool bBreakSymmetry);
  void setInvalid(std::vector<std::vector<Range> >* t);
  void setInvalid(std::vector<Range>* t);
  const Range& left(const Rectangle* r) const;
  const Range& left(const Rectangle* r, bool bLeftDom, bool bRightDom) const;
  const Range& middle(const Rectangle* r) const;
  const Range& middle(const Rectangle* r, bool bLeftDom, bool bRightDom) const;
  const Range& right(const Rectangle* r) const;
  const Range& right(const Rectangle* r, bool bLeftDom, bool bRightDom) const;
  const std::vector<Range>& get(const Rectangle* r,
				UInt nConfig) const;
  void breakVerticalSymmetry(Rectangle* r, const BoxDimensions& b,
			     const Domination* pDomination);
  void breakAllSymmetry(Rectangle* r, const BoxDimensions& b,
			const Domination* pDomination);
  void checkDomLeft(const Rectangle* r, const Bins& v,
		    const BoxDimensions& b);
  void checkDom(const Rectangle* r, const Bins& v,
		const BoxDimensions& b);

 private:
  void initialize(Rectangle* r,
		  std::vector<std::vector<Range> >* t,
		  BoxDimensions& b, const Domination* pDomination);
  void breakVerticalSymmetry(Rectangle* r, const BoxDimensions& b,
			     UInt nGap);
  void breakVerticalSymmetry(Rectangle* r, std::vector<std::vector<Range> >* t,
			     const BoxDimensions& b, UInt nGap);

  /**
   * Indexing will be as follows. First we index on the rectangle ID,
   * then we index on the configuration, and then we index on the
   * side.
   */

  std::vector<std::vector<std::vector<Range> > > m_vTable;
  std::vector<std::vector<std::vector<Range> > > m_vRTable;

 public:
  std::vector<bool> m_vDomLeft;
  std::vector<bool> m_vDomRight;

 private:
  void print() const;
  RectPtrArray* m_pArray;
  const Parameters* m_pParams;
};

#endif // COORDINATERANGES_H
