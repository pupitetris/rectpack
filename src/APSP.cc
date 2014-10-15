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

#include "APSP.h"
#include "MetaVarDesc.h"
#include "Rectangle.h"
#include <iomanip>
#include <iostream>
#include <limits>

APSP::APSP() {
}

APSP::APSP(const APSP& src) :
  std::vector<std::vector<Int> >(src) {
}

APSP::~APSP() {
}

void APSP::initialize(size_t nRectangles) {
  clear();

  /**
   * @TODO We can probably just avoid using infinity and set the
   * initial values to the largest dimension of a
   * rectangle. Non-overlap constraints are enforced by the assertion
   * of negative values anyway.
   */

  resize(nRectangles,
	 std::vector<Int>(nRectangles, 
			  std::numeric_limits<Int>::max()));
  for(size_t i = 0; i < nRectangles; ++i)
    (*this)[i][i] = 0;
}

void APSP::floydWarshall() {
  for(size_t k = 0; k < size(); ++k)
    for(size_t i = 0; i < size(); ++i)
      for(size_t j = 0; j < size(); ++j)
	if(operator[](i)[k] < std::numeric_limits<Int>::max() &&
	   operator[](k)[j] < std::numeric_limits<Int>::max())
	  operator[](i)[j] =
	    std::min(operator[](i)[j],
		     operator[](i)[k] +
		     operator[](k)[j]);
}

void APSP::floydWarshall(size_t k) {
  for(size_t i = 0; i < size(); ++i)
    for(size_t j = 0; j < size(); ++j)
      if(operator[](i)[k] < std::numeric_limits<Int>::max() &&
	 operator[](k)[j] < std::numeric_limits<Int>::max())
	operator[](i)[j] =
	  std::min(operator[](i)[j],
		   operator[](i)[k] +
		   operator[](k)[j]);
}

bool APSP::negativeCycles() const {
  for(size_t i = 0; i < size(); ++i)
    if(operator[](i)[i] < 0)
      return(true);
  return(false);
}

void APSP::update(const Rectangle* r1,
		  const Rectangle* r2, Int n) {
  (*this)[r1->m_nID][r2->m_nID] =
    std::min((*this)[r1->m_nID][r2->m_nID], n);
}

void APSP::assign(const MetaVarDesc* pDesc, int nValue) {
  switch(nValue) {
  case MetaDomain::ABOVE:
    update(pDesc->m_pRect1, pDesc->m_pRect2,
	   - (Int) pDesc->m_pRect2->m_nHeight);
    break;
  case MetaDomain::RIGHTOF:
    update(pDesc->m_pRect1, pDesc->m_pRect2,
	   - (Int) pDesc->m_pRect2->m_nWidth);
    break;
  case MetaDomain::BELOW:
    update(pDesc->m_pRect2, pDesc->m_pRect1,
	   - (Int) pDesc->m_pRect1->m_nHeight);
    break;
  case MetaDomain::LEFTOF:
    update(pDesc->m_pRect2, pDesc->m_pRect1,
	   - (Int) pDesc->m_pRect1->m_nWidth);
    break;
  default:
    std::cout << "Oops, did you really mean to assign UNASSIGNED to the APSP matrix?"
	      << std::endl;
    exit(0);
    break;
  };
}

void APSP::negate(const MetaVarDesc* pDesc, int nValue) {
  switch(nValue) {
  case MetaDomain::ABOVE:
    update(pDesc->m_pRect2, pDesc->m_pRect1,
	   (Int) pDesc->m_pRect1->m_nHeight - 1);
    break;
  case MetaDomain::RIGHTOF:
    update(pDesc->m_pRect2, pDesc->m_pRect1,
	   (Int) pDesc->m_pRect1->m_nWidth - 1);
    break;
  case MetaDomain::BELOW:
    update(pDesc->m_pRect1, pDesc->m_pRect2,
	   (Int) pDesc->m_pRect2->m_nHeight - 1);
    break;
  case MetaDomain::LEFTOF: // Assert 1 right or on the same level as 2.
    update(pDesc->m_pRect1, pDesc->m_pRect2,
	   (Int) pDesc->m_pRect2->m_nWidth - 1);
    break;
  default:
    std::cout << "Oops, did you really mean to assign UNASSIGNED to the APSP matrix?"
	      << std::endl;
    exit(0);
    break;
  };
}

Int APSP::minWidth(const MetaVarDesc* pDesc) const {
  if((*this)[pDesc->m_pRect1->m_nID][pDesc->m_pRect2->m_nID] <
     (*this)[pDesc->m_pRect2->m_nID][pDesc->m_pRect1->m_nID]) {
    Int nNegB = (*this)[pDesc->m_pRect1->m_nID][pDesc->m_pRect2->m_nID];
    if(nNegB == std::numeric_limits<Int>::max())
      return(std::numeric_limits<Int>::max());
    return((Int) pDesc->m_pRect1->m_nWidth - nNegB);
  }
  else {
    Int nNegB = (*this)[pDesc->m_pRect2->m_nID][pDesc->m_pRect1->m_nID];
    if(nNegB == std::numeric_limits<Int>::max())
      return(std::numeric_limits<Int>::max());
    return((Int) pDesc->m_pRect2->m_nWidth - nNegB);
  }
}

Int APSP::minHeight(const MetaVarDesc* pDesc) const {
  if((*this)[pDesc->m_pRect1->m_nID][pDesc->m_pRect2->m_nID] <
     (*this)[pDesc->m_pRect2->m_nID][pDesc->m_pRect1->m_nID]) {
    Int nNegB = (*this)[pDesc->m_pRect1->m_nID][pDesc->m_pRect2->m_nID];
    if(nNegB == std::numeric_limits<Int>::max())
      return(std::numeric_limits<Int>::max());
    return((Int) pDesc->m_pRect1->m_nHeight - nNegB);
  }
  else {
    Int nNegB = (*this)[pDesc->m_pRect2->m_nID][pDesc->m_pRect1->m_nID];
    if(nNegB == std::numeric_limits<Int>::max())
      return(std::numeric_limits<Int>::max());
    return((Int) pDesc->m_pRect2->m_nHeight - nNegB);
  }
}

std::ostream& operator<<(std::ostream& os, const APSP& rhs) {
  std::vector<size_t> v;
  rhs.columnWidths(v);
  os << std::setw(2) << " ";
  for(size_t i = 0; i < v.size(); ++i)
    os << std::setw(v[i] + 1) << i;
  os << std::endl;
  for(size_t i = 0; i < rhs.size(); ++i) {
    os << std::setw(2) << i;
    for(size_t j = 0; j < rhs[i].size(); ++j) {
      if(rhs[i][j] == std::numeric_limits<Int>::max())
	os << std::setw(v[j] + 1) << "inf";
      else
	os << std::setw(v[j] + 1) << rhs[i][j];
    }
    os << std::endl;
  }
  return(os);
}

size_t APSP::width(const Int& n) const {
  std::ostringstream oss;
  if(n == std::numeric_limits<Int>::max())
    return(3);
  else {
    oss << n;
    return(oss.str().size());
  }
}

void APSP::columnWidths(std::vector<size_t>& v) const {
  v.clear();
  if(empty())
    return;
  v.resize(front().size(), 0);
  for(size_t i = 0; i < size(); ++i)
    for(size_t j = 0; j < operator[](i).size(); ++j)
      v[j] = std::max(v[j], width(operator[](i)[j]));
}

void APSP::print() const {
  std::cout << *this << std::flush;
}
