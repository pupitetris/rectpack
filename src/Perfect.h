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

#ifndef PERFECT_H
#define PERFECT_H

#include "Assignment.h"
#include "BinomialCache.h"
#include "Bins.h"
#include "CornerValues.h"
#include "CornerVariables.h"
#include "EmptyCorner.h"
#include "Inferences.h"
#include "Integer.h"
#include "NodeCount.h"
#include "SymmetryMap.h"
#include "WastedBins.h"
#include <string>
#include <vector>

class BoxDimensions;
class TimeSpec;
class Grid;

/**
 * Represents the data structures, logic, and everything related to
 * the task of perfect packing.
 */

class Perfect {

  enum Corner {
    UpperLeft = 0,
    LowerLeft = 1,
    LowerRight = 2,
    UpperRight = 3
  };

 public:
  Perfect();
  ~Perfect();
  void initialize(UInt* pCuSP, BoxDimensions* pBox,
		  TimeSpec* pTime, Grid* pGrid, NodeCount* pNodes);
  void initialize(const std::string& sParams);

  bool packX(Bins* pBins, RectPtrArray::iterator iBegin,
	     const RectPtrArray::const_iterator& iEnd);
  bool packXAux(Int nDepth, const WastedBins& v);
  bool packXCorner(EmptyCorner& ec, Int nDepth, const WastedBins& v);
  bool packXAdjacency(Component* c, const std::list<ICoords>& l,
		      Int nDepth, const WastedBins& v);
  void initWastedSpace();

  void smallestDomain2(UInt& nSmallest, EmptyCorner& c);
  void smallestDomain6(UInt& nSize, UInt& nComponents,
		       std::map<UInt, std::set<Int> >& vars,
		       AlignedCongruencies& vals,
		       EmptyCorner& c,
		       EmptyCorner::Corner nType);
  void smallestDomain3(UInt& nSmallest, EmptyCorner& c,
		       const Grid* pGrid);
  void smallestDomain7(UInt& nSize, UInt& nComponents,
		       std::map<UInt, std::set<Int> >& vars,
		       AlignedCongruencies& vals,
		       EmptyCorner& c,
		       EmptyCorner::Corner nType, const Grid* pGrid);
  
  void adjacencySize(UInt& nSize, std::list<ICoords>& l,
		     Component*& c);
  void adjacencySize(std::list<ICoords>& ls,
		     const Component* pPlaced,
		     const Component* pUnplaced) const;
  void newCorners(const Component* c, std::vector<EmptyCorner>& v) const;
  void newCornersA(const Component* c, std::vector<EmptyCorner>& v) const;
  void newCornersB(const Component* c, std::vector<EmptyCorner>& v) const;
  void newCornersL(const Component* c, std::vector<EmptyCorner>& v) const;
  void newCornersR(const Component* c, std::vector<EmptyCorner>& v) const;
  SymmetryMap* getValues(const EmptyCorner& ec);
  void push();
  void pop();
  void pushAux(Component* c, const WastedBins& v);
  void popAux();
  bool insideBox(const Dimensions& d, const EmptyCorner& c) const;
  bool insideBox(const Dimensions& d, const ICoords& c) const;
  void forceDominationLeft(UInt c1, UInt c2, const WastedBins& v);
  void forceDominationLeft(UInt c, const WastedBins& v);
  void forceDominationRight(UInt c1, UInt c2, const WastedBins& v);
  bool placed(Component* c) const;

  /**
   * Some debugging functions.
   */

  void print() const;

  /**
   * Data structures that are given to us by the calling class that we
   * need to modify in order to monitor the search correctly.
   */

  UInt* m_pCuSP;
  BoxDimensions* m_pBox;
  TimeSpec* m_pTime;
  Grid* m_pGrid;
  NodeCount* m_pNodes;

  /**
   * 2D wasted space data structures.
   */

  WastedBins m_vWastedXF;
  std::vector<WastedBins> m_vWastedX;

  /**
   * Variables internal for our search.
   */

  std::vector<UInt> m_vStackSize;
  std::vector<Assignment> m_vStack;
  int m_nDepth;
  Inferences m_Inferences;
  CornerValues m_vValues;
  CornerVariables m_vVariables;
  BinomialCache m_BCache;
  unsigned long long m_nAdjacency;
  unsigned long long m_nCorners;
  bool m_bTopLevelDomination;

  /**
   * Uses the vertical and horizontal grid estimates to filter the
   * domains of each corner before using the result to report which is
   * the most constrained variable. Leaving this on will cause the
   * packer to check each domain value to ensure that the dimensions
   * as reported by the vertical and horizontal grids for that single
   * corner can accommodate the given rectangle. By default it is on.
   */

  bool m_bFilterDomainWithGrid;

  /**
   * Since we deduce when rectangles are contiguous to one another,
   * once we place one in a pair, we can place its partner almost
   * immediately. If not, we only have two choices anyway. With this
   * flag turned on, we will consider the number of placements for a
   * contiguous partner in our variable order. Turning this off means
   * we will not consider these partial contiguous variables. By
   * default this will be on.
   */

  bool m_bForceContiguousRectangles;

  /**
   * With this flag on, we will consider all four orientations of the
   * empty corner for placing rectangles into. Without it we only
   * consider the lower left empty corner. By default, this flag is
   * on.
   */

  bool m_bAllFourCorners;

  std::vector<bool> m_vPlaced;
};

#endif // PERFECT_H
