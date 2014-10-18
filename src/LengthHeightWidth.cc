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

#include "WithHeightLength.h"
#include "LengthHeightWidth.h"
#include "RDimensions.h"
#include "Rectangle.h"

LengthHeightWidth::LengthHeightWidth() {
}

LengthHeightWidth::~LengthHeightWidth() {
}

const UInt& LengthHeightWidth::d1(const Rectangle* r) const {
  return(r->m_nLength);
}

const UInt& LengthHeightWidth::d1(const Rectangle& r) const {
  return(r.m_nLength);
}

const UInt& LengthHeightWidth::d2(const Rectangle* r) const {
  return(r->m_nHeight);
}

const UInt& LengthHeightWidth::d2(const Rectangle& r) const {
  return(r.m_nHeight);
}

const UInt& LengthHeightWidth::d3(const Rectangle* r) const {
  return(r->m_nWidth);
}

const UInt& LengthHeightWidth::d3(const Rectangle& r) const {
  return(r.m_nWidth);
}

const URational& LengthHeightWidth::d1(const RDimensions* r) const {
  return(r->m_nLength);
}

const URational& LengthHeightWidth::d1(const RDimensions& r) const {
  return(r.m_nLength);
}

const URational& LengthHeightWidth::d2(const RDimensions* r) const {
  return(r->m_nHeight);
}

const URational& LengthHeightWidth::d2(const RDimensions& r) const {
  return(r.m_nHeight);
}

const URational& LengthHeightWidth::d3(const RDimensions* r) const {
  return(r->m_nWidth);
}

const URational& LengthHeightWidth::d3(const RDimensions& r) const {
  return(r.m_nWidth);
}

DimsFunctor* LengthHeightWidth::rotate() const {
  return(new WidthHeightLength());
}

DimsFunctor* LengthHeightWidth::clone() const {
  return(new LengthHeightWidth());
}
