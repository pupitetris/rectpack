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

#ifndef BOXDIMENSIONS_H
#define BOXDIMENSIONS_H

#include "Integer.h"
#include "Rational.h"
#include <iostream>
#include <string>

class BoxDimensions;
class Rectangle;

class BoxDimensions {
  friend std::ostream& operator<<(std::ostream& os, const BoxDimensions& b);

 public:
  BoxDimensions();
  ~BoxDimensions();
  void initialize(const UInt& n);
  void initialize(const UInt& w, const UInt& h);
  void initializeH(const UInt& h);
  bool operator<(const BoxDimensions& rhs) const;
  bool operator>(const BoxDimensions& rhs) const;
  bool operator==(const BoxDimensions& rhs) const;
  bool operator!=(const BoxDimensions& rhs) const;
  bool canFit(const Rectangle* r) const;
  void rotate();
  URational ratio() const;
  bool square() const;
  std::string toString() const;
  const BoxDimensions& operator*=(const UInt& ur);
  const BoxDimensions& operator/=(const UInt& ur);
  const BoxDimensions& operator*=(const URational& ur);
  const BoxDimensions& operator/=(const URational& ur);

  UInt m_nWidth;
  UInt m_nHeight;
  UInt m_nArea;

  void print() const;
};

std::ostream& operator<<(std::ostream& os, const BoxDimensions& b);

#endif // BOXDIMENSIONS_H
