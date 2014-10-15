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

#include "Coordinates.h"
#include <iostream>

Coordinates::Coordinates() :
  x((UInt) 0),
  y((UInt) 0) {
}

Coordinates::Coordinates(UInt m, UInt n) :
  x(m),
  y(n) {
}

Coordinates::Coordinates(const URational& m, const URational& n) :
  x(m),
  y(n) {
}

Coordinates::~Coordinates() {
}

bool Coordinates::operator<(const Coordinates& rhs) const {
  return((x != rhs.x) ? (x < rhs.x) : (y < rhs.y));
}

bool Coordinates::operator>(const Coordinates& rhs) const {
  return((x != rhs.x) ? (x > rhs.x) : (y > rhs.y));
}

std::ostream& operator<<(std::ostream& os, const Coordinates& c) {
  return(os << '(' << c.x << ',' << c.y << ')');
}

void Coordinates::initialize(const URational& m, const URational& n) {
  x = m; y = n;
};

Coordinates& Coordinates::operator*=(const URational& nScale) {
  x *= nScale;
  y *= nScale;
  return(*this);
}

Coordinates& Coordinates::operator/=(const URational& nScale) {
  x /= nScale;
  y /= nScale;
  return(*this);
}

void Coordinates::print() const {
  std::cout << (*this) << std::flush;
}

void Coordinates::swap() {
  std::swap(x, y);
}
