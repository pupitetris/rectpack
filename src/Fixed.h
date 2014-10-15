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

#ifndef FIXED_H
#define FIXED_H

#include <ostream>

class Fixed;

class Fixed {
  friend std::ostream& operator<<(std::ostream& os, const Fixed& rhs);

 public:
  Fixed();
  Fixed(size_t w, size_t h, size_t x, size_t y);
  ~Fixed();
  size_t m_nWidth;
  size_t m_nHeight;
  size_t m_nX;
  size_t m_nY;
};

std::ostream& operator<<(std::ostream& os, const Fixed& rhs);

#endif // FIXED_H
