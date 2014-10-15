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

#ifndef INTPLACEMENTS_H
#define INTPLACEMENTS_H

#include "IntPlacement.h"
#include <vector>

class IntPlacements;
class Placements;

class IntPlacements : public std::vector<IntPlacement> {
 public:
  IntPlacements();
  ~IntPlacements();

  /**
   * Assigns y-coordinates to a set of unit rectangles that are
   * presumed to be located at the given x-coordinate such that no
   * unit rectangle overlaps any rectangle in the current class
   * object. We're required to sort the current class object, which is
   * why the function is not const.
   *
   * @param v the vector in which we will be writing all of the
   * placement data. We expect the vector to be properly sized to the
   * number of desired placements.
   *
   * @param nX the x-coordinate which we will be assigning unit
   * rectangles to.
   */

  void assignY(IntPlacements& v, UInt nX);

  /**
   * Assigns x-coordinates to a set of unit rectangles that are
   * presumed to be located at the given y-coordinate such that no
   * unit rectangle overlaps any rectangle in the current class
   * object. We're required to sort the current class object, which is
   * why the function is not const.
   *
   * @param v the vector in which we will be writing all of the
   * placement data. We expect the vector to be properly sized to the
   * number of desired placements.
   *
   * @param nY the y-coordinate which we will be assigning unit
   * rectangles to.
   */

  void assignX(IntPlacements& v, UInt nY);
  void get(Placements& v) const;

 private:

  /**
   * Sorts all of the placements in this array by their y-coordinate.
   */

  void sortY1();

  /**
   * Sorts all of the placements in this array by their x-coordinate.
   */

  void sortX1();

  /**
   * Assigns each entry in the given vector to a y-coordinate position
   * such that no item overlaps with the current class object. We
   * assume that all items to be placed are unit rectangles.
   *
   * @param v the array in which we will be assigning the
   * coordinates. This array is expected to already have been properly
   * sized to the number of desired unit rectangles.
   */

  void assignY(IntPlacements& v) const;

  /**
   * Assigns each entry in the given vector to a x-coordinate position
   * such that no item overlaps with the current class object. We
   * assume that all items to be placed are unit rectangles.
   *
   * @param v the array in which we will be assigning the
   * coordinates. This array is expected to already have been properly
   * sized to the number of desired unit rectangles.
   */

  void assignX(IntPlacements& v) const;
  void assignY(IntPlacement& ip, UInt nY1Min, const_iterator& i) const;
  void assignX(IntPlacement& ip, UInt nX1Min, const_iterator& i) const;
  void intersectX(UInt x);
  void intersectY(UInt y);
  void print() const;

  struct LessThanX {
    bool operator()(const IntPlacement& l, const IntPlacement& r);
  };

  struct LessThanY {
    bool operator()(const IntPlacement& l, const IntPlacement& r);
  };
};

#endif // INTPLACMENTS_H
