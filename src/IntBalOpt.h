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

#ifndef INTBALOPT_H
#define INTBALOPT_H

#include "BranchingFactor.h"
#include "CoordinateRanges.h"
#include "DuplicateIndex.h"
#include "IntPack.h"
#include "RectDomains.h"
#include "RectPtrArray.h"
#include <set>
#include <vector>

/**
 * This packer is supposed to use intervals to represent values in
 * both dimensions. SDW asserts the rectangles are sorted by
 * decreasing width.
 */

class IntBalOpt : public IntPack {

 public:
  IntBalOpt();
  virtual ~IntBalOpt();
  virtual void initialize(const Parameters* pParams);
  virtual void initialize(const HeapBox* pBox);

 protected:

  /**
   * General packing function starts by calling packY to pack the
   * first dimension.
   */

  virtual bool pack();
  virtual bool packcra();

  /**
   * Finds, over the given sequence of squares, the smallest domain
   * with respect to adding in strips in order to "fix" the
   * solution. This is not as trivial as just taking the largest
   * square, because constraint propagation may cause strips to be
   * drawn in.
   */

  size_t bestStripArea(Rectangle*& s);

  /**
   * Top-level packing function for the first dimension. This ends up
   * calling the waste function to check for pruning events against
   * wasted space.
   */

  bool packY(size_t i);

  /**
   * Checks for pruning against wasted space and then calls
   * searchSpace.
   */

  bool waste(size_t i);

  /**
   * Evaluates the domain sizes and calls the appropriate packing
   * function, essentially picking the best search space to work
   * in. After picking, it either calls packX, strips, or location
   * (the latter two being different search spaces).
   */

  bool searchSpace(size_t i);
  
  /**
   * Strips search space. After placement it calls waste().
   */

  bool strips(Rectangle* s, size_t i);

  /**
   * Location-based search space. It divides the values into left,
   * middle, and right, and calls the respective location placement
   * functions. Each of these, in turn, call waste().
   */

  bool location(size_t i);

  /**
   * Interval data structures.
   */

  float m_nYScale;

  /**
   * Records, for a particular square ID, the very last Y-coordinate
   * value that should be assigned for that square.
   */

  std::vector<size_t> m_vLastBinYI;

  /**
   * The last Y-coordinate for any interval that we allow prior to
   * having the square enter into the forbidden region on the right
   * side.
   */

  std::vector<size_t> m_vForbiddenStartYI;

 private:
  bool m_bVarOrder;

  /**
   * Determines whether or not we will consider the domination tables
   * when determining ranges.
   */

  bool m_bDomBasedRanges;
  CoordinateRanges m_Ranges;
  BranchingFactor m_Branches;
  RectDomains m_vDomains;
};

#endif // INTBALOPT_H
