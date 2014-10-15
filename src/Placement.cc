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

#include "Placement.h"
#include "Rectangle.h"

Placement::Placement() {
}

Placement::Placement(const Rectangle* r, const Coordinates& c) :
  m_Dims(r->m_nWidth, r->m_nHeight),
  m_nLocation(c) {
}

Placement::~Placement() {
}

bool Placement::overlaps(const Placement& p) const {
  return(x1() < p.x2() && p.x1() < x2() &&
	 y1() < p.y2() && p.y1() < y2());
}

const URational& Placement::x1() const {
  return(m_nLocation.x);
}

URational Placement::x2() const {
  return(m_nLocation.x + m_Dims.m_nWidth);
}

const URational& Placement::y1() const {
  return(m_nLocation.y);
}

URational Placement::y2() const {
  return(m_nLocation.y + m_Dims.m_nHeight);
}

UInt Placement::ix1() const {
  return(x1().get_ui());
}

UInt Placement::ix2() const {
  return(x2().get_ui());
}

UInt Placement::iy1() const {
  return(y1().get_ui());
}

UInt Placement::iy2() const {
  return(y2().get_ui());
}

Placement& Placement::operator*=(const URational& nScale) {
  m_Dims *= nScale;
  m_nLocation *= nScale;
  return(*this);
}

Placement& Placement::operator/=(const URational& nScale) {
  m_Dims /= nScale;
  m_nLocation /= nScale;
  return(*this);
}

void Placement::print() const {
  std::cout << m_Dims << " " << m_nLocation << std::flush;
}

bool Placement::inside(const RatDims& b) const {
  return(y1() >= (URational) 0 && y2() <= b.m_nHeight &&
	 x1() >= (URational) 0 && x2() <= b.m_nWidth);
}

void Placement::rotate() {
  m_Dims.rotate();
  m_nLocation.swap();
}
