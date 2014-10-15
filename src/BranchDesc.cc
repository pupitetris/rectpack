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

#include "BranchDesc.h"
#include "Rectangle.h"

BranchDesc::BranchDesc() :
  m_pRect(NULL) {
  std::fill(m_pTotal, m_pTotal + 4, 0);
  std::fill(m_pRTotal, m_pRTotal + 4, 0);
  for(UInt i = 0; i < 4; ++i) {
    std::fill(m_pBranches[i], m_pBranches[i] + 3, 0);
    std::fill(m_pRBranches[i], m_pRBranches[i] + 3, 0);
  }
}

BranchDesc::BranchDesc(const BranchDesc& rhs) :
  m_pRect(rhs.m_pRect) {
  std::copy(rhs.m_pTotal, rhs.m_pTotal + 4, m_pTotal);
  std::copy(rhs.m_pRTotal, rhs.m_pRTotal + 4, m_pRTotal);
  for(UInt i = 0; i < 4; ++i) {
    std::copy(rhs.m_pBranches[i], rhs.m_pBranches[i] + 3,
	      m_pBranches[i]);
    std::copy(rhs.m_pRBranches[i], rhs.m_pRBranches[i] + 3,
	      m_pRBranches[i]);
  }
}

BranchDesc::~BranchDesc() {
}

const BranchDesc& BranchDesc::operator=(const BranchDesc& rhs) {
  std::copy(rhs.m_pTotal, rhs.m_pTotal + 4, m_pTotal);
  std::copy(rhs.m_pRTotal, rhs.m_pRTotal + 4, m_pRTotal);
  for(UInt i = 0; i < 4; ++i) {
    std::copy(rhs.m_pBranches[i], rhs.m_pBranches[i] + 3,
	      m_pBranches[i]);
    std::copy(rhs.m_pRBranches[i], rhs.m_pRBranches[i] + 3,
	      m_pRBranches[i]);
  }
  m_pRect = rhs.m_pRect;
  return(*this);
}

UInt BranchDesc::total() const {
  return(m_pTotal[0] + m_pRTotal[0]);
}

bool BranchDesc::operator<(const BranchDesc& rhs) const {
  return(total() < rhs.total());
}

void BranchDesc::print(const UInt pTotal[4], const UInt pBranches[4][3],
		       std::ostream& os) const {
  const char str[4][3] = { "TT", "TF", "FT", "FF" };
  for(UInt i = 0; i < 4; ++i) {
    os << str[i] << ":" << pTotal[i]
       << "=" << pBranches[i][0]
       << "+" << pBranches[i][1]
       << "+" << pBranches[i][2]
       << " ";
  }
}

std::ostream& operator<<(std::ostream& os, const BranchDesc& rhs) {
  if(rhs.m_pRect->m_bRotated) {
    os << rhs.m_pRect->m_nWidth
       << "x" << rhs.m_pRect->m_nHeight << ": ";
    rhs.print(rhs.m_pRTotal, rhs.m_pRBranches, os);
    os << std::endl;
    os << rhs.m_pRect->m_nHeight
       << "x" << rhs.m_pRect->m_nWidth << ": ";
    rhs.print(rhs.m_pTotal, rhs.m_pBranches, os);
    os << std::endl;
  }
  else {
    os << rhs.m_pRect->m_nWidth
       << "x" << rhs.m_pRect->m_nHeight << ": ";
    rhs.print(rhs.m_pTotal, rhs.m_pBranches, os);
    os << std::endl;
    os << rhs.m_pRect->m_nHeight
       << "x" << rhs.m_pRect->m_nWidth << ": ";
    rhs.print(rhs.m_pRTotal, rhs.m_pRBranches, os);
    os << std::endl;
  }
  return(os);
}
