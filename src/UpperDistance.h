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

#ifndef UPPERDISTANCE_H
#define UPPERDISTANCE_H

#include <boost/graph/adjacency_list.hpp>
#include <set>
#include <vector>

class AlignedComponents;
class BoxDimensions;
class Component;
class ComponentPtrs;

class UpperDistance : public std::vector<std::vector<UInt> > {
 public:
  typedef boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS> Adjacency;
  typedef boost::graph_traits<Adjacency> AdjacencyTraits;
  UpperDistance();
  ~UpperDistance();

  /**
   * Initializes the current data structure. The set of top-level
   * components are extracted and compressed (so that our matrix does
   * not represent lower-level components), building forward and
   * backward mappings. We also initialize the pairwise upper bounds
   * based on the maximum distance allowed by the bounding box.
   */

  void initialize(const ComponentPtrs& v, const BoxDimensions& b);
  void compute(const AlignedComponents& vTops,
	       const AlignedComponents& vBottoms,
	       const std::vector<bool>& vBottomsAdjacent,
	       const std::vector<std::set<UInt> >& vBitmap,
	       Adjacency& y);

  /**
   * Given two borders b1 and b2, an upper bound on the distance
   * between two components c1 in b1 and c2 in b2 is computed by
   * simply taking the sum of the widths of c1 and c2. If this exceeds
   * the length of the border, then they must overlap and we can
   * quickly compute this amount.
   */

  void computeConsumption(UInt nWidth,
			  const std::set<UInt>& tops,
			  const std::set<UInt>& bottoms);

  /**
   * Computes the total border width of the border given by the top
   * and bottom sets. We expect the set contents to include the
   * original, uncompressed component IDs.
   */

  UInt width(const std::set<UInt>& s) const;

  /**
   * Tightens the upper bound by enforcing all members of the given
   * set be constrained to an adjacent border width.
   */

  void forceBorder(UInt nWidth, const std::set<UInt>& s);
  void forceBorder(UInt nWidth, const std::set<UInt>& s1,
		   const std::set<UInt>& s2);
  void forceBorder(UInt nWidth, int c, const std::set<UInt>& s);

  /**
   * Runs one iteration, scanning over every pair of squares, testing
   * to see if the spacing between them can be filled by anything, or
   * if we're left simply with a vacuum. If there's a vacuum we will
   * update the bounds to represent the new adjacency relation.
   *
   * @return true if bounds were updated and false otherwise.
   */

  bool tightenBounds(const std::vector<std::set<UInt> >& vBitmap);
  bool tightenY(const AlignedComponents& vTops,
		const AlignedComponents& vBottoms, Adjacency& y);

  /**
   * Checks to see if all components in the given set are larger than
   * the minimum size. The two given compressed component IDs are
   * ignored as we iterate over the set (it is assumed that the two
   * compressed component IDs belong to the two squares that are
   * creating the empty channel.)
   */

  bool allGreaterThan(UInt nIgnore1, UInt nIgnore2, UInt nMin,
		      const std::set<UInt>& s) const;

  /**
   * Given the uncompressed component ID, this function returns a
   * pointer to the component class object.
   */

  const Component* component(int i) const;

  /**
   * Takes in two uncompressed component IDs, and a new distance bound
   * for the edge of i to j, looks up our internal component
   * representation and updates the bound if it happens to be tighter.
   *
   * @param i the uncompressed source component ID.
   *
   * @param j the uncompressed target component ID.
   *
   * @param nNew the upper bound distance from i to j.
   *
   * @return true if the upper bound changed, or false otherwise.
   */

  bool updateBound(UInt i, UInt j, UInt nNew);

  /**
   * Takes in two compressed component IDs, and a new distance bound
   * for the edge of i to j, and updates the bound if it happens to be
   * tighter.
   *
   * @param i the compressed source component ID.
   *
   * @param j the compressed target component ID.
   *
   * @param nNew the upper bound distance from i to j.
   *
   * @return true if the upper bound changed, or false otherwise.
   */

  bool updateBound2(UInt i, UInt j, UInt nNew);

  /**
   * Checks to see if the upper bound gap between compressed
   * components i and j can be filled by items in the bitmap.
   */

  bool canFill(UInt i, UInt j,
	       const std::vector<std::set<UInt> >& vBitmap) const;

  /**
   * Given a start and end Y-value and two compressed component IDs,
   * and the given gap, this function attempts to fill in their gap
   * using items from the bitmap.
   */

  bool canFill(UInt nStart, UInt nEnd, UInt n1, UInt n2,
	       UInt nSpace,
	       const std::vector<std::set<UInt> >& vBitmap) const;

  /**
   * Returns whether or not two components are X-adjacent to each
   * other (i.e. there is no space allowed between them).
   *
   * @param i the source compressed component index.
   *
   * @param j the target compressed component index.
   *
   * @return true if the upper bound admits 0 spacing between the two
   * and false otherwise.
   */

  bool xAdjacent(UInt i, UInt j) const;
  bool yAdjacent(UInt i, UInt j,
		 const std::vector<UInt>& vSliced) const;
  bool yAdjacent(UInt i, UInt j) const;

  /**
   * Returns whether or not two components overlap in the Y dimension.
   *
   * @param i the the source compressed component index.
   *
   * @param j the target compressed component index.
   *
   * @return true if the upper bound admits 0 spacing between the two
   * and false otherwise.
   */

  bool yOverlaps(UInt i, UInt j) const;

  /**
   * Computes the Floyd-Warshall all-pairs-shortest-path algorithm on
   * the current matrix.
   */

  bool asps();

  /**
   * Returns the upper bound distance between two compressed
   * components i to j.
   */

  UInt& get(UInt i, UInt j);

  /**
   * Returns the upper bound distance between two compressed
   * components i to j.
   */

  const UInt& get(UInt i, UInt j) const;
  std::vector<const Component*> m_vComponents;
  std::vector<UInt> m_vComponentMap;

  /**
   * For purposes of debugging.
   */

  void print() const;
};

#endif // UPPERDISTANCE_H
