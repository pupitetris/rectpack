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

#ifndef DYNAMICEMPTY_H
#define DYNAMICEMPTY_H

#include "AlignedComponentPtrs.h"
#include "AlignedCongruencies.h"
#include "Assignment.h"
#include "BinomialCache.h"
#include "Bins.h"
#include "NewCornerValues.h"
#include "NewCornerVariables.h"
#include "EmptyCorner.h"
#include "Inferences.h"
#include "Integer.h"
#include "NodeCount.h"
#include "PerfectTransform.h"
#include "SimpleSums.h"
#include "SpaceFill.h"
#include "SymmetryMap.h"
#include "WastedBins.h"
#include <string>
#include <vector>

class BoxDimensions;
class Cumulative;
class Packer;
class Parameters;
class TimeSpec;

/**
 * Represents the data structures, logic, and everything related to
 * the task of perfect packing. This particular class includes
 * inference rules to avoid the creation of 1x1 empty space
 * rectangles.
 */

class DynamicEmpty : public SpaceFill {

  enum Corner {
    UpperLeft = 0,
    LowerLeft = 1,
    LowerRight = 2,
    UpperRight = 3
  };

 public:
  DynamicEmpty();
  ~DynamicEmpty();
  void initialize(Packer* pPacker);
  void initialize(const Parameters* pParams);
  void initialize(const BoxDimensions* pBox);
  virtual bool packY(Cumulative& c, const SubsetSums* pHeights);
  bool packYAux(Int nDepth);
  bool packYCorner(EmptyCorner& ec, Int nDepth);
  void smallestDomain(UInt& nSmallest, EmptyCorner& c);

  /**
   * Immediately places any rectangles that have dominated positions,
   * such as those spanning the width of the bounding box.
   */

  void placeDominated(Int& nStartDepth);
  void findTopSymmetry();
  void computeValidY();

  /**
   * @param bResult a variable returning the result of the search.
   * 
   * @return true if we have a case of stacking items into the corner,
   * and false otherwise.
   */

  bool stackCorner(EmptyCorner& ec, SymmetryMap* pValues,
		   Int nDepth, bool& bResult);

  /**
   * Picks the smallest domain based on
   * SymmetryMap::symPermutations. Break ties in favor of the set that
   * has the smallest number of components.
   */

  void smallestDomain(UInt& nSize, UInt& nComponents,
		      std::map<UInt, std::set<Int> >& vars,
		      AlignedCongruencies& vals,
		      EmptyCorner& c, EmptyCorner::Corner nType);
  void newCorners(const Component* c, std::vector<EmptyCorner>& v) const;
  SymmetryMap* getValues(const EmptyCorner& ec);
  void push();
  void pop();
  void pop(Component* c);
  void pushAux(Component* c);
  void pushAux(Component* c, Component* pNew);
  void popAux();
  void popAux(Component* c);
  bool insideBox(const Dimensions& d, const EmptyCorner& c) const;
  bool insideBox(const Dimensions& d, const ICoords& c) const;
  bool placed(Component* c) const;
  bool canPlace(const Component* c) const;
  void get(Placements& v) const;
  void get(IntPlacements& v) const;

  /**
   * Computes the maximum height possible for an object placed in the
   * given empty corner, with the given width. We check this against
   * the bounding box as well as against rectangles that have been
   * placed in our stack.
   */

  UInt maxHeight(const EmptyCorner& ec, const UInt& nWidth) const;

  /**
   * Some debugging functions.
   */

  void print() const;
  void printStack() const;

  /**
   * Data structures that are given to us by the calling class that we
   * need to modify in order to monitor the search correctly.
   */

  UInt* m_pCuSP;
  const BoxDimensions* m_pBox;
  TimeSpec* m_pTime;
  NodeCount* m_pNodes;

  /**
   * Some indexing and inference classes.
   */

  AlignedComponentPtrs m_Left;
  AlignedComponentPtrs m_Right;
  AlignedCongruencies m_LeftCongruencies;
  AlignedCongruencies m_RightCongruencies;
  PerfectTransform m_Transform;

  /**
   * Variables internal for our search.
   */

  std::vector<UInt> m_vStackSize;
  std::vector<Assignment> m_vStack;
  int m_nDepth;
  NewCornerValues m_vValues;
  NewCornerVariables m_vVariables;
  BinomialCache m_BCache;
  unsigned long long m_nCorners;

  /**
   * Tracks which components have been placed due to them being
   * dominated.
   */

  std::deque<Component*> m_vDominated;

  /**
   * Pointer to a set of subset sums that represent valid
   * y-coordinates.
   */

  const SubsetSums* m_pValidY;
  bool m_bDominated;
  Component* m_pTopSymmetry;

  BoxDimensions m_NewBox;
  UInt m_nNewY;
  SimpleSums m_ValidY;
};

#endif // DYNAMICEMPTY_H
