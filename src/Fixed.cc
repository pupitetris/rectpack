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

#include "Fixed.h"
#include <ostream>

Fixed::Fixed() {
}

Fixed::~Fixed() {
}

Fixed::Fixed(size_t w, size_t h, size_t x, size_t y) :
  m_nWidth(w),
  m_nHeight(h),
  m_nX(x),
  m_nY(y) {
}

std::ostream& operator<<(std::ostream& os, const Fixed& rhs) {
  return(os << rhs.m_nWidth << "x" << rhs.m_nHeight << "=(" << rhs.m_nX
	 << "," << rhs.m_nY << ")");
}
