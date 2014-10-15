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
#include <iomanip>
#include <iostream>
#include <sstream>

Rectangle::Rectangle() :
  m_bRotated(false),
  m_bRotatable(true),
  m_bSquare(false),
  m_bFixed(false) {
}

Rectangle::~Rectangle() {
}

void Rectangle::initialize(const UInt& nSize) {
  initialize(nSize, nSize);
}

void Rectangle::initialize(const UInt& nWidth, const UInt& nHeight) {
  m_nWidth = nWidth;
  m_nHeight = nHeight;
  m_nArea = m_nWidth * m_nHeight;
  m_nMinDim = std::min(nWidth, nHeight);
  m_nMaxDim = std::max(nWidth, nHeight);
  m_bSquare = (m_nWidth == m_nHeight);
  if(m_bSquare) m_bRotatable = false;
}

void Rectangle::initialize(const RDimensions& d) {
  if(d.m_nOrientation == RDimensions::ORIENTED)
    m_bRotatable = false;
  if(d.m_nWidth.integer() && d.m_nHeight.integer())
    initialize(d.m_nWidth.get_ui(), d.m_nHeight.get_ui());
  else
    std::cout << "Rectangle::size(...): Dimensions should be integers." << std::endl;
  m_bRotated = false;
}

void Rectangle::print() const {
  if(m_bRotated) printHW();
  else printWH();
}

void Rectangle::printWH() const {
  std::cout << m_nWidth << "x" << m_nHeight << std::flush;
}

void Rectangle::printHW() const {
  std::cout << m_nHeight << "x" << m_nWidth << std::flush;
}

void Rectangle::printNoLocation() const {
  std::cout << std::setw(2) << m_nID << ":" << std::setw(2)
	    << m_nWidth << "x" << std::setw(2) << m_nHeight;
}

int Rectangle::nextX() const {
  return(x + m_nWidth);
}

int Rectangle::nextY() const {
  return(y + m_nHeight);
}

bool Rectangle::skippingX(const Grid* pGrid) {
  const Rectangle* r;
  if(pGrid->occupied(x, y)) {
    r = pGrid->rect(x, y);
    x = r->x + r->m_nWidth - 1;
    return(true);
  }
  else if(pGrid->occupied(x + m_nWidth - 1, y)) {
    r = pGrid->rect(x + m_nWidth - 1, y);
    x = r->x + r->m_nWidth - 1;
    return(true);
  }
  else if(pGrid->occupied(x, y + m_nHeight - 1)) {
    r = pGrid->rect(x, y + m_nHeight - 1);
    x = r->x + r->m_nWidth - 1;
    return(true);
  }
  else if(pGrid->occupied(x + m_nWidth - 1, y + m_nHeight - 1)) {
    r = pGrid->rect(x + m_nWidth - 1, y + m_nHeight - 1);
    x = r->x + r->m_nWidth - 1;
    return(true);
  }
  else
    return(false);
}

bool Rectangle::skippingY(const Grid* pGrid) {
  const Rectangle* r;
  if(pGrid->occupied(x, y)) {
    r = pGrid->rect(x, y);
    y = r->y + r->m_nHeight - 1;
    return(true);
  }
  else if(pGrid->occupied(x + m_nWidth - 1, y)) {
    r = pGrid->rect(x + m_nWidth - 1, y);
    y = r->y + r->m_nHeight - 1;
    return(true);
  }
  else if(pGrid->occupied(x, y + m_nHeight - 1)) {
    r = pGrid->rect(x, y + m_nHeight - 1);
    y = r->y + r->m_nHeight - 1;
    return(true);
  }
  else if(pGrid->occupied(x + m_nWidth - 1, y + m_nHeight - 1)) {
    r = pGrid->rect(x + m_nWidth - 1, y + m_nHeight - 1);
    y = r->y + r->m_nHeight - 1;
    return(true);
  }
  else
    return(false);
}

bool Rectangle::overlaps(const Rectangle* r) const {
  return(!((x + m_nWidth) <= r->x ||
	   (r->x + r->m_nWidth) <= x ||
	   (y + m_nHeight) <= r->y ||
	   (r->y + r->m_nHeight) <= y));
}

bool Rectangle::fixed() const {
  return(m_bFixed);
}
  
void Rectangle::fix(const Coordinates& c) {
  m_nFixX = c.x.get_ui();
  m_nFixY = c.y.get_ui();
  m_bFixed = true;
}

void Rectangle::fix(const Fixed& f) {
  m_nFixX = f.m_nX;
  m_nFixY = f.m_nY;
  m_bFixed = true;
}

void Rectangle::rotate() {
  std::swap(m_nWidth, m_nHeight);
  m_bRotated = !m_bRotated;
}

void Rectangle::rotateTall() {
  if(m_nWidth > m_nHeight)
    rotate();
}

void Rectangle::rotateWide() {
  if(m_nHeight > m_nWidth)
    rotate();
}

bool Rectangle::equal(const Rectangle& r) const {
  return(m_nWidth == r.m_nWidth &&
	 m_nHeight == r.m_nHeight);
}

bool Rectangle::equal(const Rectangle* r) const {
  return(m_nWidth == r->m_nWidth &&
	 m_nHeight == r->m_nHeight);
}

bool Rectangle::uequal(const Rectangle& r) const {
  return(equal(r) ||
	 (m_nWidth == r.m_nHeight &&
	  m_nHeight == r.m_nWidth));
}

bool Rectangle::uequal(const Rectangle* r) const {
  return(equal(r) ||
	 (m_nWidth == r->m_nHeight &&
	  m_nHeight == r->m_nWidth));
}

void Rectangle::printTop() const {
  std::cout << "(" << x << "," << y + m_nHeight << ")->("
	    << x + m_nWidth << "," << y + m_nHeight << ")";
}

void Rectangle::resetRotation() {
  if(m_bRotated) rotate();
}

void Rectangle::constrainHeight(size_t nHeight) {
  if(m_bRotatable && m_nHeight > nHeight)
    rotate();
  if(m_nWidth > nHeight || m_bSquare)
    m_bRotatable = false;
}

void Rectangle::constrain(const BoxDimensions& b) {
  if(m_bRotatable && (m_nHeight > b.m_nHeight || m_nWidth > b.m_nWidth))
    rotate();
  if(m_nWidth > b.m_nHeight || m_nHeight > b.m_nWidth || m_bSquare)
    m_bRotatable = false;
}

std::ostream& operator<<(std::ostream& os, const Rectangle& r) {
  return(os << r.m_nWidth << "x" << r.m_nHeight);
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
  m_nWidth = src.m_nWidth;
  m_nHeight = src.m_nHeight;
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
  m_nScale = src.m_nScale;
  m_nRScale = src.m_nRScale;
  return(*this);
}

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
  m_nHeight *= ur.get_num();
  m_nHeight /= ur.get_den();
  m_nWidth *= ur.get_num();
  m_nWidth /= ur.get_den();
  m_nArea = m_nHeight * m_nWidth;
}

void Rectangle::unscale(const URational& ur) {
  m_nHeight *= ur.get_den();
  m_nHeight /= ur.get_num();
  m_nWidth *= ur.get_den();
  m_nWidth /= ur.get_num();
  m_nArea = m_nHeight * m_nWidth;
}

UInt Rectangle::minDim(const UInt& nMax,
		       const DimsFunctor* pDims,
		       const Rectangle* pRect) const {
  UInt nMin(pDims->d2(this) + pDims->d2(pRect));
  if(pDims->d1(this) + pDims->d1(pRect) <= nMax)
    nMin = std::min(nMin, std::max(pDims->d2(this), pDims->d2(pRect)));
  if(pRect->m_bRotatable) {
    nMin = std::min(nMin, pDims->d2(this) + pDims->d1(pRect));
    if(pDims->d1(this) + pDims->d2(pRect) <= nMax)
      nMin = std::min(nMin, std::max(pDims->d2(this), pDims->d1(pRect)));
  }

  if(m_bRotatable) {
    nMin = std::min(nMin, pDims->d1(this) + pDims->d2(pRect));
    if(pDims->d2(this) + pDims->d1(pRect) <= nMax)
      nMin = std::min(nMin, std::max(pDims->d1(this), pDims->d2(pRect)));
    if(pRect->m_bRotatable) {
      nMin = std::min(nMin, pDims->d1(this) + pDims->d1(pRect));
      if(pDims->d2(this) + pDims->d2(pRect) <= nMax)
	nMin = std::min(nMin, std::max(pDims->d1(this), pDims->d1(pRect)));
    }
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

bool Rectangle::unit() const {
  return(m_nWidth == 1 && m_nHeight == 1);
}

void Rectangle::swap() {
  std::swap(m_nWidth, m_nHeight);
}
