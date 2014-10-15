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

#ifndef PACKER_H
#define PACKER_H

#include "Bins.h"
#include "BoxDimensions.h"
#include "Coordinates.h"
#include "DuplicateIndex.h"
#include "Integer.h"
#include "LockableState.h"
#include "NodeCount.h"
#include "RectArray.h"
#include "RectPtrArray.h"
#include "TimeSpec.h"
#include <boost/thread/shared_mutex.hpp>
#include <deque>
#include <vector>
#include <map>

class BoundingBoxes;
class HeapBox;
class Parameters;
class Placements;
class Rectangle;
class SearchControl;
class TimeSpec;

class Packer {

 public:
  Packer();
  virtual ~Packer();

  /**
   * Called after we have instantiated the packer.
   */

  virtual void initialize(Packer* pPacker);

  /**
   * Called when we get a new instance description.
   */

  virtual void initialize(const Parameters* pParams);

  /**
   * Called when we have instantiated and analyzed the entire set of
   * bounding boxes.
   */

  virtual void initialize(const BoundingBoxes* pBoxes);

  /**
   * Called when we get a new box dimension to pack for.
   */

  virtual void initialize(const HeapBox* pBox);
  static Packer* create(const Parameters* pParams);

  /**
   * Call that is made by the bounding box.
   */

  virtual bool pack(const HeapBox& b);
  virtual bool packcra(const std::string& sFile);
  void printNodes() const;
  virtual void get(Placements& v) const = 0;
  const BoundingBoxes* m_pBoxes;

  virtual bool pack() = 0;
  virtual bool packcra();

  /**
   * Variables used in the context of search.
   */

  boost::shared_mutex m_BoxMutex;
  BoxDimensions m_Box;
  const Parameters* m_pParams;

  /**
   * Number of nodes generated, and their running totals.
   */

  NodeCount m_Nodes;

  /**
   * CPU times.
   */

  TimeSpec m_Time;
  TimeSpec m_XTime;
  TimeSpec m_YTime;

  /**
   * CuSP solutions.
   */

  UInt m_nCuSP;

  /**
   * An array of class objects that represent what we will be packing
   * into our bounding box. The nth element in this array corresponds
   * to a square of size n. Therefore, there are n+1 elements if the
   * problem instance size is n.
   */

  RectArray m_vRects;
  RectArray m_vOriginalRects;

  /**
   * An array of class object pointers which corresponds to exactly
   * the reverse sequence of m_vSquares except it does not contain the
   * 0th element of m_vSquares. This sequence shouldn't be directly
   * indexed into -- instead it should be used simply as a sequence of
   * squares to pack. We're trying to decouple the size of the square
   * from its index in this array.
   */

  RectPtrArray m_vRectPtrs;

  /**
   * Sets the search control as well as the visualization type.
   */

  void set(SearchControl* pControl);
  bool m_bInitialized;

#ifdef VIZ
  LockableState m_State;
#endif // VIZ

 public:

  /**
   * Determines if we will be remapping the instance in order to force
   * the y dimension to be smaller than the x dimension. Generally
   * this will be true for the integer packers for legacy reasons.
   */

  bool m_bYSmallerThanX;
  DuplicateIndex m_Duplicates;

  /**
   * Determines if this packer can be used to derive domination
   * tables.
   */

  bool m_bCanBuildDomination;

  /**
   * Accessor functions for retrieving information.
   */

  virtual TimeSpec& timeDomination() = 0;

  /**
   * Iterator that points to the first 1x1 in the sequence -- it is
   * assumed that at some point the remaining sequence will simply be
   * a set of 1x1s.
   */

  void firstUnitRectangle();
  virtual void placeUnitRectangles() = 0;
  RectPtrArray::iterator m_iFirstUnit;
  UInt m_nNonUnits;

  virtual void initAccumulators();
  void print() const;
  SearchControl* m_pControl;

  /**
   * Tracks the deepest conflict
   */

  UInt m_nDeepestConflict;
  bool m_bInstanceRotated;
};

#endif // PACKER_H
