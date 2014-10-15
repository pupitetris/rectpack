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

#ifndef COMPONENT_H
#define COMPONENT_H

#include "Dimensions.h"
#include "Integer.h"
#include "Rational.h"
#include <set>
#include <map>

class Component;
class EmptyCorner;
class EmptySpace;
class Rectangle;
class RectPtrArray;

class Component {
 public:
  Component();
  ~Component();
  bool contains(const Rectangle*) const;

  /**
   * Performs a depth-first search to find the first rectangle that is
   * represented by this component.
   */

  const Rectangle* getFirstRect() const;
  bool sameDims(const Component& c) const;
  bool xOverlaps(const Component& c) const;
  bool yOverlaps(const Component& c) const;

  /**
   * Writes into the nStart and nEnd variables the starting and ending
   * points that represent how much overlap there is in the Y
   * dimension between the current class object and the given
   * component c.
   *
   * @param c the Component against which we will be computing the
   * amount of overlap.
   *
   * @param nStart the starting point of the interval of Y-overlap
   * (inclusive).
   *
   * @param nEnd the ending point of the interval of Y-overlap
   * (inclusive).
   */

  void yOverlap(const Component& c, UInt& nStart,
		UInt& nEnd) const;
  void assignX(const EmptyCorner& ec);
  void assignY(const EmptyCorner& ec);
  bool fills(const EmptyCorner& ec) const;
  void instantiate();
  void instantiateX();
  void instantiateY();
  UInt y2() const;
  UInt x2() const;

  UInt m_nID;
  Int m_nX;
  Int m_nY;
  Dimensions m_Dims;
  bool m_bEmpty;
  bool m_bTopLevel;
  std::set<Component*> m_Members;
  const Rectangle* m_pRect;
  EmptySpace* m_pEmpty;

  /**
   * Some debugging functions.
   */

  void print() const;
  bool checkMembers() const;
};

#endif // COMPONENT_H
