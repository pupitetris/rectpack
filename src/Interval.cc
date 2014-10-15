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

#include "Interval.h"
#include "Range.h"
#include "Rectangle.h"

std::ostream& operator<<(std::ostream& os, const Interval& i) {
  return(os << '[' << i.m_nBegin << ',' << i.m_nEnd << "],w="
	 << i.m_nWidth);
}

const Interval& Interval::operator=(const Rectangle* r) {
  m_nBegin = m_nEnd = r->y;
  m_nWidth = r->m_nHeight;
  return(*this);
}

void Interval::set(const Rectangle* r, const Range* pRange,
		   UInt nCurrent, UInt nBranches) {
  UInt nOffset1 = nCurrent * pRange->size() / nBranches;
  UInt nOffset2 = (nCurrent + 1) * pRange->size() / nBranches;
  m_nBegin = pRange->lb() + nOffset1;
  m_nEnd = pRange->lb() + nOffset2 - 1;
  m_nWidth = r->m_nHeight - (m_nEnd - m_nBegin);
}

void Interval::set(const Rectangle* r, UInt lb, UInt ub,
		   UInt nCurrent, UInt nBranches) {
  UInt nRangeSize = ub - lb + 1;
  UInt nOffset1 = nCurrent * nRangeSize / nBranches;
  UInt nOffset2 = (nCurrent + 1) * nRangeSize / nBranches;
  m_nBegin = lb + nOffset1;
  m_nEnd = lb + nOffset2 - 1;
  m_nWidth = r->m_nHeight - (m_nEnd - m_nBegin);
}
