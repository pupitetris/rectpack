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

#ifndef WASTEDBINS_H
#define WASTEDBINS_H

#include "Integer.h"
#include "RectPtrArray.h"
#include <set>
#include <vector>

class BoxDimensions;
class ComponentPtrs;
class Parameters;
class Rectangle;
class RectSetMinDim;
class WastedBins;

class WastedBins : public std::vector<Int> {
 public:
  WastedBins();
  WastedBins(UInt i, Int j);
  WastedBins(const BoxDimensions& b);
  void initialize(const Parameters* pParams);
  ~WastedBins();
  void initialize(const BoxDimensions& b);
  void moveArea(UInt nOld, UInt nNew, UInt nAmount);
  const WastedBins& operator=(const WastedBins& src);

  /**
   * Runs the empty space constraInt test on the given sequence. We
   * assume that the sequence is a set of rectangles sorted by
   * decreasing width.
   */

  bool canFitw(RectPtrArray::const_iterator iStart,
	       const RectPtrArray::const_iterator& iEnd) const;

  /**
   * This version returns through the last parameter the rectangle ID
   * that caused the pruning (if applicable). This function also
   * assumes that the originally ordered (non-reverse) sequence goes
   * from smallest width rectangles to largest -- which is why we must
   * reverse it.
   */

  bool canFitw(RectPtrArray::const_reverse_iterator iStart,
	       const RectPtrArray::const_reverse_iterator& iEnd) const;
  bool canFitw(RectPtrArray::const_reverse_iterator iStart,
	       const RectPtrArray::const_reverse_iterator& iEnd,
	       UInt& r) const;

  /**
   * This version returns through the last parameter the rectangle ID
   * that caused the pruning (if applicable). It also returns the next
   * rectangle that could fit. This information may be useful to the
   * space partitioner's backjumping algorithm.
   */

  bool canFitw(RectPtrArray::const_iterator iStart,
	       const RectPtrArray::const_iterator& iEnd,
	       UInt& r, UInt& s) const;

  /**
   * Runs the empty space constraInt test on the given sequence. We
   * assume that the sequence is a set of rectangles sorted by
   * decreasing height.
   */

  bool canFith(RectPtrArray::const_iterator iStart,
	       const RectPtrArray::const_iterator& iEnd) const;
  bool canFitm(RectPtrArray::const_iterator iStart,
	       const RectPtrArray::const_iterator& iEnd) const;
  bool canFit(const RectSetMinDim& s) const;
  bool canFit(const ComponentPtrs& vComponents,
	      const std::vector<bool>& vPlaced) const;
  bool canFitKorf(RectPtrArray::const_iterator iStart,
		  const RectPtrArray::const_iterator& iEnd,
		  Int nMaxWaste) const;
  bool canFitKorf(Int n, Int nMaxWaste) const;

  /**
   * This function assumes that we have subtracted the area of the
   * rectangles out of the empty space histogram, and we simply
   * iterate over it to test if the area is less than zero at any
   * poInt.
   *
   * @return true if the wasted space heuristic admits the partial
   * solution.
   *
   * @return false if the subtree is infeasible.
   */

  bool canFit() const;
  void print() const;

  /**
   * This flag can be made false to disable the entire class.
   */

  bool m_bEnabled;
  void enable() { m_bEnabled = true; };
  void disable() { m_bEnabled = false; };
};

#endif // WASTEDBINS_H
