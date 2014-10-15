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

#include "Dimensions.h"
#include "Component.h"
#include "RDimensions.h"
#include <limits>

Dimensions::Dimensions() :
  m_nWidth(0),
  m_nHeight(0),
  m_nMinDim(0),
  m_nArea(0) {
}

Dimensions::Dimensions(const Dimensions& d) :
  m_nWidth(d.m_nWidth),
  m_nHeight(d.m_nHeight),
  m_nMinDim(d.m_nMinDim),
  m_nArea(d.m_nArea) {
}

Dimensions::Dimensions(const RDimensions& d) :
  m_nWidth(d.m_nWidth.get_ui()),
  m_nHeight(d.m_nHeight.get_ui()),
  m_nMinDim(d.m_nMinDim.get_ui()),
  m_nArea(d.m_nArea.get_ui()) {
}

void Dimensions::initialize(UInt nWidth, UInt nHeight) {
  m_nWidth = nWidth;
  m_nHeight = nHeight;
  m_nMinDim = std::min(nWidth, nHeight);
  m_nArea = nWidth * nHeight;
}

const Dimensions& Dimensions::operator=(const Dimensions& d) {
  m_nWidth = d.m_nWidth;
  m_nHeight = d.m_nHeight;
  m_nMinDim = d.m_nMinDim;
  m_nArea = d.m_nArea;
  return(*this);
}

Dimensions::Dimensions(UInt nWidth, UInt nHeight) :
  m_nWidth(nWidth),
  m_nHeight(nHeight),
  m_nMinDim(std::min(m_nWidth, m_nHeight)),
  m_nArea(nWidth * nHeight) {
}

Dimensions::Dimensions(const Component* c) :
  m_nWidth(c->m_Dims.m_nWidth),
  m_nHeight(c->m_Dims.m_nHeight),
  m_nMinDim(c->m_Dims.m_nMinDim),
  m_nArea(c->m_Dims.m_nArea) {
}

Dimensions::~Dimensions() {
}

void Dimensions::print(std::ostream& os) const {
  os << m_nWidth << "x" << m_nHeight;
}

void Dimensions::print() const {
  print(std::cout);
}

std::ostream& operator<<(std::ostream& os, const Dimensions& d) {
  return(os << d.m_nWidth << "x" << d.m_nHeight);
}

bool Dimensions::operator<(const Dimensions& d) const {
  if(m_nArea != d.m_nArea)
    return(m_nArea < d.m_nArea);
  else if(m_nWidth != d.m_nWidth)
    return(m_nWidth < d.m_nWidth);
  else
    return(m_nHeight < d.m_nHeight);
}

bool Dimensions::operator>(const Dimensions& d) const {
  if(m_nArea != d.m_nArea)
    return(m_nArea > d.m_nArea);
  else if(m_nWidth != d.m_nWidth)
    return(m_nWidth > d.m_nWidth);
  else
    return(m_nHeight > d.m_nHeight);
}

bool Dimensions::operator==(const Dimensions& d) const {
  return(m_nWidth == d.m_nWidth && m_nHeight == d.m_nHeight);
}

bool Dimensions::operator!=(const Dimensions& d) const {
  return(m_nWidth != d.m_nWidth || m_nHeight != d.m_nHeight);
}

void Dimensions::setArea() {
  m_nArea = m_nWidth * m_nHeight;
  m_nMinDim = std::min(m_nWidth, m_nHeight);
}

bool Dimensions::canFit(int nWidth, int nHeight) const {
  return((int) m_nWidth <= nWidth && (int) m_nHeight <= nHeight);
}

bool Dimensions::square() const {
  return(m_nWidth == m_nHeight);
}

void Dimensions::initMax() {
  m_nWidth = std::numeric_limits<UInt>::max();
  m_nHeight = std::numeric_limits<UInt>::max();
}

void Dimensions::initMin() {
  m_nWidth = std::numeric_limits<UInt>::min();
  m_nHeight = std::numeric_limits<UInt>::min();
}

bool Dimensions::rotatedEqual(const Dimensions& d) const {
  return(m_nHeight == d.m_nWidth && m_nWidth == d.m_nHeight);
}

void Dimensions::rotate() {
  std::swap(m_nWidth, m_nHeight);
}

float Dimensions::ratiowh() const {
  return(m_nWidth / (float) m_nHeight);
}

float Dimensions::ratiohw() const {
  return(m_nHeight / (float) m_nWidth);
}
