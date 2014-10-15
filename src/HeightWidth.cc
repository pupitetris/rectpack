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

#include "HeightWidth.h"
#include "RDimensions.h"
#include "Rectangle.h"
#include "WidthHeight.h"

HeightWidth::HeightWidth() {
}

HeightWidth::~HeightWidth() {
}

const UInt& HeightWidth::d1(const Rectangle* r) const {
  return(r->m_nHeight);
}

const UInt& HeightWidth::d1(const Rectangle& r) const {
  return(r.m_nHeight);
}

const UInt& HeightWidth::d2(const Rectangle* r) const {
  return(r->m_nWidth);
}

const UInt& HeightWidth::d2(const Rectangle& r) const {
  return(r.m_nWidth);
}

const URational& HeightWidth::d1(const RDimensions* r) const {
  return(r->m_nHeight);
}

const URational& HeightWidth::d1(const RDimensions& r) const {
  return(r.m_nHeight);
}

const URational& HeightWidth::d2(const RDimensions* r) const {
  return(r->m_nWidth);
}

const URational& HeightWidth::d2(const RDimensions& r) const {
  return(r.m_nWidth);
}

DimsFunctor* HeightWidth::reverse() const {
  return(new WidthHeight());
}

DimsFunctor* HeightWidth::clone() const {
  return(new HeightWidth());
}
