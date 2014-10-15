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

#include "IntPlacements.h"
#include "IntPlacement.h"
#include "Placements.h"
#include <algorithm>

IntPlacements::IntPlacements() {
}

IntPlacements::~IntPlacements() {
}

bool IntPlacements::LessThanY::
operator()(const IntPlacement& l, const IntPlacement& r) {
  return(l.y1() < r.y1());
};

bool IntPlacements::LessThanX::
operator()(const IntPlacement& l, const IntPlacement& r) {
  return(l.x1() < r.x1());
};

void IntPlacements::sortY1() {
  std::sort(begin(), end(), LessThanY());
}

void IntPlacements::sortX1() {
  std::sort(begin(), end(), LessThanX());
}

void IntPlacements::intersectX(UInt x) {
  iterator i(begin());
  while(i != end())
    if(i->overlapsX(x))
      ++i;
    else {
      std::swap(*i, back());
      pop_back();
    }
}

void IntPlacements::intersectY(UInt y) {
  iterator i(begin());
  while(i != end())
    if(i->overlapsY(y))
      ++i;
    else {
      std::swap(*i, back());
      pop_back();
    }
}

void IntPlacements::assignY(IntPlacement& ip, UInt nY1Min,
			    const_iterator& i) const {

  /**
   * Initialize the search for empty space by setting the coordinate
   * to the minimum required.
   */

  ip.m_nY = nY1Min;

  /**
   * Iterate over the stacked set of rectangles, attempting to find an
   * empty space available.
   */

  while(i != end()) {

    /**
     * We continuously loop by exploring y-coordinates on top of the
     * next rectangle. Since all rectangles in this column are assumed
     * to NOT overlap, we simply need to find a placement such that it
     * is below the starting point of the next rectangle.
     */

    if(ip.y2() <= i->y1())
      break;
    ip.m_nY = i->y2();
    ++i;
  }
}

void IntPlacements::assignY(IntPlacements& v) const {

  /**
   * Since the current class object is sorted by y-coordinate, and
   * we're placing unit rectangles, we only have to iterate over the
   * current rectangles in our sequence, and just find openings to
   * place the next unit rectangle.
   */

  UInt yPrevious(0);
  const_iterator iCurrent(begin());
  for(std::vector<IntPlacement>::iterator i = v.begin();
      i != v.end(); ++i) {

    /**
     * Given the current rectangle we have to check for in the class
     * object (this will be incremented by the function we call), and
     * the current minimum coordinate, assign the next IntPlacement.
     */

    assignY(*i, yPrevious, iCurrent);
    yPrevious = i->y2();
  }
}

void IntPlacements::assignY(IntPlacements& v, UInt nX) {
  intersectX(nX);
  sortY1();
  assignY(v);
}

void IntPlacements::assignX(IntPlacement& ip, UInt nX1Min,
			    const_iterator& i) const {
  ip.m_nX = nX1Min;
  while(i != end()) {
    if(ip.x2() <= i->x1())
      break;
    ip.m_nX = i->x2();
    ++i;
  }
}

void IntPlacements::assignX(IntPlacements& v) const {
  UInt xPrevious(0);
  const_iterator iCurrent(begin());
  for(std::vector<IntPlacement>::iterator i = v.begin();
      i != v.end(); ++i) {
    assignX(*i, xPrevious, iCurrent);
    xPrevious = i->x2();
  }
}

void IntPlacements::assignX(IntPlacements& v, UInt nY) {
  intersectY(nY);
  sortX1();
  assignX(v);
}

void IntPlacements::get(Placements& v) const {
  for(const_iterator i = begin(); i != end(); ++i)
    v.push_back(Placement(i->m_pRect, Coordinates(i->x1(), i->y1())));
}

void IntPlacements::print() const {
  for(const_iterator i = begin(); i != end(); ++i)
    i->print();
}
