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

#include "BoxDimensions.h"
#include "Coordinates.h"
#include "DimsFunctor.h"
#include "Fixed.h"
#include "Grid.h"
#include "Rational.h"
#include "RDimensions.h"
#include "Rectangle.h"
#include "WidthHeightLength.h"
#include <iomanip>
#include <iostream>
#include <sstream>

Rectangle::Rectangle() :
  m_bRotated(false),
  m_bRotatable(true),
  m_bSquare(false),
  m_pRotation(NULL),
  m_bFixed(false) {
}

Rectangle::~Rectangle() {
}

void Rectangle::initialize(const UInt& nSize) {
  initialize(nSize, nSize, nSize);
}

void Rectangle::initialize(const UInt& nWidth, const UInt& nHeight, const UInt& nLength) {
  m_nWidth = nWidth;
  m_nHeight = nHeight;
  m_nLength = nLength;
  m_nArea = m_nWidth * m_nHeight * m_nLength;
  m_nMinDim = std::min(nWidth, std::min(nHeight, nLength));
  m_nMaxDim = std::max(nWidth, std::max(nHeight, nLength));
  m_bSquare = (m_nWidth == m_nHeight && m_nWidth == m_nLength);
  if(m_bSquare) m_bRotatable = false;
  m_pRotation = WidthHeightLength::get(); // Initial rotation is a 1:1 mapping.
}

void Rectangle::initialize(const RDimensions& d) {
  if(d.m_nOrientation == RDimensions::ORIENTED)
    m_bRotatable = false;
  if(d.m_nWidth.integer() && d.m_nHeight.integer() && d.m_nLength.integer())
    initialize(d.m_nWidth.get_ui(), d.m_nHeight.get_ui(), d.m_nLength.get_ui());
  else
    std::cout << "Rectangle::size(...): Dimensions should be integers." << std::endl;
  m_bRotated = false;
}

void Rectangle::print() const {
  if (m_bRotated)
    std::cout << m_nWidth << "x" << m_nHeight << m_nLength << std::flush;
  else
    std::cout << m_pRotation->d1(this) <<  m_pRotation->d2(this) <<  m_pRotation->d3(this) << std::flush;
}

void Rectangle::printNoLocation() const {
  if (m_bRotated)
    std::cout << std::setw(2) << m_nID << ":" << std::setw(2)
	      << m_pRotation->d1(this) << "x" << std::setw(2) << m_pRotation->d2(this) << "x" << std::setw(2) << m_pRotation->d3(this);
  else
    std::cout << std::setw(2) << m_nID << ":" << std::setw(2)
	      << m_nWidth << "x" << std::setw(2) << m_nHeight << "x" << std::setw(2) << m_nLength;
}

int Rectangle::nextX() const {
  return(x + m_nWidth);
}

int Rectangle::nextY() const {
  return(y + m_nHeight);
}

int Rectangle::nextZ() const {
  return(z + m_nLength);
}

bool Rectangle::skippingX(const Grid* pGrid) {
  const Rectangle* r;
  if(pGrid->occupied(x, y, z)) {
    r = pGrid->rect(x, y, z);
    x = r->x + r->m_nWidth - 1;
    return(true);
  }
  else if(pGrid->occupied(x + m_nWidth - 1, y, z)) {
    r = pGrid->rect(x + m_nWidth - 1, y, z);
    x = r->x + r->m_nWidth - 1;
    return(true);
  }
  else if(pGrid->occupied(x, y + m_nHeight - 1, z)) {
    r = pGrid->rect(x, y + m_nHeight - 1, z);
    x = r->x + r->m_nWidth - 1;
    return(true);
  }
  else if(pGrid->occupied(x + m_nWidth - 1, y + m_nHeight - 1, z)) {
    r = pGrid->rect(x + m_nWidth - 1, y + m_nHeight - 1, z);
    x = r->x + r->m_nWidth - 1;
    return(true);
  }
  else if(pGrid->occupied(x, y, z + m_nLength - 1)) {
    r = pGrid->rect(x, y, z + m_nLength - 1);
    x = r->x + r->m_nWidth - 1;
    return(true);
  }
  else if(pGrid->occupied(x + m_nWidth - 1, y, z + m_nLength - 1)) {
    r = pGrid->rect(x + m_nWidth - 1, y, z + m_nLength - 1);
    x = r->x + r->m_nWidth - 1;
    return(true);
  }
  else if(pGrid->occupied(x, y + m_nHeight - 1, z + m_nLength - 1)) {
    r = pGrid->rect(x, y + m_nHeight - 1, z + m_nLength - 1);
    x = r->x + r->m_nWidth - 1;
    return(true);
  }
  else if(pGrid->occupied(x + m_nWidth - 1, y + m_nHeight - 1, z + m_nLength - 1)) {
    r = pGrid->rect(x + m_nWidth - 1, y + m_nHeight - 1, z + m_nLength - 1);
    x = r->x + r->m_nWidth - 1;
    return(true);
  }
  else
    return(false);
}

bool Rectangle::skippingY(const Grid* pGrid) {
  const Rectangle* r;
  if(pGrid->occupied(x, y, z)) {
    r = pGrid->rect(x, y, z);
    y = r->y + r->m_nHeight - 1;
    return(true);
  }
  else if(pGrid->occupied(x + m_nWidth - 1, y, z)) {
    r = pGrid->rect(x + m_nWidth - 1, y, z);
    y = r->y + r->m_nHeight - 1;
    return(true);
  }
  else if(pGrid->occupied(x, y + m_nHeight - 1, z)) {
    r = pGrid->rect(x, y + m_nHeight - 1, z);
    y = r->y + r->m_nHeight - 1;
    return(true);
  }
  else if(pGrid->occupied(x + m_nWidth - 1, y + m_nHeight - 1, z)) {
    r = pGrid->rect(x + m_nWidth - 1, y + m_nHeight - 1, z);
    y = r->y + r->m_nHeight - 1;
    return(true);
  }
  else if(pGrid->occupied(x, y, z + m_nLength - 1)) {
    r = pGrid->rect(x, y, z + m_nLength - 1);
    y = r->y + r->m_nHeight - 1;
    return(true);
  }
  else if(pGrid->occupied(x + m_nWidth - 1, y, z + m_nLength - 1)) {
    r = pGrid->rect(x + m_nWidth - 1, y, z + m_nLength - 1);
    y = r->y + r->m_nHeight - 1;
    return(true);
  }
  else if(pGrid->occupied(x, y + m_nHeight - 1, z + m_nLength - 1)) {
    r = pGrid->rect(x, y + m_nHeight - 1, z + m_nLength - 1);
    y = r->y + r->m_nHeight - 1;
    return(true);
  }
  else if(pGrid->occupied(x + m_nWidth - 1, y + m_nHeight - 1, z + m_nLength - 1)) {
    r = pGrid->rect(x + m_nWidth - 1, y + m_nHeight - 1, z + m_nLength - 1);
    y = r->y + r->m_nHeight - 1;
    return(true);
  }
  else
    return(false);
}

bool Rectangle::skippingZ(const Grid* pGrid) {
  const Rectangle* r;
  if(pGrid->occupied(x, y, z)) {
    r = pGrid->rect(x, y, z);
    z = r->z + r->m_nLength - 1;
    return(true);
  }
  else if(pGrid->occupied(x + m_nWidth - 1, y, z)) {
    r = pGrid->rect(x + m_nWidth - 1, y, z);
    z = r->z + r->m_nLength - 1;
    return(true);
  }
  else if(pGrid->occupied(x, y + m_nHeight - 1, z)) {
    r = pGrid->rect(x, y + m_nHeight - 1, z);
    z = r->z + r->m_nLength - 1;
    return(true);
  }
  else if(pGrid->occupied(x + m_nWidth - 1, y + m_nHeight - 1, z)) {
    r = pGrid->rect(x + m_nWidth - 1, y + m_nHeight - 1, z);
    z = r->z + r->m_nLength - 1;
    return(true);
  }
  else if(pGrid->occupied(x, y, z + m_nLength - 1)) {
    r = pGrid->rect(x, y, z + m_nLength - 1);
    z = r->z + r->m_nLength - 1;
    return(true);
  }
  else if(pGrid->occupied(x + m_nWidth - 1, y, z + m_nLength - 1)) {
    r = pGrid->rect(x + m_nWidth - 1, y, z + m_nLength - 1);
    z = r->z + r->m_nLength - 1;
    return(true);
  }
  else if(pGrid->occupied(x, y + m_nHeight - 1, z + m_nLength - 1)) {
    r = pGrid->rect(x, y + m_nHeight - 1, z + m_nLength - 1);
    z = r->z + r->m_nLength - 1;
    return(true);
  }
  else if(pGrid->occupied(x + m_nWidth - 1, y + m_nHeight - 1, z + m_nLength - 1)) {
    r = pGrid->rect(x + m_nWidth - 1, y + m_nHeight - 1, z + m_nLength - 1);
    z = r->z + r->m_nLength - 1;
    return(true);
  }
  else
    return(false);
}

bool Rectangle::overlaps(const Rectangle* r) const {
  return(!((x + m_nWidth) <= r->x ||
	   (r->x + r->m_nWidth) <= x ||
	   (y + m_nHeight) <= r->y ||
	   (r->y + r->m_nHeight) <= y ||
	   (z + m_nLength) <= r->z ||
	   (r->z + r->m_nLength) <= z));
}

bool Rectangle::fixed() const {
  return(m_bFixed);
}
  
void Rectangle::fix(const Coordinates& c) {
  m_nFixX = c.x.get_ui();
  m_nFixY = c.y.get_ui();
  m_nFixZ = c.z.get_ui();
  m_bFixed = true;
}

void Rectangle::fix(const Fixed& f) {
  m_nFixX = f.m_nX;
  m_nFixY = f.m_nY;
  m_nFixZ = f.m_nZ;
  m_bFixed = true;
}

void Rectangle::rotate() {
  DimsFunctor *pRotator;
  UInt height, length;
  
  pRotator = m_pRotation->rotator ();
  length = pRotator->d3(this);
  height = pRotator->d2(this);
  m_nWidth = pRotator->d1(this);
  m_nHeight = height;
  m_nLength = length;

  m_pRotation = m_pRotation->rotate ();
  m_bRotated = m_pRotation->isRotated ();
}

void Rectangle::rotateTall() {
  while (std::max (m_nWidth, std::max (m_nHeight, m_nLength)) != m_nHeight)
    rotate();
}

void Rectangle::rotateWide() {
  while (std::max (m_nWidth, std::max (m_nHeight, m_nLength)) != m_nWidth)
    rotate();
}

void Rectangle::rotateLong() {
  while (std::max (m_nWidth, std::max (m_nHeight, m_nLength)) != m_nLength)
    rotate();
}

bool Rectangle::equal(const Rectangle& r) const {
  return(m_nWidth == r.m_nWidth &&
	 m_nHeight == r.m_nHeight &&
	 m_nLength == r.m_nLength);
}

bool Rectangle::equal(const Rectangle* r) const {
  return(m_nWidth == r->m_nWidth &&
	 m_nHeight == r->m_nHeight &&
	 m_nLength == r->m_nLength);
}

bool Rectangle::uequal(const Rectangle& r) const {
  return((m_nWidth == r.m_nWidth  && m_nHeight == r.m_nHeight && m_nLength == r.m_nLength) ||
	 (m_nWidth == r.m_nWidth  && m_nHeight == r.m_nLength && m_nLength == r.m_nHeight) ||
	 (m_nWidth == r.m_nHeight && m_nHeight == r.m_nLength && m_nLength == r.m_nWidth)  ||
	 (m_nWidth == r.m_nLength && m_nHeight == r.m_nHeight && m_nLength == r.m_nWidth)  ||
	 (m_nWidth == r.m_nLength && m_nHeight == r.m_nWidth  && m_nLength == r.m_nHeight) ||
	 (m_nWidth == r.m_nHeight && m_nHeight == r.m_nWidth  && m_nLength == r.m_nLength));
}

bool Rectangle::uequal(const Rectangle* r) const {
  return((m_nHeight == r->m_nWidth  && m_nWidth == r->m_nHeight && m_nLength == r->m_nLength) ||
	 (m_nHeight == r->m_nWidth  && m_nWidth == r->m_nLength && m_nLength == r->m_nHeight) ||
	 (m_nHeight == r->m_nHeight && m_nWidth == r->m_nWidth  && m_nLength == r->m_nLength) ||
	 (m_nHeight == r->m_nLength && m_nWidth == r->m_nWidth  && m_nLength == r->m_nHeight) ||
	 (m_nHeight == r->m_nHeight && m_nWidth == r->m_nLength && m_nLength == r->m_nWidth)  ||
	 (m_nHeight == r->m_nLength && m_nWidth == r->m_nHeight && m_nLength == r->m_nWidth));
}

void Rectangle::printTop() const {
  std::cout << "(" << x << "," << y + m_nHeight << "," << z << ")->("
	    << x + m_nWidth << "," << y + m_nHeight << "," << z + m_nLength << ")";
}

void Rectangle::resetRotation() {
  while(m_bRotated) rotate();
}

void Rectangle::constrainHeight(size_t nHeight) {
  if (m_bRotatable) {
    for (int i = 0; i < NUM_ROTATIONS && m_nHeight > nHeight; i++)
      rotate();
  }

  if(m_nWidth > nHeight || m_nLength > nHeight || m_bSquare)
    m_bRotatable = false;
}

void Rectangle::constrainLength(size_t nLength) {
  if (m_bRotatable) {
    for (int i = 0; i < NUM_ROTATIONS && m_nLength > nLength; i++)
      rotate();
  }

  if(m_nWidth > nLength || m_nHeight > nLength || m_bSquare)
    m_bRotatable = false;
}

void Rectangle::constrain(const BoxDimensions& b) {
  if(m_bRotatable) {
    for (int i = 0; i < NUM_ROTATIONS && (m_nHeight > b.m_nHeight || m_nWidth > b.m_nWidth || m_nLength > b.m_nLength); i++)
      rotate();
  }

  if(m_nWidth > b.m_nHeight || m_nHeight > b.m_nWidth || m_nLength > b.m_nLength || m_bSquare)
    m_bRotatable = false;
}

std::ostream& operator<<(std::ostream& os, const Rectangle& r) {
  return(os << r.m_nWidth << "x" << r.m_nHeight << "x" << r.m_nLength);
}

bool Rectangle::rotatable() const {
  return(m_bRotatable);
}

const URational& Rectangle::scale() const {
  return(m_bRotated ? m_nRScale : m_nScale);
}

URational& Rectangle::scale() {
  return(m_bRotated ? m_nRScale : m_nScale);
}

const Rectangle& Rectangle::operator=(const Rectangle& src) {
  m_nID = src.m_nID;
  x = src.x;
  xi = src.xi;
  y = src.y;
  yi = src.yi;
  z = src.z;
  zi = src.zi;
  m_nWidth = src.m_nWidth;
  m_nHeight = src.m_nHeight;
  m_nLength = src.m_nLength;
  m_nMinDim = src.m_nMinDim;
  m_nMaxDim = src.m_nMaxDim;
  m_nArea = src.m_nArea;
  m_Color = src.m_Color;
  m_bRotated = src.m_bRotated;
  m_bRotatable = src.m_bRotatable;
  m_bSquare = src.m_bSquare;
  m_bFixed = src.m_bFixed;
  m_nFixX = src.m_nFixX;
  m_nFixY = src.m_nFixY;
  m_nFixZ = src.m_nFixZ;
  m_nScale = src.m_nScale;
  m_nRScale = src.m_nRScale;
  return(*this);
}

// pupitetris TODO: review this.
void Rectangle::gutsyScale(const BoxDimensions& b) {
  scale().initialize(35, 100);
  size_t nValues = b.m_nHeight - m_nHeight + 1;
  for(size_t nBranches = 1; nBranches <= nValues; ++nBranches) {
    size_t nValuesPerBranch = nValues / nBranches;
    if(nValuesPerBranch < m_nHeight) {
      scale().initialize(nValuesPerBranch, m_nHeight);
      break;
    }
  }
}

void Rectangle::rescale(const URational& ur) {
  m_nLength *= ur.get_num();
  m_nLength /= ur.get_den();
  m_nHeight *= ur.get_num();
  m_nHeight /= ur.get_den();
  m_nWidth *= ur.get_num();
  m_nWidth /= ur.get_den();
  m_nArea = m_nHeight * m_nWidth;
}

void Rectangle::unscale(const URational& ur) {
  m_nLength *= ur.get_den();
  m_nLength /= ur.get_num();
  m_nHeight *= ur.get_den();
  m_nHeight /= ur.get_num();
  m_nWidth *= ur.get_den();
  m_nWidth /= ur.get_num();
  m_nArea = m_nHeight * m_nWidth;
}

UInt Rectangle::minDim2(const UInt& nMax,
			const DimsFunctor* pDims,
			const Rectangle* pRect) const {
  UInt nMin (std::numeric_limits<UInt>::max());

  Rectangle rect = *pRect;    // copies
  Rectangle thisRect = *this;

  int i, j;
  for (i = 0; i < NUM_ROTATIONS; i++) {
    for (j = 0; j < NUM_ROTATIONS; j++) {
      nMin = std::min(nMin, pDims->d2(thisRect) + pDims->d2(rect));
      if(pDims->d1(thisRect) + pDims->d1(rect) <= nMax)
	nMin = std::min(nMin, std::max(pDims->d2(thisRect), pDims->d2(rect)));
      if(!rect.m_bRotatable)
	break;
      rect.rotate();
    }
    if(!m_bRotatable)
      break;
    thisRect.rotate();
  }

  return(nMin);
}

UInt Rectangle::minDim3(const UInt& nMax,
			const DimsFunctor* pDims,
			const Rectangle* pRect) const {
  UInt nMin (std::numeric_limits<UInt>::max());

  Rectangle rect = *pRect;    // copies
  Rectangle thisRect = *this;

  int i, j;
  for (i = 0; i < NUM_ROTATIONS; i++) {
    for (j = 0; j < NUM_ROTATIONS; j++) {
      nMin = std::min(nMin, pDims->d3(thisRect) + pDims->d3(rect));
      if(pDims->d1(thisRect) + pDims->d1(rect) <= nMax)
	nMin = std::min(nMin, std::max(pDims->d3(thisRect), pDims->d3(rect)));
      if(!rect.m_bRotatable)
	break;
      rect.rotate();
    }
    if(!m_bRotatable)
      break;
    thisRect.rotate();
  }

  return(nMin);
}

UInt& Rectangle::width() {
  return(m_nWidth);
}

const UInt& Rectangle::width() const {
  return(m_nWidth);
}

UInt& Rectangle::height() {
  return(m_nHeight);
}

const UInt& Rectangle::height() const {
  return(m_nHeight);
}

UInt& Rectangle::length() {
  return(m_nLength);
}

const UInt& Rectangle::length() const {
  return(m_nLength);
}

bool Rectangle::unit() const {
  return(m_nWidth == 1 && m_nHeight == 1 && m_nLength == 1);
}
