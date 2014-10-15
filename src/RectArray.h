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

#ifndef RECTARRAY_H
#define RECTARRAY_H

#include "Rational.h"
#include "Rectangle.h"
#include <vector>
#include <string>

class DimsFunctor;
class HeapBox;
class Instance;
class Parameters;

class RectArray : public std::vector<Rectangle> {
 public:
  RectArray();
  ~RectArray();
  void initialize(const Parameters* pParams);
  void loady(const std::vector<int>& v);
  void savey(std::vector<int>& v) const;
  void str2vec(const std::string& s, std::vector<int>& v) const;
  void vec2str(const std::vector<int>& v, std::string& s) const;
  void color();
  void reset(const Instance& i);
  void constrain(const HeapBox& b);
  void constrain(const BoxDimensions& b);
  void constrainHeight(size_t nHeight);
  void inferProperties();
  void rotate();
  void swap();
  const RectArray& operator*=(const URational& ur);

  /**
   * Determines whether or not there exists at least one of any of the
   * following properties.
   */

  bool m_bNonRotatables;
  bool m_bRotatables;
  bool m_bUnorientedNonSquares;
  bool m_bOrientedNonSquares;

  void print() const;
};

#endif // RECTARRAY_H
