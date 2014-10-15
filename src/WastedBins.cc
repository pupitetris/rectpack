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

#include "Bins.h"
#include "BoxDimensions.h"
#include "Component.h"
#include "ComponentPtrs.h"
#include "Parameters.h"
#include "RectSetMinDim.h"
#include "WastedBins.h"
#include <assert.h>
#include <iostream>
#include <iomanip>

WastedBins::WastedBins() :
  m_bEnabled(true) {
}

WastedBins::WastedBins(UInt i, Int j) :
  std::vector<Int>(i, j),
  m_bEnabled(true) {
}

WastedBins::WastedBins(const BoxDimensions& b) :
  std::vector<Int>(b.m_nWidth + 1, 0),
  m_bEnabled(true) {
  back() = b.m_nArea;
}

void WastedBins::initialize(const Parameters* pParams) {
  m_bEnabled = (pParams->m_s1.find_first_of('w') >= pParams->m_s1.size());
}

void WastedBins::initialize(const BoxDimensions& b) {
  clear();
  resize(b.m_nWidth + 1, 0);
  back() = b.m_nArea;
}

WastedBins::~WastedBins() {
}

const WastedBins& WastedBins::operator=(const WastedBins& src) {
  std::vector<Int>::operator=(src);
  m_bEnabled = src.m_bEnabled;
  return(*this);
}

bool WastedBins::canFitw(RectPtrArray::const_iterator iStart,
			 const RectPtrArray::const_iterator& iEnd) const {
  if(m_bEnabled) {
    Int nArea(0), nSpace(0);
    for(int j = size() - 1; j >= 0; --j) {
      nSpace += operator[](j);
      for(; iStart != iEnd && (Int) (*iStart)->m_nWidth >= j; ++iStart)
	nArea += (*iStart)->m_nArea;
      if(nSpace < nArea)
	return(false);
    }
  }
  return(true);
}

bool WastedBins::canFitw(RectPtrArray::const_reverse_iterator iStart,
			 const RectPtrArray::const_reverse_iterator& iEnd) const {
  if(m_bEnabled) {
    Int nArea(0), nSpace(0);
    Int j = size() - 1;
    for(; iStart != iEnd; ++iStart) {
      nArea += (*iStart)->m_nArea;
      for(; j >= (Int) (*iStart)->m_nWidth; --j)
	nSpace += operator[](j);
      if(nSpace < nArea)
	return(false);
    }
  }
  return(true);
}

bool WastedBins::canFitw(RectPtrArray::const_reverse_iterator iStart,
			 const RectPtrArray::const_reverse_iterator& iEnd,
			 UInt& r) const {
  if(m_bEnabled) {
    Int nArea(0), nSpace(0);
    Int j = size() - 1;
    for(; iStart != iEnd; ++iStart) {
      nArea += (*iStart)->m_nArea;
      for(; j >= (Int) (*iStart)->m_nWidth; --j)
	nSpace += operator[](j);
      if(nSpace < nArea) {
	r = (*iStart)->m_nID;
	return(false);
      }
    }
  }
  return(true);
}

bool WastedBins::canFitw(RectPtrArray::const_iterator iStart,
			 const RectPtrArray::const_iterator& iEnd,
			 UInt& r, UInt& s) const {
  if(m_bEnabled) {
    Int nArea(0), nSpace(0);
    Int j = size() - 1;
    for(; iStart != iEnd; ++iStart) {
      nArea += (*iStart)->m_nArea;
      for(; j >= (Int) (*iStart)->m_nWidth; --j)
	nSpace += operator[](j);
      if(nSpace < nArea) {
	r = (*iStart)->m_nID;

	/**
	 * Now deduce which of the next rectangles could fit. First
	 * remove the previous rectangle.
	 */

	nArea -= (*iStart)->m_nArea;
	for(++iStart; iStart != iEnd; ++iStart) {
	  nArea += (*iStart)->m_nArea;
	  for(--j; j >= (Int) (*iStart)->m_nWidth; --j)
	    nSpace += operator[](j);
	  if(nSpace >= nArea) {
	    s = (*iStart)->m_nID;
	    return(false);
	  }
	}

	/**
	 * If we got here, then it means that we none of the remaining
	 * rectangles in the sequence can be assigned, so by default
	 * we'll return 0.
	 */

	s = 0;
	return(false);
      }
    }
  }
  return(true);
}

bool WastedBins::canFit(const RectSetMinDim& s) const {
  if(m_bEnabled) {
    Int nArea(0), nSpace(0);
    RectSetMinDim::const_iterator i = s.begin();
    for(int j = size() - 1; j >= 0; --j) {
      nSpace += operator[](j);
      for(; i != s.end() && (Int) (*i)->m_nMinDim >= j; ++i)
	nArea += (*i)->m_nArea;
      if(nSpace < nArea)
	return(false);
    }
  }
  return(true);
}

bool WastedBins::canFith(RectPtrArray::const_iterator iStart,
			 const RectPtrArray::const_iterator& iEnd) const {
  if(m_bEnabled) {
    Int nArea(0), nSpace(0);
    for(int j = size() - 1; j >= 0; --j) {
      nSpace += operator[](j);
      for(; iStart != iEnd && (Int) (*iStart)->m_nHeight >= j; ++iStart)
	nArea += (*iStart)->m_nArea;
      if(nSpace < nArea)
	return(false);
    }
  }
  return(true);
}

bool WastedBins::canFitm(RectPtrArray::const_iterator iStart,
			 const RectPtrArray::const_iterator& iEnd) const {
  if(m_bEnabled) {
    Int nArea(0), nSpace(0);
    for(int j = size() - 1; j >= 0; --j) {
      nSpace += operator[](j);
      for(; iStart != iEnd && (Int) (*iStart)->m_nMinDim >= j; ++iStart)
	nArea += (*iStart)->m_nArea;
      if(nSpace < nArea)
	return(false);
    }
  }
  return(true);
}

bool WastedBins::canFit(const ComponentPtrs& vComponents,
			const std::vector<bool>& vPlaced) const {
  if(m_bEnabled) {
    Int nArea(0), nSpace(0), j(size() - 1);
    for(ComponentPtrs::const_iterator i = vComponents.begin();
	i != vComponents.end(); ++i) {
      if(vPlaced[(*i)->m_nID] || !(*i)->m_bTopLevel) continue;
      nArea += (*i)->m_Dims.m_nArea;
      for(; j >= (Int) (*i)->m_Dims.m_nMinDim; --j)
	nSpace += operator[](j);
      if(nSpace < nArea)
	return(false);
    }
  }
  return(true);
}

bool WastedBins::canFitKorf(RectPtrArray::const_iterator iStart,
			    const RectPtrArray::const_iterator& iEnd,
			    Int nMaxWaste) const {
  if(!m_bEnabled) return(true);
  Int carryover(0);
  Int waste(0);
  Int diff(0);

  if(iStart == iEnd) return(false);
  RectPtrArray::const_iterator i = iEnd - 1;

  /**
   * Collect the waste from all squares smaller than our smallest
   * square.
   */

  Int j = 1;
  for(; j < (Int) (*i)->m_nMinDim; ++j)
    waste += operator[](j);

  /**
   * Begin the loop of placing and checking waste.
   */

  while(i != iStart) {

    /**
     * Place that next square.
     */
    
    if(operator[](j) > (Int) (*i)->m_nArea) {
      diff = operator[](j) - (*i)->m_nArea;
      if(carryover >= diff)
	carryover -= diff;
      else {
	waste += diff - carryover;
	carryover = 0;
      }
    }
    else if(operator[](j) < (Int) (*i)->m_nArea)
      carryover += (*i)->m_nArea - operator[](j);

    /**
     * Accumulate the wasted space up until the next square we have to
     * place.
     */

    --i;
    for(++j; j < (Int) (*i)->m_nMinDim; ++j)
      carryover -= operator[](j);
    if(carryover < 0) {// If it's negative it means we actually had empty space.
      waste -= carryover;
      carryover = 0;
    }
  };
  return(waste <= nMaxWaste);
}

bool WastedBins::canFitKorf(Int n, Int nMaxWaste) const {
  if(!m_bEnabled) return(true);
  Int square;                           /* size of next square to try to place */
  Int carryover;            /* total area of smaller strips available to place */
  Int waste;                                    /* total amount of wasted area */
  Int area;                               /* total area of a given size square */

  carryover = 0; /*initialize total area of smaller squares available to place */
  waste = 0;                        /* initialize total amount of wasted space */

  for(square = 1; square < n; square++) /*for each size square, except largest*/
    {area = square * square;                      /* area of this size square */
      if(operator[](square) > area) /* more cells than can be filled by this square */
	{if(operator[](square) - area <= carryover) /* available smaller squares can fill in */
	    carryover -= operator[](square) - area;   /* decrease excess by difference */
	  else     /* available smaller squares can't fill all of additional waste */
	    {waste += operator[](square) - area - carryover;      /* use all of excess */
	      carryover = 0;}}                                     /* no excess left */
      else if(operator[](square) < area)       /* less than a full square is needed */
	carryover += area - operator[](square);}  /* excess can offset further waste */

  if(waste > nMaxWaste) {
    return(false);               /* too much wasted space */
  }
  else {
    return(true);  
  }
}

void WastedBins::print() const {
  for(UInt i = 0; i < size(); ++i)
    std::cout << std::setw(3) << i << ": " << operator[](i)
	      << std::endl;
}

bool WastedBins::canFit() const {
  if(m_bEnabled) {
    Int nSum(0);
    for(const_reverse_iterator i = rbegin(); i != rend(); ++i)
      if((nSum += *i) < 0) {
	//std::cout << "pruned at " << rend() - i - 1 << std::endl;
	return(false);
      }
  }
  return(true);
}

void WastedBins::moveArea(UInt nOld, UInt nNew,
			  UInt nAmount) {
  operator[](nOld) += nAmount;
  operator[](nNew) -= nAmount;
}
