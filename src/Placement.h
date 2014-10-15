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

#ifndef PLACEMENT_H
#define PLACEMENT_H

#include "Coordinates.h"
#include "RatDims.h"
#include "Rational.h"

class Placement;
class Rectangle;

class Placement {
 public:
  Placement();
  Placement(const Rectangle*, const Coordinates& c);
  virtual ~Placement();
  bool overlaps(const Placement& p) const;
  bool inside(const RatDims& b) const;
  Placement& operator*=(const URational& nScale);
  Placement& operator/=(const URational& nScale);
  void rotate();

  const URational& x1() const;
  URational x2() const;
  const URational& y1() const;
  URational y2() const;
  
  UInt ix1() const;
  UInt ix2() const;
  UInt iy1() const;
  UInt iy2() const;

  void print() const;

  RatDims m_Dims;
  Coordinates m_nLocation;
};

#endif // PLACEMENT_H
