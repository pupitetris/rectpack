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
#include "LengthHeightWidth.h"
#include "LengthWidthHeight.h"
#include "RDimensions.h"
#include "Rectangle.h"

LengthWidthHeight* LengthWidthHeight::singleton = NULL;

LengthWidthHeight::LengthWidthHeight() {
}

LengthWidthHeight::~LengthWidthHeight() {
}

DimsFunctor* LengthWidthHeight::get() {
  if (singleton == NULL)
    singleton = new LengthWidthHeight ();
  return singleton;
}

const UInt& LengthWidthHeight::d1(const Rectangle* r) const {
  return(r->m_nLength);
}

const UInt& LengthWidthHeight::d1(const Rectangle& r) const {
  return(r.m_nLength);
}

const UInt& LengthWidthHeight::d2(const Rectangle* r) const {
  return(r->m_nWidth);
}

const UInt& LengthWidthHeight::d2(const Rectangle& r) const {
  return(r.m_nWidth);
}

const UInt& LengthWidthHeight::d3(const Rectangle* r) const {
  return(r->m_nHeight);
}

const UInt& LengthWidthHeight::d3(const Rectangle& r) const {
  return(r.m_nHeight);
}

const UInt& LengthWidthHeight::d1(const Dimensions* r) const {
  return(r->m_nLength);
}

const UInt& LengthWidthHeight::d1(const Dimensions& r) const {
  return(r.m_nLength);
}

const UInt& LengthWidthHeight::d2(const Dimensions* r) const {
  return(r->m_nWidth);
}

const UInt& LengthWidthHeight::d2(const Dimensions& r) const {
  return(r.m_nWidth);
}

const UInt& LengthWidthHeight::d3(const Dimensions* r) const {
  return(r->m_nHeight);
}

const UInt& LengthWidthHeight::d3(const Dimensions& r) const {
  return(r.m_nHeight);
}

const URational& LengthWidthHeight::d1(const RDimensions* r) const {
  return(r->m_nLength);
}

const URational& LengthWidthHeight::d1(const RDimensions& r) const {
  return(r.m_nLength);
}

const URational& LengthWidthHeight::d2(const RDimensions* r) const {
  return(r->m_nWidth);
}

const URational& LengthWidthHeight::d2(const RDimensions& r) const {
  return(r.m_nWidth);
}

const URational& LengthWidthHeight::d3(const RDimensions* r) const {
  return(r->m_nHeight);
}

const URational& LengthWidthHeight::d3(const RDimensions& r) const {
  return(r.m_nHeight);
}

DimsFunctor* LengthWidthHeight::rotate() const {
  return(HeightWidthLength::get());
}

DimsFunctor* LengthWidthHeight::rotator() const {
  return(HeightLengthWidth::get());
}

DimsFunctor* LengthWidthHeight::shift() const {
  return(WidthHeightLenght::get ());
}

DimsFunctor* LengthWidthHeight::clone() const {
  return(new LengthWidthHeight());
}
