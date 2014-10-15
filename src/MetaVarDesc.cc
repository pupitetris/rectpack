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

#include "MetaVarDesc.h"
#include "Rectangle.h"

MetaVarDesc::MetaVarDesc()  :
  m_pRect1(NULL),
  m_pRect2(NULL),
  m_nMinArea(0),
  m_nMaxArea(0) {
}

MetaVarDesc::~MetaVarDesc() {
}

void MetaVarDesc::initialize(const Rectangle* r1,
			     const Rectangle* r2) {
  m_pRect1 = r1;
  m_pRect2 = r2;
  m_nMinArea = std::min(r1->m_nArea, r2->m_nArea);
  m_nMaxArea = std::max(r1->m_nArea, r2->m_nArea);
}

std::ostream& operator<<(std::ostream& os, const MetaVarDesc& rhs) {
  return(os << '(' << rhs.m_pRect1->m_nID << ','
	 << rhs.m_pRect2->m_nID << ')');
}

void MetaVarDesc::print() const {
  std::cout << *this << std::flush;
}
