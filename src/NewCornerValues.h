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

#ifndef NEWCORNERVALUES_H
#define NEWCORNERVALUES_H

#include "AlignedCongruencies.h"
#include "SymmetryMap.h"
#include <vector>
#include <map>

class NewCornerVariables;
class Grid;
class Inferences;

/**
 * Represents the set of values (i.e. components that are eligible to
 * be placed) for a given empty corner. The data structure allows us
 * to look up the values given that we know the empty corner. If the
 * empty corner represents a right or left corner, we look that up in
 * the vector first, and then we secondly use the x-coordinate as the
 * key into a set of component IDs that are eligible to be placed in
 * the given empty corner.
 */

class NewCornerValues : public std::vector<AlignedCongruencies> {
 public:
  enum Side {
    Right = 0,
    Left = 1,
  };

  NewCornerValues();
  ~NewCornerValues();
  void initialize(const AlignedCongruencies& right,
		  const AlignedCongruencies& left);
  void erase(Component* c);
  void insert(Component* c);

  /**
   * Given a set of unassigned variables, we'll check to see if any
   * domains are empty for each variable.
   */

  bool emptyDomain(const NewCornerVariables& cv) const;

  /**
   * Some debugging functions. For every x-coordinate, we print the
   * left and right sets. A "left" set represents the domain values
   * that are available to the left empty corner at the given
   * x-coordinate. In each set, we first have a set of dimensions, and
   * within that, a bunch of component IDs that match that dimension.
   */

  void print() const;
};

#endif // NEWCORNERVALUES_H
