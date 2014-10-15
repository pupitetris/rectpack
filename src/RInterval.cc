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

#include "RInterval.h"

RInterval::RInterval() :
  m_nLeft(0),
  m_nRight(0) {
}

RInterval::~RInterval() {
}

const RInterval& RInterval::operator+=(const UInt& ur) {
  m_nLeft += ur;
  m_nRight += ur;
  return(*this);
}

const RInterval& RInterval::operator=(const UInt& ur) {
  m_nLeft = m_nRight = ur;
  return(*this);
}

UInt RInterval::size() const {
  return(m_nRight - m_nLeft + 1);
}

bool RInterval::isPoint() const {
  return(m_nLeft == m_nRight);
}

void RInterval::print() const {
  std::cout << "[" << m_nLeft << "," << m_nRight << "]" << std::flush;
}

std::ostream& operator<<(std::ostream& os, const RInterval& rhs) {
  return(os << "[" << rhs.m_nLeft << "," << rhs.m_nRight << "]" << std::flush);
}

bool RInterval::empty() const {
  return(m_nLeft > m_nRight);
}
