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
#include "Integer.h"
#include "RatDims.h"
#include "Rectangle.h"

RatDims::RatDims() :
  m_nWidth(0),
  m_nHeight(0) {
}

RatDims::RatDims(const URational& nWidth, const URational& nHeight) :
  m_nWidth(nWidth),
  m_nHeight(nHeight) {
}

RatDims::~RatDims() {
}

RatDims& RatDims::operator=(const BoxDimensions& b) {
  m_nWidth = b.m_nWidth;
  m_nHeight = b.m_nHeight;
  return(*this);
}

RatDims& RatDims::operator=(const Rectangle& r) {
  m_nWidth = r.m_nWidth;
  m_nHeight = r.m_nHeight;
  return(*this);
}

void RatDims::print() const {
  std::cout << m_nWidth << 'x' << m_nHeight << std::flush;
}

std::ostream& operator<<(std::ostream& os, const RatDims& r) {
  return(os << r.m_nWidth << 'x' << r.m_nHeight);
}

RatDims& RatDims::operator*=(const URational& n) {
  m_nWidth *= n;
  m_nHeight *= n;
  return(*this);
}

RatDims& RatDims::operator/=(const URational& n) {
  m_nWidth /= n;
  m_nHeight /= n;
  return(*this);
}

URational RatDims::minLabelScale() const {
  URational nScale;
  std::ostringstream ossw, ossh;
  ossw << m_nWidth;
  ossh << m_nHeight;
  UInt nWidth(ossw.str().size()), nHeight(ossh.str().size());

  /**
   * Try WxH on one line.
   */

  nScale = minScaleToContain(nWidth + 1 + nHeight + 1, 2);

  /**
   * Try Wx \n H.
   */

  UInt n = std::max(nWidth + 1, nHeight);
  nScale = std::min(nScale, minScaleToContain(n + 1, 3));

  /**
   * Try W \n xH.
   */

  n = std::max(nWidth, nHeight + 1);
  nScale = std::min(nScale, minScaleToContain(n + 1, 3));

  /**
   * Try W \n x \n H.
   */

  n = std::max(nWidth, nHeight);
  nScale = std::min(nScale, minScaleToContain(n + 1, 4));
  return(nScale);
}

URational RatDims::minScaleToContain(const URational& w,
				     const URational& h) const {
  URational nScale1 = w / m_nWidth;
  if(m_nHeight * nScale1 < h)
    nScale1 = h / m_nHeight;
  URational nScale2 = h / m_nHeight;
  if(m_nWidth * nScale2 < w)
    nScale2 = w / m_nWidth;
  return(nScale1 < nScale2 ? nScale1 : nScale2);
}

void RatDims::rotate() {
  std::swap(m_nWidth, m_nHeight);
}
