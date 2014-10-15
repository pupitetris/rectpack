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

#ifndef BRANCHINGFACTOR_H
#define BRANCHINGFACTOR_H

#include "BranchDesc.h"
#include "Integer.h"
#include "RectPtrArray.h"
#include <vector>

class BoxDimensions;
class CoordinateRanges;
class Domination;
class Rectangle;

class BranchingFactor : public std::vector<BranchDesc> {
 public:
  BranchingFactor();
  ~BranchingFactor();

  /**
   * The initialization function will precompute the estimated
   * branching factor by simulating the number of placements (in light
   * of domination conditions as well). This data is recorded and used
   * to create a sorted instance based on branching factor.
   */

  void initialize(const Domination* pDomination,
		  const BoxDimensions* pBox,
		  RectPtrArray::iterator iBegin,
		  RectPtrArray::iterator iEnd,
		  const CoordinateRanges* pRanges);
  void initialize(const Domination* pDomination,
		  const BoxDimensions* pBox,
		  const std::vector<Range>& vRanges,
		  const Rectangle* r, UInt* pTotal,
		  UInt* pBranches) const;

  /**
   * This version estimates the branching factor without the use of
   * any domination tables.
   */

  void initialize(const BoxDimensions* pBox,
		  const std::vector<Range>& vRanges,
		  const Rectangle* r, UInt* pTotal,
		  UInt* pBranches) const;
  Rectangle* firstSymmetry(const BoxDimensions& b) const;
  void print() const;
};

#endif // BRANCHINGFACTOR_H
