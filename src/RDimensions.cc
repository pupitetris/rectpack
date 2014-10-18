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
#include "Dimensions.h"
#include "DimsFunctor.h"
#include "Rectangle.h"
#include "RDimensions.h"
#include "Component.h"
#include <limits>

RDimensions::RDimensions(const Rectangle& r) :
  m_nOrientation(r.m_bRotatable ? UNORIENTED : ORIENTED),
  m_nWidth(r.m_nWidth),
  m_nHeight(r.m_nHeight),
  m_nLength(r.m_nLength),
  m_nMinDim(r.m_nMinDim),
  m_nArea(r.m_nArea) {
}

RDimensions::RDimensions() :
  m_nOrientation(UNSPECIFIED),
  m_nWidth(0),
  m_nHeight(0),
  m_nLength(0),
  m_nMinDim(0),
  m_nArea(0) {
}

RDimensions::RDimensions(const RDimensions& d) :
  m_nOrientation(d.m_nOrientation),
  m_nWidth(d.m_nWidth),
  m_nHeight(d.m_nHeight),
  m_nLength(d.m_nLength),
  m_nMinDim(d.m_nMinDim),
  m_nArea(d.m_nArea) {
}

RDimensions::RDimensions(const Dimensions& d) :
  m_nOrientation(UNSPECIFIED),
  m_nWidth(d.m_nWidth),
  m_nHeight(d.m_nHeight),
  m_nLength(d.m_nLength),
  m_nMinDim(d.m_nMinDim),
  m_nArea(d.m_nArea) {
}

RDimensions::RDimensions(const BoxDimensions& b) :
  m_nOrientation(UNSPECIFIED),
  m_nWidth(b.m_nWidth),
  m_nHeight(b.m_nHeight),
  m_nLength(b.m_nLength),
  m_nMinDim(std::min(m_nWidth, m_nHeight, m_nLength)),
  m_nArea(b.m_nArea) {
}

const RDimensions& RDimensions::operator=(const RDimensions& d) {
  m_nOrientation = d.m_nOrientation;
  m_nWidth = d.m_nWidth;
  m_nHeight = d.m_nHeight;
  m_nLength = d.m_nLength;
  m_nMinDim = d.m_nMinDim;
  m_nArea = d.m_nArea;
  return(*this);
}

RDimensions::RDimensions(URational nWidth, URational nHeight, URational nLength) :
  m_nOrientation(UNSPECIFIED),
  m_nWidth(nWidth),
  m_nHeight(nHeight),
  m_nLength(nLength),
  m_nMinDim(std::min(m_nWidth, m_nHeight, m_nLength)),
  m_nArea(nWidth * nHeight * nLength) {
}

RDimensions::RDimensions(const Component* c) :
  m_nOrientation(ORIENTED),
  m_nWidth(c->m_Dims.m_nWidth),
  m_nHeight(c->m_Dims.m_nHeight),
  m_nLength(c->m_Dims.m_nLength),
  m_nMinDim(c->m_Dims.m_nMinDim),
  m_nArea(c->m_Dims.m_nArea) {
}

RDimensions::~RDimensions() {
}

void RDimensions::print(std::ostream& os) const {
  os << m_nWidth << "x" << m_nHeight << "x" << m_nLength << std::flush;
}

void RDimensions::print() const {
  print(std::cout);
}

std::ostream& operator<<(std::ostream& os, const RDimensions& d) {
  if(d.m_nOrientation == RDimensions::UNSPECIFIED) 
    return(os << d.m_nWidth << 'x' << d.m_nHeight << 'x' << d.m_nLength);
  else if(d.m_nOrientation == RDimensions::ORIENTED)
    return(os << d.m_nWidth << 'x' << d.m_nHeight << 'x' << d.m_nLength << "o");
  else if(d.m_nOrientation == RDimensions::UNORIENTED)
    return(os << d.m_nWidth << 'x' << d.m_nHeight << 'x' << d.m_nLength << "u");
  else return(os);
}

bool RDimensions::operator<(const RDimensions& d) const {
  if(m_nArea != d.m_nArea)
    return(m_nArea < d.m_nArea);
  else if(m_nWidth != d.m_nWidth)
    return(m_nWidth < d.m_nWidth);
  else if(m_nHeight != d.m_nHeight)
    return(m_nHeight < d.m_nHeight);
  else if(m_nLength != d.m_nLength)
    return(m_nLength < d.m_nLength);
  else
    return(m_nOrientation < d.m_nOrientation);
}

bool RDimensions::operator>(const RDimensions& d) const {
  if(m_nArea != d.m_nArea)
    return(m_nArea > d.m_nArea);
  else if(m_nWidth != d.m_nWidth)
    return(m_nWidth > d.m_nWidth);
  else if(m_nHeight != d.m_nHeight)
    return(m_nHeight > d.m_nHeight);
  else if(m_nLength != d.m_nLength)
    return(m_nLength > d.m_nLength);
  else
    return(m_nOrientation < d.m_nOrientation);
}

bool RDimensions::operator==(const RDimensions& d) const {
  return(m_nWidth == d.m_nWidth &&
	 m_nHeight == d.m_nHeight &&
	 m_nLength == d.m_nLength &&
	 m_nOrientation == d.m_nOrientation);
}

bool RDimensions::operator!=(const RDimensions& d) const {
  return(m_nWidth != d.m_nWidth ||
	 m_nHeight != d.m_nHeight || 
	 m_nLength != d.m_nLength || 
	 m_nOrientation != d.m_nOrientation);
}

bool RDimensions::operator==(const Dimensions& d) const {
  return(m_nWidth.get_ui() == d.m_nWidth &&
	 m_nHeight.get_ui() == d.m_nHeight &&
	 m_nLength.get_ui() == d.m_nLength);
}

bool RDimensions::operator!=(const Dimensions& d) const {
  return(m_nWidth.get_ui() != d.m_nWidth ||
	 m_nHeight.get_ui() != d.m_nHeight ||
	 m_nLength.get_ui() != d.m_nLength);
}

void RDimensions::setArea() {
  m_nArea = m_nWidth * m_nHeight * m_nLength;
  m_nMinDim = std::min(m_nWidth, m_nHeight, m_nLength);
}

bool RDimensions::canFit(const URational& nWidth,
			 const URational& nHeight,
			 const URational& nLength) const {
  return(m_nWidth <= nWidth && m_nHeight <= nHeight && m_nLength <= nLength);
}

bool RDimensions::square() const {
  return(m_nWidth == m_nHeight && m_nWidth == m_nLength);
}

void RDimensions::initMax() {
  m_nWidth = std::numeric_limits<UInt>::max();
  m_nHeight = std::numeric_limits<UInt>::max();
  m_nLength = std::numeric_limits<UInt>::max();
}

void RDimensions::initMin() {
  m_nWidth = std::numeric_limits<UInt>::min();
  m_nHeight = std::numeric_limits<UInt>::min();
  m_nLength = std::numeric_limits<UInt>::min();
}

bool RDimensions::canEqual(const RDimensions& d,
			   bool bUnoriented) const {
  if(rotatable(bUnoriented) || d.rotatable(bUnoriented))
    return((m_nHeight == d.m_nWidth  && m_nWidth == d.m_nHeight && m_nLength == d.m_nLength) ||
	   (m_nHeight == d.m_nWidth  && m_nWidth == d.m_nLength && m_nLength == d.m_nHeight) ||
	   (m_nHeight == d.m_nHeight && m_nWidth == d.m_nWidth  && m_nLength == d.m_nLength) ||
	   (m_nHeight == d.m_nLength && m_nWidth == d.m_nWidth  && m_nLength == d.m_nHeight) ||
	   (m_nHeight == d.m_nHeight && m_nWidth == d.m_nLength && m_nLength == d.m_nWidth)  ||
	   (m_nHeight == d.m_nLength && m_nWidth == d.m_nHeight && m_nLength == d.m_nWidth));
  else
    return(m_nHeight == d.m_nHeight && m_nWidth == d.m_nWidth && m_nLength == d.m_nLength);
}

bool RDimensions::rotatable(bool bUnoriented) const {
  return(!square() &&
	 (bUnoriented ?
	  (m_nOrientation != ORIENTED) :
	  (m_nOrientation == UNORIENTED)));
}

void RDimensions::rotateL() {
  std::swap(m_nWidth, m_nHeight);
}

void RDimensions::rotateW() {
  std::swap(m_nLength, m_nHeight);
}

void RDimensions::rotateH() {
  std::swap(m_nWidth, m_nLength);
}

URational RDimensions::ratiowhl() const {
  return(m_nWidth / sqrt(m_nHeight * m_nHeight + m_nLength * m_nLength));
}

URational RDimensions::ratiohwl() const {
  return(m_nHeight / sqrt(m_nWidth * m_nWidth + m_nLength * m_nLength));
}

URational RDimensions::ratiolhw() const {
  return(m_nLength / sqrt(m_nHeight * m_nHeight + m_nWidth * m_nWidth));
}

const RDimensions& RDimensions::operator*=(const URational& n) {
  m_nWidth *= n;
  m_nHeight *= n;
  m_nLength *= n;
  return(*this);
}

const RDimensions& RDimensions::operator/=(const URational& n) {
  m_nWidth /= n;
  m_nHeight /= n;
  m_nLength /= n;
  return(*this);
}

const URational& RDimensions::maxDim() const {
  return(std::max(m_nWidth, m_nHeight, m_nLength));
}

URational RDimensions::minDim(const URational& nMax,
			      const RDimensions& r,
			      const DimsFunctor* pDims1,
			      const DimsFunctor* pDims2) const {
  URational nMin(pDims1->d1(this) + pDims2->d1(r));
  if (pDims1->d1(this) + pDims2->d1(r) <= nMax)
    nMin = std::min(nMin, std::max(pDims1->d2(this), pDims->d2(r)));
  if (pDims1->d2(this) + pDims2->d2(r) <= nMax)
    nMin = std::min(nMin, std::max(pDims1->d3(this), pDims->d3(r)));
  return(nMin);
}

void RDimensions::relax() {
  m_nOrientation = ORIENTED;
  m_nWidth = m_nMinDim;
  m_nHeight = m_nMinDim;
  m_nLength = m_nMinDim;
  m_nArea = m_nWidth * m_nHeight * m_nLength;
}

bool RDimensions::unit() const{
  return(m_nWidth.get_str() == "1/1" &&
	 m_nHeight.get_str() == "1/1" &&
	 m_nLength.get_str() == "1/1");
}
