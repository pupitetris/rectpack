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

#include "IntPlacement.h"
#include "Rectangle.h"
#include <iomanip>

IntPlacement::IntPlacement() :
  m_nX(0),
  m_nY(0),
  m_pRect(NULL) {
}

IntPlacement::IntPlacement(const Rectangle* pRect, UInt nX, UInt nY) :
  m_nX(nX),
  m_nY(nY),
  m_pRect(pRect) {
}

IntPlacement::~IntPlacement() {
}

bool IntPlacement::overlaps(const IntPlacement& i) const {
  return(overlapsX(i) && overlapsY(i));
}

bool IntPlacement::overlapsX(const IntPlacement& i) const {
  return(x1() < i.x2() && x2() > i.x1());
}

bool IntPlacement::overlapsY(const IntPlacement& i) const {
  return(y1() < i.y2() && y2() > i.y2());
}

UInt IntPlacement::x1() const {
  return(m_nX);
}

UInt IntPlacement::x2() const {
  return(x1() + m_pRect->m_nWidth);
}

UInt IntPlacement::y1() const {
  return(m_nY);
}

UInt IntPlacement::y2() const {
  return(y1() + m_pRect->m_nHeight);
}

bool IntPlacement::overlapsX(UInt x) const {
  return(x1() <= x && x < x2());
}

bool IntPlacement::overlapsY(UInt y) const {
  return(y1() <= y && y < y2());
}

void IntPlacement::print() const {
  std::cout << std::setw(2) << m_pRect->m_nID << ": "
	    << "(" << m_pRect->m_nWidth << "x" << m_pRect->m_nHeight << ") @ "
	    << "(" << m_nX << "," << m_nY << ")" << std::endl;
}
