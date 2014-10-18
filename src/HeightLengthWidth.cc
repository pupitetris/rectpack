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

#include "LengthHeightWidth.h"
#include "HeightLengthWidth.h"
#include "RDimensions.h"
#include "Rectangle.h"

HeightLengthWidth::HeightLengthWidth() {
}

HeightLengthWidth::~HeightLengthWidth() {
}

const UInt& HeightLengthWidth::d1(const Rectangle* r) const {
  return(r->m_nHeight);
}

const UInt& HeightLengthWidth::d1(const Rectangle& r) const {
  return(r.m_nHeight);
}

const UInt& HeightLengthWidth::d2(const Rectangle* r) const {
  return(r->m_nLength);
}

const UInt& HeightLengthWidth::d2(const Rectangle& r) const {
  return(r.m_nLength);
}

const UInt& HeightLengthWidth::d3(const Rectangle* r) const {
  return(r->m_nWidth);
}

const UInt& HeightLengthWidth::d3(const Rectangle& r) const {
  return(r.m_nWidth);
}

const URational& HeightLengthWidth::d1(const RDimensions* r) const {
  return(r->m_nHeight);
}

const URational& HeightLengthWidth::d1(const RDimensions& r) const {
  return(r.m_nHeight);
}

const URational& HeightLengthWidth::d2(const RDimensions* r) const {
  return(r->m_nLength);
}

const URational& HeightLengthWidth::d2(const RDimensions& r) const {
  return(r.m_nLength);
}

const URational& HeightLengthWidth::d3(const RDimensions* r) const {
  return(r->m_nWidth);
}

const URational& HeightLengthWidth::d3(const RDimensions& r) const {
  return(r.m_nWidth);
}

DimsFunctor* HeightLengthWidth::rotate() const {
  return(new LengthHeightWidth());
}

DimsFunctor* HeightLengthWidth::clone() const {
  return(new HeightLengthWidth());
}
