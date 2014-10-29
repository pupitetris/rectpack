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

#ifndef DIMSFUNCTOR_H
#define DIMSFUNCTOR_H

#include "Integer.h"
#include "Rational.h"

#define NUM_DIMS 3
#define NUM_ROTATIONS 6 // Number of possible orthogonal orientations for N dims is N!

#define ROT_W        WidthLengthHeight // Rotates on X axis (swaps L & H)
#define ROT_H        LengthHeightWidth // Rotates on Y axis (swaps W & L)
#define ROT_L        HeightWidthLength // Rotates on Z axis (swaps H & W)
#define ROT_IDENTITY WidthHeightLenght // Rotation that does not modify the original
#define ROT_SHIFT    HeightLengthWidth // Rotation that shifts all dims to the left
#define ROT_UNSHIFT  LengthWidthHeight // Rotation that shifts all dims to the right

class DimsFunctor;
class RDimensions;
class Dimensions;
class Rectangle;

class DimsFunctor {
 public:
  DimsFunctor();
  virtual ~DimsFunctor();
  static DimsFunctor* get();

  virtual const UInt& d1(const Rectangle* r) const = 0;
  virtual const UInt& d1(const Rectangle& r) const = 0;
  virtual const UInt& d2(const Rectangle* r) const = 0;
  virtual const UInt& d2(const Rectangle& r) const = 0;
  virtual const UInt& d3(const Rectangle* r) const = 0;
  virtual const UInt& d3(const Rectangle& r) const = 0;

  virtual const UInt& d1(const Dimensions* r) const = 0;
  virtual const UInt& d1(const Dimensions& r) const = 0;
  virtual const UInt& d2(const Dimensions* r) const = 0;
  virtual const UInt& d2(const Dimensions& r) const = 0;
  virtual const UInt& d3(const Dimensions* r) const = 0;
  virtual const UInt& d3(const Dimensions& r) const = 0;

  virtual const URational& d1(const RDimensions* r) const = 0;
  virtual const URational& d1(const RDimensions& r) const = 0;
  virtual const URational& d2(const RDimensions* r) const = 0;
  virtual const URational& d2(const RDimensions& r) const = 0;
  virtual const URational& d3(const RDimensions* r) const = 0;
  virtual const URational& d3(const RDimensions& r) const = 0;
  
  // Rotate is the next possible transformation (wraps around).
  virtual DimsFunctor* rotate() const = 0;

  // Rotator is the functor that transforms dimensions back to the identity.
  virtual DimsFunctor* rotator() const = 0;

  // The functor that delivers dimensions shifted to the left relative to this one.
  virtual DimsFunctor* shift() const = 0;

  virtual bool isRotated();

  virtual DimsFunctor* clone() const = 0;
};

#endif // DIMSFUNCTOR_H
