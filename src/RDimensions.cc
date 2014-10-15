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
  m_nMinDim(r.m_nMinDim),
  m_nArea(r.m_nArea) {
}

RDimensions::RDimensions() :
  m_nOrientation(UNSPECIFIED),
  m_nWidth(0),
  m_nHeight(0),
  m_nMinDim(0),
  m_nArea(0) {
}

RDimensions::RDimensions(const RDimensions& d) :
  m_nOrientation(d.m_nOrientation),
  m_nWidth(d.m_nWidth),
  m_nHeight(d.m_nHeight),
  m_nMinDim(d.m_nMinDim),
  m_nArea(d.m_nArea) {
}

RDimensions::RDimensions(const Dimensions& d) :
  m_nOrientation(UNSPECIFIED),
  m_nWidth(d.m_nWidth),
  m_nHeight(d.m_nHeight),
  m_nMinDim(d.m_nMinDim),
  m_nArea(d.m_nArea) {
}

RDimensions::RDimensions(const BoxDimensions& b) :
  m_nOrientation(UNSPECIFIED),
  m_nWidth(b.m_nWidth),
  m_nHeight(b.m_nHeight),
  m_nMinDim(std::min(m_nWidth, m_nHeight)),
  m_nArea(b.m_nArea) {
}

const RDimensions& RDimensions::operator=(const RDimensions& d) {
  m_nOrientation = d.m_nOrientation;
  m_nWidth = d.m_nWidth;
  m_nHeight = d.m_nHeight;
  m_nMinDim = d.m_nMinDim;
  m_nArea = d.m_nArea;
  return(*this);
}

RDimensions::RDimensions(URational nWidth, URational nHeight) :
  m_nOrientation(UNSPECIFIED),
  m_nWidth(nWidth),
  m_nHeight(nHeight),
  m_nMinDim(std::min(m_nWidth, m_nHeight)),
  m_nArea(nWidth * nHeight) {
}

RDimensions::RDimensions(const Component* c) :
  m_nOrientation(ORIENTED),
  m_nWidth(c->m_Dims.m_nWidth),
  m_nHeight(c->m_Dims.m_nHeight),
  m_nMinDim(c->m_Dims.m_nMinDim),
  m_nArea(c->m_Dims.m_nArea) {
}

RDimensions::~RDimensions() {
}

void RDimensions::print(std::ostream& os) const {
  os << m_nWidth << "x" << m_nHeight << std::flush;
}

void RDimensions::print() const {
  print(std::cout);
}

std::ostream& operator<<(std::ostream& os, const RDimensions& d) {
  if(d.m_nOrientation == RDimensions::UNSPECIFIED) 
    return(os << d.m_nWidth << 'x' << d.m_nHeight);
  else if(d.m_nOrientation == RDimensions::ORIENTED)
    return(os << d.m_nWidth << 'x' << d.m_nHeight << "o");
  else if(d.m_nOrientation == RDimensions::UNORIENTED)
    return(os << d.m_nWidth << 'x' << d.m_nHeight << "u");
  else return(os);
}

bool RDimensions::operator<(const RDimensions& d) const {
  if(m_nArea != d.m_nArea)
    return(m_nArea < d.m_nArea);
  else if(m_nWidth != d.m_nWidth)
    return(m_nWidth < d.m_nWidth);
  else if(m_nHeight != d.m_nHeight)
    return(m_nHeight < d.m_nHeight);
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
  else
    return(m_nOrientation < d.m_nOrientation);
}

bool RDimensions::operator==(const RDimensions& d) const {
  return(m_nWidth == d.m_nWidth &&
	 m_nHeight == d.m_nHeight &&
	 m_nOrientation == d.m_nOrientation);
}

bool RDimensions::operator!=(const RDimensions& d) const {
  return(m_nWidth != d.m_nWidth ||
	 m_nHeight != d.m_nHeight || 
	 m_nOrientation != d.m_nOrientation);
}

bool RDimensions::operator==(const Dimensions& d) const {
  return(m_nWidth.get_ui() == d.m_nWidth &&
	 m_nHeight.get_ui() == d.m_nHeight);
}

bool RDimensions::operator!=(const Dimensions& d) const {
  return(m_nWidth.get_ui() != d.m_nWidth ||
	 m_nHeight.get_ui() != d.m_nHeight);
}

void RDimensions::setArea() {
  m_nArea = m_nWidth * m_nHeight;
  m_nMinDim = std::min(m_nWidth, m_nHeight);
}

bool RDimensions::canFit(const URational& nWidth,
			 const URational& nHeight) const {
  return(m_nWidth <= nWidth && m_nHeight <= nHeight);
}

bool RDimensions::square() const {
  return(m_nWidth == m_nHeight);
}

void RDimensions::initMax() {
  m_nWidth = std::numeric_limits<UInt>::max();
  m_nHeight = std::numeric_limits<UInt>::max();
}

void RDimensions::initMin() {
  m_nWidth = std::numeric_limits<UInt>::min();
  m_nHeight = std::numeric_limits<UInt>::min();
}

bool RDimensions::canEqual(const RDimensions& d,
			   bool bUnoriented) const {
  if(rotatable(bUnoriented) || d.rotatable(bUnoriented))
    return((m_nHeight == d.m_nWidth && m_nWidth == d.m_nHeight) ||
	   (m_nHeight == d.m_nHeight && m_nWidth == d.m_nWidth));
  else
    return(m_nHeight == d.m_nHeight && m_nWidth == d.m_nWidth);
}

bool RDimensions::rotatable(bool bUnoriented) const {
  return(!square() &&
	 (bUnoriented ?
	  (m_nOrientation != ORIENTED) :
	  (m_nOrientation == UNORIENTED)));
}

void RDimensions::rotate() {
  std::swap(m_nWidth, m_nHeight);
}

URational RDimensions::ratiowh() const {
  return(m_nWidth / m_nHeight);
}

URational RDimensions::ratiohw() const {
  return(m_nHeight / m_nWidth);
}

const RDimensions& RDimensions::operator*=(const URational& n) {
  m_nWidth *= n;
  m_nHeight *= n;
  return(*this);
}

const RDimensions& RDimensions::operator/=(const URational& n) {
  m_nWidth /= n;
  m_nHeight /= n;
  return(*this);
}

const URational& RDimensions::maxDim() const {
  return(m_nWidth > m_nHeight ? m_nWidth : m_nHeight);
}

URational RDimensions::minDim(const URational& nMax,
			      const RDimensions& r,
			      const DimsFunctor* pDims1,
			      const DimsFunctor* pDims2) const {
  if(pDims1->d1(this) + pDims2->d1(r) > nMax)
    return(pDims1->d2(this) + pDims2->d2(r));
  else
    return(std::max(pDims1->d2(this), pDims2->d2(r)));
}

void RDimensions::relax() {
  m_nOrientation = ORIENTED;
  m_nWidth = m_nMinDim;
  m_nHeight = m_nMinDim;
  m_nArea = m_nWidth * m_nHeight;
}

bool RDimensions::unit() const{
  return(m_nWidth.get_str() == "1/1" &&
	 m_nHeight.get_str() == "1/1");
}
