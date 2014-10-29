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

#include "HeightWidthLength.h"
#include "WidthHeightLength.h"
#include "WidthLengthHeight.h"
#include "RDimensions.h"
#include "Dimensions.h"
#include "Rectangle.h"

HeightWidthLength* HeightWidthLength::singleton = NULL;

HeightWidthLength::HeightWidthLength() {
}

HeightWidthLength::~HeightWidthLength() {
}

DimsFunctor* HeightWidthLength::get() {
  if (singleton == NULL)
    singleton = new HeightWidthLength ();
  return singleton;
}

const UInt& HeightWidthLength::d1(const Rectangle* r) const {
  return(r->m_nHeight);
}

const UInt& HeightWidthLength::d1(const Rectangle& r) const {
  return(r.m_nHeight);
}

const UInt& HeightWidthLength::d2(const Rectangle* r) const {
  return(r->m_nWidth);
}

const UInt& HeightWidthLength::d2(const Rectangle& r) const {
  return(r.m_nWidth);
}

const UInt& HeightWidthLength::d3(const Rectangle* r) const {
  return(r->m_nLength);
}

const UInt& HeightWidthLength::d3(const Rectangle& r) const {
  return(r.m_nLength);
}

const UInt& HeightWidthLength::d1(const Dimensions* r) const {
  return(r->m_nHeight);
}

const UInt& HeightWidthLength::d1(const Dimensions& r) const {
  return(r.m_nHeight);
}

const UInt& HeightWidthLength::d2(const Dimensions* r) const {
  return(r->m_nWidth);
}

const UInt& HeightWidthLength::d2(const Dimensions& r) const {
  return(r.m_nWidth);
}

const UInt& HeightWidthLength::d3(const Dimensions* r) const {
  return(r->m_nLength);
}

const UInt& HeightWidthLength::d3(const Dimensions& r) const {
  return(r.m_nLength);
}

const URational& HeightWidthLength::d1(const RDimensions* r) const {
  return(r->m_nHeight);
}

const URational& HeightWidthLength::d1(const RDimensions& r) const {
  return(r.m_nHeight);
}

const URational& HeightWidthLength::d2(const RDimensions* r) const {
  return(r->m_nWidth);
}

const URational& HeightWidthLength::d2(const RDimensions& r) const {
  return(r.m_nWidth);
}

const URational& HeightWidthLength::d3(const RDimensions* r) const {
  return(r->m_nLength);
}

const URational& HeightWidthLength::d3(const RDimensions& r) const {
  return(r.m_nLength);
}

DimsFunctor* HeightWidthLength::rotate() const {
  return(WidthHeightLength::get());
}

DimsFunctor* HeightWidthLength::rotator() const {
  return(HeightWidthLength::get());
}

DimsFunctor* HeightWidthLength::shift() const {
  return(WidthLengthHeight::get ());
}

DimsFunctor* HeightWidthLength::clone() const {
  return(new HeightWidthLength());
}
