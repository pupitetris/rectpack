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

#include "HeightFirst.h"
#include "Rectangle.h"
#include "WidthFirst.h"

WidthFirst::WidthFirst() {
}

WidthFirst::WidthFirst(const Rectangle* pRect) :
  RectDecorator(pRect) {
}

WidthFirst::~WidthFirst() {
}

const UInt& WidthFirst::d1() const {
  return(m_pRect->m_nWidth);
}

const UInt& WidthFirst::d2() const {
  return(m_pRect->m_nHeight);
}

RectDecorator* WidthFirst::clone() const {
  return(new WidthFirst());
}

RectDecorator* WidthFirst::reverse() const {
  return(new HeightFirst());
}

bool WidthFirst::operator<(const WidthFirst& rhs) const {
  return(d1() < rhs.d1());
}
