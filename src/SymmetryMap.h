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

#ifndef SYMMETRYMAP
#define SYMMETRYMAP

#include "Dimensions.h"
#include "Integer.h"
#include <list>
#include <map>
#include <set>

class BinomialCache;
class Component;

class SymmetryMap : public std::map<Dimensions, std::set<Component*> > {
 public:
  SymmetryMap();
  ~SymmetryMap();

  /**
   * Returns the number of search nodes to try all possible
   * combinations and permutations of assignment, subject to our
   * knowledge of symmetries. This is computed iteratively over the
   * number of entries (sets of identical items) in our map. For each
   * set, we compute the solution to the "bars and stars" problem
   * where the number of stars is the total number of all previously
   * processed elements, and the bars is the number of elements in the
   * current set. Each result is multiplied in a cumulative fashion.
   *
   * @param c a cache of previously computed binomial coefficient
   * values.
   */

  UInt symPermutations(BinomialCache& c) const;

  /**
   * This function is similar to the one previously defined, with the
   * exception that it understands the multiplicity member variable in
   * the component class.
   */

  UInt symPermutations2(BinomialCache& c) const;
  UInt symPermutations3(BinomialCache& c) const;
  UInt symPermutations(BinomialCache& c, Int nWidth,
		       Int nHeight) const;

  /**
   * Tests to see if this represents an empty domain. Either there are
   * no sets, or in each set, there are no elements.
   */

  bool emptyDomain() const;

  /**
   * Applies the maximum width and height filter and checks to see if
   * there are any remaining values that are available.
   */

  bool emptyDomain(Int nWidth, Int nHeight) const;
  UInt components() const;
  UInt components(Int nWidth, Int nHeight) const;
  Dimensions largestArea() const;
  Dimensions largestHeight() const;
  Dimensions largestWidth() const;
  void hasHeight(UInt h, std::list<Int>& l) const;
  void print() const;
  void print(std::ostream& os) const;
};

#endif // SYMMETRYMAP
