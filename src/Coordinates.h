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

#ifndef COORDINATES_H
#define COORDINATES_H

#include "Integer.h"
#include "Rational.h"
#include <ostream>

class Coordinates;

class Coordinates {

  friend std::ostream& operator<<(std::ostream& os,
				  const Coordinates& c);
 public:
  Coordinates();
  Coordinates(UInt m, UInt n);
  Coordinates(const URational& m, const URational& n);
  ~Coordinates();
  Coordinates& operator*=(const URational& nScale);
  Coordinates& operator/=(const URational& nScale);
  void initialize(const URational& m, const URational& n);
  bool operator<(const Coordinates& rhs) const;
  bool operator>(const Coordinates& rhs) const;
  void print() const;
  void swap();

  URational x;
  URational y;
};

std::ostream& operator<<(std::ostream& os, const Coordinates& c);

#endif // COORDINATES_H
