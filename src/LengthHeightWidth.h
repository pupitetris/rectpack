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

#ifndef LENGTHHEIGHTWIDTH_H
#define LENGTHHEIGHTWIDTH_H

#include "DimsFunctor.h"
#include "Integer.h"
#include "Rational.h"

class RDimensions;
class Rectangle;

class LengthHeightWidth : public DimsFunctor {
 public:
  LengthHeightWidth();
  virtual ~LengthHeightWidth();

  virtual const UInt& d1(const Rectangle* r) const;
  virtual const UInt& d1(const Rectangle& r) const;
  virtual const UInt& d2(const Rectangle* r) const;
  virtual const UInt& d2(const Rectangle& r) const;
  virtual const UInt& d3(const Rectangle* r) const;
  virtual const UInt& d3(const Rectangle& r) const;

  virtual const URational& d1(const RDimensions* r) const;
  virtual const URational& d1(const RDimensions& r) const;
  virtual const URational& d2(const RDimensions* r) const;
  virtual const URational& d2(const RDimensions& r) const;
  virtual const URational& d3(const RDimensions* r) const;
  virtual const URational& d3(const RDimensions& r) const;

  virtual DimsFunctor* rotate() const;
  virtual DimsFunctor* rotator() const;
  virtual DimsFunctor* clone() const;
};

#endif // LENGTHHEIGHTWIDTH_H
