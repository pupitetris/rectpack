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

#ifndef INFERENCES_H
#define INFERENCES_H

#include <vector>
#include <map>
#include <set>
#include <boost/graph/adjacency_list.hpp>
#include "AlignedComponents.h"
#include "AlignedCongruencies.h"
#include "Bins.h"
#include "ComponentPtrs.h"
#include "Dimensions.h"
#include "Integer.h"
#include "Rectangle.h"
#include "SymmetryMap.h"
#include "UpperDistance.h"

class BoxDimensions;
class Grid;
class GridViz;
class RectPtrArray;

class Inferences {
 public:

  /**
   * UpperBound represents the graph that will contain an "upper
   * bound" on the X-distance between two squares s1 and s2. If an
   * edge does not exist, then that means we haven't determined what
   * its upper bound should be (effectively, infinity). The vertex
   * contains a boolean representing whether or not it needs to be
   * processed again, while the edge contains an integer representing
   * the upper bound.
   */

  typedef boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS> Adjacency;
  typedef boost::graph_traits<Adjacency> AdjacencyTraits;
  Inferences();
  ~Inferences();
  void clear();
  void initialize(const Bins& vBins,
		  RectPtrArray::iterator i,
		  RectPtrArray::const_iterator j);

  /**
   * Applies all inferences rules until we've reached a steady
   * state. All knowledge is stored in our bitmap, top/bottom data
   * structures, and adjacency graph. We know about empty cells so we
   * consolidate empty columns and rows, and then call
   * computeNonEmpty.
   */

  void compute();

  /**
   * Runs the inference rules that depend on the perfect packing
   * transformation. That is, we compose vertical and horizontal
   * rectangles, and run subset sums.
   */

  bool computeNonEmpty();

  /**
   * Precomputes the top-level Y-overlapping components with respect
   * to every other top-level component. This gives us an upper bound
   * on which other components we must check against in order to
   * ensure non-overlap.
   */

  void computeOverlap();

  /**
   * Maps the remaining empty cells into 1x1 components.
   */

  void emptyToComponents();

  /**
   * Computes the areas of all components as well as updates their
   * minimum dimension.
   */

  void updateComponentAreas();

  /**
   * Propagates the upper bound distance bounds and extracts
   * contiguity constraints.
   */

  void updateAdjacency(const BoxDimensions& b);

  /**
   * @todo code this function to disqualify middle squares from being
   * considered as values for empty corners.
   */

  void updateValidValues();

  /**
   * Attempts to derive top-level domination conditions for the second
   * dimension search. Top-level dominations occur when we have for
   * example a square that spans the entire height of the bounding box
   * (in which case we can force that square to be against the side of
   * the box).
   */

  void updateDomination(const BoxDimensions& b);
  void updateMinSliced(const BoxDimensions& b);
  
  /**
   * Given a set of top and bottom borders that match each other, we
   * solve the partitioning problem to see if all of these borders
   * must be adjacent (and store such results in the
   * m_vBottomsAdjacent vector). We only record such an assertion if
   * we can prove that adjacency must be enforced. Otherwise, we don't
   * infer any other relationships.
   */

  bool partitioning();

  /**
   * A piecemeal function of the main one, that reasons only between
   * two bins and puts the result in m_vBottomsAdjacent.
   */

  bool partitioning(UInt nBottom, UInt nTop);

  /**
   * Detects cases when two components must be stacked one above the
   * other, and can be consolidated into a larger, exactly rectangular
   * component. This is done by analysis of the bordering constraint
   * and is applied only when we can prove that there are exactly two
   * components bordering each other of the same width.
   */

  bool consolidateComponentsV();

  /**
   * Iterate over all X-adjacent edges and find two top-level
   * components who have the same heights and the same Y-values. Note
   * that all components in our data structures are top-level already,
   * except for those in m_vComponents.
   */

  bool consolidateComponentsH();

  /**
   * Detects cases when we have a column of empty cells but these
   * empty cells slice all other squares horizontally. Therefore, they
   * must be stacked on top of each other according to the bordering
   * constraint.
   */

  bool consolidateEmptyColumns();

  /**
   * Whenever a border has only empty space, then every item in the
   * counterpart gets assigned a group of empty cells, that represent
   * essentially the row of cells. Furthermore, we can glue this row
   * of empty cells onto that other item.
   */

  bool consolidateEmptyRows();

  /**
   * Combine two components together into a new component, or adds one
   * component to another one. This version does not check the
   * dimensionality of the inputs, but combines the second component
   * into the first component.
   *
   * @return the component that now represents the two input
   * components.
   */

  UInt combineV(UInt n1, UInt n2);
  UInt combineH(UInt n1, UInt n2);
  UInt combineH(const std::list<UInt>& l);
  void replace(UInt n1, UInt n2, UInt nNew);
  void replace(const std::list<UInt>& l, UInt nNew);

  /**
   * Checks the dimensionality of two components to see if they can be
   * grouped together either vertically or horizontally. Notice that
   * these checks only check the dimensionality and not their
   * positions. It is up to the caller to ensure that they actually
   * are adjacent to each other and share a common X or Y value.
   */

  bool canGroupH(UInt n1, UInt n2);
  bool canGroupV(UInt n1, UInt n2);
  bool hasSubset(UInt nTarget, UInt nCurrent, UInt nEmpty,
		 std::set<UInt>::const_iterator iBegin,
		 std::set<UInt>::const_iterator iEnd) const;
  void excludeWidth(const std::set<UInt>& s,
		    std::set<UInt>::const_iterator& iBegin,
		    std::set<UInt>::const_iterator& iEnd,
		    UInt& nExcluded) const;
  bool updateAdjacencyList(UInt nBottomItems,
			   UInt nTopItems,
			   const std::set<UInt>& s1,
			   const std::set<UInt>& s2);

  /**
   * Remaps all vertices named nOld to nNew in the adjacency graph
   * g. All edges referencing the old vertex will be remapped to point
   * to the new vertex and the edges connecting to the old vertex will
   * be cleared.
   *
   * @param nOld the label of the old vertex.
   *
   * @param nNew the label of the new vertex.
   */

  void remap(UInt nOld, UInt nNew, Adjacency& g);
  void remap(const std::list<UInt>& l, UInt nNew, Adjacency& g);
  bool boundsAdjacency();
  bool boundsAdjacency(UInt nBottom, UInt nTop);

  /**
   * @param nWidth the width of the largest rectangle participating in
   * the border.
   *
   * @param nRest the remaining length of the border (without the
   * largest rectangle as a part of this length).
   */

  bool boundsAdjacency(UInt nWidth, UInt nRest, bool bBottom,
		       const std::set<UInt>& s);
  bool canFill(UInt nStart, UInt nEnd, UInt n1, UInt n2, UInt nSpace);
  bool allGreaterThan(UInt nIgnore1, UInt nIgnore2, UInt nMin,
		      const std::set<UInt>& s);
  //bool overlaps(int c, const std::vector<Int>& v) const;

  /**
   * Adds a new component as well as new vertices in our adjacency
   * graphs to represent this.
   */

  int newComponent();

  /**
   * Inserts the component n into the bitmap, tops, and bottoms data
   * structures. Notice that this does not actually allocate a new
   * Component data object.
   */

  void addComponent(UInt n);
  void findSymmetries();
  void surroundingHeight(UInt h, const std::set<UInt>& s,
			 std::list<UInt>& l) const;
  bool isBookEnd(UInt c, const std::set<UInt>& s) const;

  ComponentPtrs m_vComponents;
  std::vector<UInt> m_vEmptySpace;
  AlignedComponents m_Tops;
  AlignedComponents m_Bottoms;
  std::vector<std::set<UInt> > m_vBitmap;

  /**
   * Represents, for a given coordinate (key), all of the component
   * IDs with the same dimensions. These objects are
   * symmetric/congruent to one another.
   */

  AlignedCongruencies m_TopSymmetries;
  AlignedCongruencies m_BottomSymmetries;

  /**
   * Specifies when two components c1 and c2 must be adjacent along
   * the x-axis. That is, they are side by side.
   */

  Adjacency m_X;

  /**
   * Specifies when two components c2 and c2 must be adjacent along
   * the y-axis. That is, they are stacked.
   */

  Adjacency m_Y;
  UpperDistance m_XDist;

  /**
   * Specifies whether or not the bottoms in the given index must be
   * somehow in an adjacent row (but not necessarily all pairwise
   * adjacent).
   */

  std::vector<bool> m_vBottomsAdjacent;
  std::list<std::pair<UInt, UInt> > m_lDomination;

  /**
   * For debugging purposes.
   */

  void print() const;
  void print2() const;
  bool checkRows() const;

  BoxDimensions* myb;

  bool m_bAdjacency;
  bool m_bDistance;
  bool m_bSubsetSums;
  bool m_bComposition;
  bool m_bEmptyComposition;
};

#endif // INFERENCES_H
