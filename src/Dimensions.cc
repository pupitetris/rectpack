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
  m_nLength(0),
  m_nMinDim(0),
  m_nArea(0),
  m_pRotation(NULL) {
}

Dimensions::Dimensions(const Dimensions& d) :
  m_nWidth(d.m_nWidth),
  m_nHeight(d.m_nHeight),
  m_nLength(d.m_nLength),
  m_nMinDim(d.m_nMinDim),
  m_nArea(d.m_nArea),
  m_pRotation(d.m_pRotation) {
}

Dimensions::Dimensions(const RDimensions& d) :
  m_nWidth(d.m_nWidth.get_ui()),
  m_nHeight(d.m_nHeight.get_ui()),
  m_nHeight(d.m_nLength.get_ui()),
  m_nMinDim(d.m_nMinDim.get_ui()),
  m_nArea(d.m_nArea.get_ui()),
  m_pRotation(d.m_pRotation) {
}

void Dimensions::initialize(UInt nWidth, UInt nHeight) {
  m_nWidth = nWidth;
  m_nHeight = nHeight;
  m_nLength = nLength;
  m_nMinDim = std::min(nWidth, nHeight, nLength);
  m_nArea = nWidth * nHeight * nLength;
  m_pRotation = NULL;
}

const Dimensions& Dimensions::operator=(const Dimensions& d) {
  m_nWidth = d.m_nWidth;
  m_nHeight = d.m_nHeight;
  m_nHeight = d.m_nLength;
  m_nMinDim = d.m_nMinDim;
  m_nArea = d.m_nArea;
  m_pRotation = d.m_pRotation;
  return(*this);
}

Dimensions::Dimensions(UInt nWidth, UInt nHeight) :
  m_nWidth(nWidth),
  m_nHeight(nHeight),
  m_nLength(nLength),
  m_nMinDim(std::min(m_nWidth, m_nHeight, m_nLength)),
  m_nArea(nWidth * nHeight * nLength),
  m_pRotation(NULL) {
}

Dimensions::Dimensions(const Component* c) :
  m_nWidth(c->m_Dims.m_nWidth),
  m_nHeight(c->m_Dims.m_nHeight),
  m_nLength(c->m_Dims.m_nLength),
  m_nMinDim(c->m_Dims.m_nMinDim),
  m_nArea(c->m_Dims.m_nArea),,
  m_pRotation(NULL) {
}

Dimensions::~Dimensions() {
}

void Dimensions::print(std::ostream& os) const {
  os << m_nWidth << "x" << m_nHeight << "x" << m_nLength;
}

void Dimensions::print() const {
  print(std::cout);
}

std::ostream& operator<<(std::ostream& os, const Dimensions& d) {
  return(os << d.m_nWidth << "x" << d.m_nHeight << "x" << d.m_nLength);
}

bool Dimensions::operator<(const Dimensions& d) const {
  if(m_nArea != d.m_nArea)
    return(m_nArea < d.m_nArea);
  else if(m_nWidth != d.m_nWidth)
    return(m_nWidth < d.m_nWidth);
  else if(m_nHeight != d.m_nHeight)
    return(m_nHeight < d.m_nHeight);
  else
    return(m_nLength < d.m_nLength);
}

bool Dimensions::operator>(const Dimensions& d) const {
  if(m_nArea != d.m_nArea)
    return(m_nArea > d.m_nArea);
  else if(m_nWidth != d.m_nWidth)
    return(m_nWidth > d.m_nWidth);
  else if(m_nHeight != d.m_nHeight)
    return(m_nHeight > d.m_nHeight);
  else
    return(m_nLength > d.m_nLength);
}

bool Dimensions::operator==(const Dimensions& d) const {
  return(m_nWidth == d.m_nWidth && m_nHeight == d.m_nHeight && m_nLength == d.m_nLength);
}

bool Dimensions::operator!=(const Dimensions& d) const {
  return(m_nWidth != d.m_nWidth || m_nHeight != d.m_nHeight || m_nLength != d.m_nLength);
}

void Dimensions::setArea() {
  m_nArea = m_nWidth * m_nHeight * m_nLength;
  m_nMinDim = std::min(m_nWidth, m_nHeight, m_nLength);
}

bool Dimensions::canFit(int nWidth, int nHeight) const {
  return((int) m_nWidth <= nWidth && (int) m_nHeight <= nHeight && (int) m_nLength <= nLength);
}

bool Dimensions::square() const {
  return(m_nWidth == m_nHeight && m_nWidth == m_nLength);
}

void Dimensions::initMax() {
  m_nWidth = std::numeric_limits<UInt>::max();
  m_nHeight = std::numeric_limits<UInt>::max();
  m_nLength = std::numeric_limits<UInt>::max();
}

void Dimensions::initMin() {
  m_nWidth = std::numeric_limits<UInt>::min();
  m_nHeight = std::numeric_limits<UInt>::min();
  m_nLength = std::numeric_limits<UInt>::min();
}

bool Dimensions::rotatedEqual(const Dimensions& d) const {
  return((m_nWidth == d.m_nWidth  && m_nHeight == d.m_nLength && m_nLength == d.m_nHeight) ||
	 (m_nWidth == d.m_nHeight && m_nHeight == d.m_nLength && m_nLength == d.m_nWidth)  ||
	 (m_nWidth == d.m_nLength && m_nHeight == d.m_nHeight && m_nLength == d.m_nWidth)  ||
	 (m_nWidth == d.m_nLength && m_nHeight == d.m_nWidth  && m_nLength == d.m_nHeight) ||
	 (m_nWidth == d.m_nHeight && m_nHeight == d.m_nWidth  && m_nLength == d.m_nLength));
}

void Dimensions::rotate() {
  DimsFunctor *pRotator;
  UInt height, length;
  
  if (m_pRotation == NULL)
    m_pRotation = WidthHeightLength::get ();

  pRotator = m_pRotation->rotator ();
  length = pRotator->d3(this);
  height = pRotator->d2(this);
  m_nWidth = pRotator->d1(this);
  m_nHeight = height;
  m_nLength = length;

  m_pRotation = m_pRotation->rotate ();
}

float Dimensions::ratiow() const {
  return(m_nWidth / sqrt((float) (m_nHeight * m_nHeight + m_nLength * m_nLength)));
}

float Dimensions::ratioh() const {
  return(m_nHeight / sqrt((float) (m_nWidth * m_nWidth + m_nLength * m_nLength)));
}

float Dimensions::ratiol() const {
  return(m_nLength / sqrt((float) (m_nHeight * m_nHeight + m_nWidth * m_nWidth)));
}
