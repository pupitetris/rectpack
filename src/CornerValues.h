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

#ifndef CORNERVALUES_H
#define CORNERVALUES_H

#include "AlignedCongruencies.h"
#include "SymmetryMap.h"
#include <vector>
#include <map>

class CornerVariables;
class Grid;
class Inferences;

class CornerValues : public std::vector<AlignedCongruencies> {
 public:
  enum Side {
    Above = 0,
    Below = 1,
  };

  CornerValues();
  ~CornerValues();
  void initialize(const AlignedCongruencies& above,
		  const AlignedCongruencies& below);
  void erase(Component* c);
  void insert(Component* c);

  /**
   * Given a set of unassigned variables, we'll check to see if any
   * domains are empty for each variable.
   */

  bool emptyDomain(const CornerVariables& cv) const;

  /**
   * For every row that has values, we'll scan across the grid, making
   * sure that there is in fact enough room to place square.
   */

  bool emptyDomain(const Grid* pGrid, const Inferences& inf) const;

  /**
   * Some debugging functions.
   */

  void print() const;
};

#endif // CORNERVALUES_H
