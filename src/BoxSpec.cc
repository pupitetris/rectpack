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

#include "BoxSpec.h"
#include "Rational.h"

BoxSpec::BoxSpec() :
  m_nWidth(0),
  m_nHeight(0){
}

BoxSpec::BoxSpec(const BoxSpec& b) :
  m_nWidth(b.m_nWidth),
  m_nHeight(b.m_nHeight) {
}

void BoxSpec::clear() {
  m_nWidth.clear();
  m_nHeight.clear();
}

const BoxSpec& BoxSpec::operator=(const BoxSpec& b) {
  m_nWidth = b.m_nWidth;
  m_nHeight = b.m_nHeight;
  return(*this);
}

BoxSpec::~BoxSpec() {
}

void BoxSpec::print() const {
  std::cout << m_nWidth << "x" << m_nHeight << std::endl;
}

std::ostream& operator<<(std::ostream& os, const BoxSpec& b) {
  return(os << b.m_nWidth << "x" << b.m_nHeight);
}

bool BoxSpec::load(const std::string& s) {
  if(s.empty()) return(true);
  size_t x = s.find_first_of("xX");
  if(x >= s.size()) return(false);

  std::string sWidth(s.substr(0, x));
  std::string sHeight(s.substr(x + 1));
  m_nWidth.initialize(sWidth);
  m_nHeight.initialize(sHeight);
  return(true);
}

void BoxSpec::rescale(const URational& nScale) {
  if(nScale > (URational) 0) {
    m_nWidth /= nScale;
    m_nHeight /= nScale;
  }
}
