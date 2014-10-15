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

#ifndef INSTANCE_H
#define INSTANCE_H

#include "DimsFunctor.h"
#include "Integer.h"
#include "Rational.h"
#include "RDimensions.h"
#include <deque>
#include <set>
#include <ostream>

class Instance;

class Instance : public std::deque<RDimensions> {
  friend std::ostream& operator<<(std::ostream& os,
                                  const Instance& is);

 public:
  Instance();
  Instance(const Instance& i);
  const Instance& operator=(const Instance& i);
  ~Instance();
  void parseInstance(const std::string& s);
  void parseInstance(std::deque<UInt>& v);
  void parseInstance(std::deque<UInt>& v,
		     std::deque<UInt>& vLower,
		     std::deque<UInt>& vUpper);
  Instance& operator/=(const URational& n);
  Instance& operator*=(const URational& n);
  void roundUp();
  void roundDown();

  /**
   * Sorts the current instance according to that specified in the
   * given parameter. The different orders are defined in Parameters.
   */

  void sort(int nOrdering);
  void sort(int nOrdering, bool bYSmallerThanX);
  void sortDecreasingArea();
  void sortIncreasingArea();
  void sortDecreasingMinDim();
  void sortDecreasingHeight();
  void sortDecreasingWidth();
  void sortIncreasingWH();
  void sortIncreasingHW();

  /**
   * Sorts the rectangles in order of H>W, H=W, then W>H, and then
   * within that constraint, we sort the items by decreasing area.
   */

  void inferInstanceProperties();
  void inferInstanceOrientation();
  void inferInstanceUniqueness(const std::multiset<RDimensions>& s);
  void inferInstanceSorting(const std::multiset<RDimensions>& s);
  void inferInstanceSequential(const std::multiset<RDimensions>& s);

  /**
   * Determines, based on the problem instance, whether or not we have
   * a symmetric instance along the diagonal of the bounding box. This
   * occurs naturally if all rectangles are unoriented. It also occurs
   * when all items are either squares, or each item has a rectangle
   * pair where the width and heights are reversed. In the case that
   * there are multiple symmetric items (in the oriented case), there
   * must be a 1-to-1 pairing of rectangles whose widths and heights
   * are reversed. Notice that if one rectangle were oriented but
   * there were another rectangle (of similar width and height) that
   * were unoriented, this doesn't count as a matched pair.
   */

  void inferInstanceSymmetry(const std::multiset<RDimensions>& s);

  /**
   * Infers certain numeric properties about the instance, such as the
   * maximums and minimums of different dimensions, etc., including
   * computing the areas.
   */

  void inferInstanceExtrema();
  void rewriteJValue(float nJValue);
  bool rotatable(const RDimensions& r) const;
  bool oriented(const RDimensions& r) const;
  bool square(const RDimensions& r) const;
  void rotate();

  /**
   * Given that all rectangles are constrained in the first dimension
   * by nMax, this computes the minimum other dimension based on
   * pairwise placements.
   *
   * @param nMax the constraint on the first dimension.
   *
   * @param pDims a functor that determines which dimension is first
   * and which is the second.
   *
   * @return the minimum size of the second dimension.
   */

  URational minDimPairs(const URational& nMax,
			const DimsFunctor* pDims) const;

  /**
   * Given that all rectangles are constrained in the first dimension
   * by nMax, this computes the minimum other dimension based on a
   * series of rectangles that must be stacked on each other since
   * they exceed half of the first dimension.
   *
   * @param nMax the maximum size of the first dimension.
   *
   * @param pDims a functor that determines which dimension is first
   * and which is the second.
   *
   * @return the minimum sizeo f the second dimension.
   */

  URational minDimStacked(const URational& nMax,
			  const DimsFunctor* pDims) const;

  /**
   * Given that all rectangles are constrained in the first dimension
   * by nMax, this computes the minimum other dimension based on a
   * series of rectangles that must be stacked on each other since
   * they exceed half of the first dimension. This is computed
   * differently from the previous function, and therefore it is
   * useful to employ both constraints to tighten the bound.
   *
   * @param nMax the maximum size of the first dimension.
   *
   * @param pDims a functor that determines which dimension is first
   * and which is the second.
   *
   * @param bOpenInterval a return parameter that specifies whether
   * the interval returned is open or closed.
   *
   * @return the minimum size of the second dimension. Note that this
   * result is an open interval. That is, the actual bound must be
   * greater than this number by some small value epsilon.
   */

  URational minDimStacked2(const URational& nMax,
			   const DimsFunctor* pDims,
			   bool& bOpenInterval) const;

  /**
   * For packers that can only handle integral descriptions, this
   * function scales the description and recomputes the instance
   * inferences.
   */

  void scaleDiscrete();
  
  /**
   * Attempts to reduce the instance by the greatest common divisor.
   */

  void scaleDivisor();
  void print() const;

  int m_nBenchmark;
  std::string m_sInstance;
  bool m_bSequential;
  bool m_bUnique;
  bool m_bAllSquares;
  bool m_bUnoriented;
  bool m_bOrientedNonSquares;
  bool m_bDiagonalSymmetry;
  bool m_bSortedDecreasingArea;
  bool m_bSortedDecreasingMinDim;
  bool m_bSortedDecreasingWidth;

  /**
   * Both widths and heights of every successive square is not greater
   * than the minimum dimension of the previous square.
   */

  bool m_bSortedDecreasingSize;

  URational m_nTotalArea;
  URational m_nMinArea;
  URational m_nMaxArea;
  RDimensions m_nMax;
  RDimensions m_nMin;

  /**
   * The maximum width or height over all minimum widths (or
   * heights). This will differ from m_nMax only in the case of
   * unoriented rectangles. This is useful when computing the minimum
   * heights and widths required for possibly feasible bounding boxes.
   */

  RDimensions m_nMaxMin;
  RDimensions m_nStacked;
  URational m_nMaxDim;
  URational m_nMinDim;

  /**
   * Represents the ratio original:current, that scales the problem
   * instance. For a scaled number, to get back the original, multiply
   * it by this value.
   */

  URational m_nScale;

  /**
   * Represents the interval ratio to apply 
   */

  URational m_nInterval;
};

std::ostream& operator<<(std::ostream& os, const Instance& is);

#endif // INSTANCE_H
