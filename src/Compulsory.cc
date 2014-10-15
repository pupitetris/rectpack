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

#include "Compulsory.h"
#include "Rectangle.h"

Compulsory::Compulsory() :
  m_pRect(NULL) {
}

Compulsory::~Compulsory() {
}

const Compulsory& Compulsory::operator+=(const UInt& ur) {
  m_nStart += ur;
  m_nEnd += ur;
  return(*this);
}

void Compulsory::initialize(const UInt& nInterval,
			    const Rectangle* pRectangle) {
  m_nStart.m_nLeft = 0;
  m_nStart.m_nRight = nInterval;
  m_nEnd = m_nStart;
  m_nEnd += pRectangle->m_nWidth;
  m_pRect = pRectangle;
}

void Compulsory::initialize(const UInt& nStart1,
			    const UInt& nStart2,
			    const Rectangle* pRectangle) {
  m_nStart.m_nLeft = nStart1;
  m_nStart.m_nRight = nStart2;
  m_nEnd = m_nStart;
  m_nEnd += pRectangle->m_nWidth;
  m_pRect = pRectangle;
}

const UInt& Compulsory::left() const {
  return(m_nStart.m_nRight);
}

UInt& Compulsory::left() {
  return(m_nStart.m_nRight);
}

const UInt& Compulsory::right() const {
  return(m_nEnd.m_nLeft);
}

UInt& Compulsory::right() {
  return(m_nEnd.m_nLeft);
}

bool Compulsory::isFull() const {
  return(m_nStart.isPoint());
}

UInt Compulsory::size() const {
  return(right() - left());
}

UInt Compulsory::csize() const {
  return(m_nStart.size());
}

UInt Compulsory::area() const {
  return(size() * m_pRect->m_nHeight);
}

UInt Compulsory::carea() const {
  return(csize() * m_pRect->m_nHeight);
}

void Compulsory::print() const {
  std::cout << *this << std::endl;
}

std::ostream& operator<<(std::ostream& os, const Compulsory& c) {
  if(c.m_pRect) os << *c.m_pRect;
  else os << "n/a";
  return(os << ": Start=" << c.m_nStart << ", End=" << c.m_nEnd);
}

bool Compulsory::overlaps(UInt n) const {
  return(left() <= n && n < right());
}
