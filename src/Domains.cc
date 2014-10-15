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

#include "Domains.h"
#include "HeightFirst.h"
#include "Packer.h"
#include "Parameters.h"
#include "Rectangle.h"
#include "RectDecorator.h"
#include "RectPtrArray.h"
#include "WidthFirst.h"
#include <iomanip>

Domains::Domains() :
  m_bBreakTopSymmetry(true) {
}

Domains::~Domains() {
}

void Domains::initialize(Packer* pPacker) {
  m_pPacker = pPacker;
}

void Domains::initialize(const Parameters* pParams) {
  clear();
  resize(pParams->m_vInstance.size());
  for(iterator i = begin(); i != end(); ++i)
    i->resize(2, Oriented(4));
  if(pParams->m_s1.find_first_of('s') < pParams->m_s1.size())
    m_bBreakTopSymmetry = false;
}

void Domains::initialize(const BoxDimensions* pBox) {
  for(RectPtrArray::iterator i = m_pPacker->m_vRectPtrs.begin();
      i != m_pPacker->m_vRectPtrs.end(); ++i) {
    Rectangle* r(*i);
    for(size_t j = 0; j < 4; ++j)
      initialize(r, pBox, (*this)[r->m_nID][r->m_bRotated ? 1 : 0][j]);
    if(r->rotatable()) {
      r->rotate();
      for(size_t j = 0; j < 4; ++j)
	initialize(r, pBox, (*this)[r->m_nID][r->m_bRotated ? 1 : 0][j]);
      r->rotate();
    }
  }
}

void Domains::initialize(const Rectangle* r,
			 const BoxDimensions* pBox, DomConfig& v) {

  /**
   * Number of values we need to account for.
   */

  UInt nRange(pBox->m_nWidth - r->m_nWidth + 1);
  if(m_bBreakTopSymmetry && r->m_nID == 0 && nRange > 1)
    nRange /= 2;

  /**
   * The size of the interval for the given rectangle. It must be at
   * least one.
   */

  URational nInterval(r->scale() * (URational) r->m_nWidth);
  if(nInterval < (URational) 1) nInterval = (URational) 1;
  UInt nIntervalSize(nInterval.get_ui());

  /**
   * The theoretical number of branches is nDiv. Then we will decide
   * how many integral branches we'll actually have, which is nDiv
   * rounded up to the nearest integer.
   */

  UInt nBranches(nRange / nIntervalSize);
  if(nRange % nIntervalSize) ++nBranches;

  /**
   * The size of the interval per branch. This is the increment that
   * should be added.
   */

  v.clear();
  v.resize(nBranches);
  for(UInt i = 0; i < nBranches; ++i) {
    UInt nStart = i * nRange / nBranches;
    UInt nEnd = (i + 1) * nRange / nBranches;
    v[i].initialize(nStart, nEnd - 1, r);
  }
}

UInt Domains::intervalSize(const Rectangle* r, const UInt& nWidth) {
  UInt nRange(nWidth - r->m_nWidth + 1);
  if(m_bBreakTopSymmetry && r->m_nID == 0 && nRange > 1)
    nRange /= 2;
  URational nInterval(r->scale() * (URational) r->m_nWidth);
  if(nInterval < (URational) 1) nInterval = (URational) 1;
  return(nInterval.get_ui());
}

void Domains::initialize(const Rectangle* r,
			 const UInt& nWidth, DomConfig& v) {

  /**
   * Number of values we need to account for.
   */

  UInt nRange(nWidth - r->m_nWidth + 1);
  if(m_bBreakTopSymmetry && r->m_nID == 0 && nRange > 1)
    nRange /= 2;

  /**
   * The size of the interval for the given rectangle. It must be at
   * least one.
   */

  URational nInterval(r->scale() * (URational) r->m_nWidth);
  if(nInterval < (URational) 1) nInterval = (URational) 1;
  UInt nIntervalSize(nInterval.get_ui());

  /**
   * The theoretical number of branches is nDiv. Then we will decide
   * how many integral branches we'll actually have, which is nDiv
   * rounded up to the nearest integer.
   */

  UInt nBranches(nRange / nIntervalSize);
  if(nRange % nIntervalSize) ++nBranches;

  /**
   * The size of the interval per branch. This is the increment that
   * should be added.
   */

  v.clear();
  v.resize(nBranches);
  for(UInt i = 0; i < nBranches; ++i) {
    UInt nStart = i * nRange / nBranches;
    UInt nEnd = (i + 1) * nRange / nBranches;
    v[i].initialize(nStart, nEnd - 1, r);
  }
}

const Oriented& Domains::get(const Rectangle* r) const {
  return(operator[](r->m_nID)[r->m_bRotated ? 1 : 0]);
}

void Domains::print() const {
  std::cout << "ID   Dimensions    DomAvail   Intervals" << std::endl;
  for(size_t i = 0; i < size(); ++i) {
    std::cout << std::setiosflags(std::ios::right)
	      << std::setw(2) << i << " ";
    const Rectangle* pRect(NULL);
    if(!(*this)[i][0][0].empty())
      pRect = (*this)[i][0][0][0].m_pRect;
    else if(!(*this)[i][1][0].empty())
      pRect = (*this)[i][1][0][0].m_pRect;

    /**
     * Set up the appropriate rectangle decorators for convenience.
     */

    RectDecorator* pDec(NULL);
    WidthFirst wf(pRect);
    HeightFirst hf(pRect);

    /**
     * Iterate over the rotation.
     */

    for(size_t r = 0; r < 2; ++r) {
      if((r == 0 && !pRect->m_bRotated) ||
	 (r == 1 && pRect->m_bRotated))
	pDec = &wf;
      else
	pDec = &hf;

      if(r > 0) std::cout << "   ";
      std::ostringstream oss;
      std::cout << std::setw(7)
		<< std::resetiosflags(std::ios::adjustfield)
		<< std::setiosflags(std::ios::right)
		<< pDec->d1();
      std::cout << "x";
      std::cout << std::setw(7)
		<< std::resetiosflags(std::ios::adjustfield)
		<< std::setiosflags(std::ios::left)
		<< pDec->d2() << " ";
      
      for(size_t j = 0; j < 4; ++j) {
	if(j > 0) std::cout << "                   ";
	if(j == 0)      std::cout << "Left Right";
	else if(j == 1) std::cout << "Left      ";
	else if(j == 2) std::cout << "     Right";
	else if(j == 3) std::cout << "          ";
	
	for(DomConfig::const_iterator k = (*this)[i][r][j].begin();
	    k != (*this)[i][r][j].end(); ++k)
	  std::cout << " " << k->m_nStart;
	std::cout << std::endl;
      }
    }
  }
}
