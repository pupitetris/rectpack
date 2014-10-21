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
#include "Rectangle.h"
#include <algorithm>
#include <sstream>

BoxDimensions::BoxDimensions() {
}

BoxDimensions::~BoxDimensions() {
}

void BoxDimensions::initialize(const UInt& n) {
  initialize(n, n, n);
}

void BoxDimensions::initialize(const UInt& w, const UInt& h, const UInt& l) {
  m_nWidth = w;
  m_nHeight = h;
  m_nLength = l;
  m_nArea = w * h * l;
}

void BoxDimensions::initializeH(const UInt& h) {
  m_nHeight = h;
  m_nArea = m_nWidth * h * m_nLength;
}

void BoxDimensions::initializeL(const UInt& l) {
  m_nLength = l;
  m_nArea = m_nWidth * m_nHeight * l;
}

bool BoxDimensions::operator<(const BoxDimensions& rhs) const {
  if(m_nArea != rhs.m_nArea)
    return(m_nArea < rhs.m_nArea);
  else if(m_nWidth != rhs.m_nWidth)
    return(m_nWidth < rhs.m_nWidth);
  else if(m_nHeight != rhs.m_nHeight)
    return(m_nHeight < rhs.m_nHeight);
  return(m_nLength < rhs.m_nLength)
}

bool BoxDimensions::operator>(const BoxDimensions& rhs) const {
  if(m_nArea != rhs.m_nArea)
    return(m_nArea > rhs.m_nArea);
  else if(m_nWidth != rhs.m_nWidth)
    return(m_nWidth > rhs.m_nWidth);
  else if(m_nHeight != rhs.m_nHeight)
    return(m_nHeight > rhs.m_nHeight);
  return(m_nLength > rhs.m_nLength)
}

URational BoxDimensions::ratioW() const {
  return(URational(m_nWidth, sqrt(m_nHeight * m_nHeight + m_nLength * m_nLength)));
}

URational BoxDimensions::ratioH() const {
  return(URational(m_nHeight, sqrt(m_nWidth * m_nWidth + m_nLength * m_nLength)));
}

URational BoxDimensions::ratioL() const {
  return(URational(m_nLength, sqrt(m_nHeight * m_nHeight + m_nWidth * m_nWidth)));
}

std::string BoxDimensions::toString() const {
  std::ostringstream oss;
  oss << (*this);
  return(oss.str());
}

std::ostream& operator<<(std::ostream& os, const BoxDimensions& b) {
  return(os << b.m_nWidth << 'x' << b.m_nHeight << std::flush);
}

bool BoxDimensions::canFit(const Rectangle* r) const {
  return(r->m_nWidth <= m_nWidth && r->m_nHeight <= m_nHeight);
}

void BoxDimensions::rotate() {
  std::swap(m_nWidth, m_nHeight);
}

bool BoxDimensions::square() const {
  return(m_nWidth == m_nHeight && m_nWidth == m_nLength);
}

const BoxDimensions& BoxDimensions::operator*=(const UInt& ur) {
  m_nWidth *= ur;
  m_nHeight *= ur;
  m_nLength *= ur;
  m_nArea = m_nWidth * m_nHeight * m_nLength;
  return(*this);
}

const BoxDimensions& BoxDimensions::operator/=(const UInt& ur) {
  m_nWidth /= ur;
  m_nHeight /= ur;
  m_nLength /= ur;
  m_nArea = m_nWidth * m_nHeight * m_nLength;
  return(*this);
}

const BoxDimensions& BoxDimensions::operator*=(const URational& ur) {
  m_nWidth *= ur.get_num();
  m_nWidth /= ur.get_den();
  m_nHeight *= ur.get_num();
  m_nHeight /= ur.get_den();
  m_nLength *= ur.get_num();
  m_nLength /= ur.get_den();
  m_nArea = m_nWidth * m_nHeight * m_nLength;
  return(*this);
}

const BoxDimensions& BoxDimensions::operator/=(const URational& ur) {
  m_nWidth *= ur.get_den();
  m_nWidth /= ur.get_num();
  m_nHeight *= ur.get_den();
  m_nHeight /= ur.get_num();
  m_nLength *= ur.get_den();
  m_nLength /= ur.get_num();
  m_nArea = m_nWidth * m_nHeight * m_nLength;
  return(*this);
}

void BoxDimensions::print() const {
  std::cout << m_nWidth << "x" << m_nHeight << "x" << m_nLength << std::endl;
}

bool BoxDimensions::operator==(const BoxDimensions& rhs) const {
  return(m_nWidth == rhs.m_nWidth && m_nHeight == rhs.m_nHeight && m_nLength == rhs.m_nLength);
}

bool BoxDimensions::operator!=(const BoxDimensions& rhs) const {
  return(m_nWidth != rhs.m_nWidth || m_nHeight != rhs.m_nHeight || m_nLength != rhs.m_nLength);
}
