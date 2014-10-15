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

#ifndef INTPLACEMENT_H
#define INTPLACEMENT_H

#include "Integer.h"
#include <vector>

class IntPlacement;
class Rectangle;

class IntPlacement {
 public:
  IntPlacement();
  IntPlacement(const Rectangle* pRect, UInt nX, UInt nY);
  ~IntPlacement();
  bool overlaps(const IntPlacement& i) const;
  bool overlapsX(const IntPlacement& i) const;
  bool overlapsY(const IntPlacement& i) const;
  bool overlapsX(UInt x) const;
  bool overlapsY(UInt y) const;
  
  UInt x1() const;
  UInt x2() const;
  UInt y1() const;
  UInt y2() const;
  void print() const;

  UInt m_nX;
  UInt m_nY;
  const Rectangle* m_pRect;
};

#endif // INTPLACEMENT_H
