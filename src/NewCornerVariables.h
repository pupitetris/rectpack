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

#ifndef NEWCORNERVARIABLES_H
#define NEWCORNERVARIABLES_H

#include "Integer.h"
#include <vector>
#include <set>
#include <map>

class BoxDimensions;
class EmptyCorner;
class Component;
class CornerValues;
class Grid;

class NewCornerVariables : public std::vector<std::map<UInt, std::set<Int> > > {
 public:

  enum Corner {
    UpperLeft = 0,
    LowerLeft = 1,
    LowerRight = 2,
    UpperRight = 3
  };

  NewCornerVariables();
  ~NewCornerVariables();
  void initialize(const BoxDimensions& b);

  /**
   * Erases the specified empty corner.
   */

  void erase(const EmptyCorner& ec);

  /**
   * Given a placed component, erase any of the four empty corner
   * variables that this component is filling up. Instead of simply
   * throwing out the erased corners, we insert them into the given
   * set.
   */

  void erase(const Component* c, std::vector<EmptyCorner>& vErased);
  void insert(std::vector<EmptyCorner>& vErased);
  void insert(const EmptyCorner& ec);
  bool empty(Corner c, UInt n) const;

  /**
   * For debugging purposes.
   */

  void print() const;
};

#endif // NEWCORNERVARIABLES_H
