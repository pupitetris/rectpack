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

#ifndef ICOORDS_H
#define ICOORDS_H

#include "Integer.h"
#include <ostream>

class ICoords;

class ICoords {

  friend std::ostream& operator<<(std::ostream& os,
				  const ICoords& c);
 public:
  inline ICoords() : x(0), y(0) { };
  inline ICoords(Int m, Int n) : x(m), y(n) { };
  inline ~ICoords() { };
  inline void set(Int m, Int n) {
    x = m; y = n;
  };

  bool operator<(const ICoords& rhs) const;
  bool operator>(const ICoords& rhs) const;

  Int x;
  Int y;
};

std::ostream& operator<<(std::ostream& os, const ICoords& c);

#endif // ICOORDS_H
