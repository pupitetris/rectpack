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

#ifndef LABEL_H
#define LABEL_H

#include "Rational.h"
#include <string>
#include <vector>

class RatDims;
class RDimensions;
class Rectangle;

class Label : public std::vector<std::string> {
 public:
  Label();
  virtual ~Label();
  void initialize(const RDimensions* pActual,
		  const RatDims* pScaled);
  bool empty() const;
  size_t width() const;
  size_t height() const;
  size_t yOffset() const;

 private:
  void center(const RatDims* pScaled);
  size_t m_nYOffset;
};

#endif // LABEL_H
