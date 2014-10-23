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

#include "WidthLengthHeight.h"
#include "WidthHeightLength.h"
#include "RDimensions.h"
#include "Rectangle.h"

WidthHeightLength* WidthHeightLength::singleton = NULL;

WidthHeightLength::WidthHeightLength() {
}

WidthHeightLength::~WidthHeightLength() {
}

DimsFunctor* WidthHeightLength::get() {
  if (singleton == NULL)
    singleton = new WidthHeightLength ();
  return singleton;
}

const UInt& WidthHeightLength::d1(const Rectangle* r) const {
  return(r->m_nWidth);
}

const UInt& WidthHeightLength::d1(const Rectangle& r) const {
  return(r.m_nWidth);
}

const UInt& WidthHeightLength::d2(const Rectangle* r) const {
  return(r->m_nHeight);
}

const UInt& WidthHeightLength::d2(const Rectangle& r) const {
  return(r.m_nHeight);
}

const UInt& WidthHeightLength::d3(const Rectangle* r) const {
  return(r->m_nLength);
}

const UInt& WidthHeightLength::d3(const Rectangle& r) const {
  return(r.m_nLength);
}

const URational& WidthHeightLength::d1(const RDimensions* r) const {
  return(r->m_nWidth);
}

const URational& WidthHeightLength::d1(const RDimensions& r) const {
  return(r.m_nWidth);
}

const URational& WidthHeightLength::d2(const RDimensions* r) const {
  return(r->m_nHeight);
}

const URational& WidthHeightLength::d2(const RDimensions& r) const {
  return(r.m_nHeight);
}

const URational& WidthHeightLength::d3(const RDimensions* r) const {
  return(r->m_nLength);
}

const URational& WidthHeightLength::d3(const RDimensions& r) const {
  return(r.m_nLength);
}

DimsFunctor* WidthHeightLength::rotate() const {
  return(WidthLengthHeight::get ());
}

DimsFunctor* WidthHeightLength::rotator() const {
  return(ROT_IDENTITY::get ());
}

DimsFunctor* WidthHeightLength::shift() const {
  return(HeightLengthWidth::get ());
}

bool WidthHeightLength::isRotated() {
  return(false);
}

DimsFunctor* WidthHeightLength::clone() const {
  return(new WidthHeightLength());
}
