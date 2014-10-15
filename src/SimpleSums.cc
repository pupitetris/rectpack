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
#include "HeightWidth.h"
#include "Rectangle.h"
#include "SimpleSums.h"
#include "WidthHeight.h"
#include <iostream>

SimpleSums::SimpleSums() {
}

SimpleSums::~SimpleSums() {
}

void SimpleSums::initializeInts(UInt nMin, UInt nMax) {
  clear();
  while(nMin <= nMax) {
    insert(nMin);
    ++nMin;
  }
}

void SimpleSums::initialize(RectArray::const_iterator iBegin,
			    RectArray::const_iterator iEnd,
			    const DimsFunctor* pDims) {

  clear();
  insert(0);
  for(RectArray::const_iterator i = iBegin; i != iEnd; ++i) {
    const Rectangle* pRect = &(*i);
    SimpleSums mNew(*this);
    for(iterator j = begin(); j != end(); ++j)
      mNew.insert(*j + pDims->d1(pRect));
    if(pRect->rotatable())
      for(iterator j = begin(); j != end(); ++j)
	mNew.insert(*j + pDims->d2(*i));
    insert(mNew.begin(), mNew.end());
  }
}

void SimpleSums::add(const UInt& n) {
  SimpleSums mNew;
  for(iterator j = begin(); j != end(); ++j) {
    UInt nNew = *j + n;
    mNew.insert(mNew.end(), nNew);
  }
  insert(mNew.begin(), mNew.end());
}

void SimpleSums::add(const UInt& n, const UInt& nMax) {
  SimpleSums mNew;
  for(iterator j = begin(); j != end(); ++j) {
    UInt nNew = *j + n;
    if(nNew > nMax)
      break;
    mNew.insert(mNew.end(), nNew);
  }
  insert(mNew.begin(), mNew.end());
}

void SimpleSums::add(const UInt& n1, const UInt& n2,
		     const UInt& nMax) {
  SimpleSums mNew;
  for(iterator j = begin(); j != end(); ++j) {
    UInt nNew1 = *j + n1;
    UInt nNew2 = *j + n2;
    if(nNew1 <= nMax)
      mNew.insert(mNew.end(), nNew1);
    if(nNew2 <= nMax)
      mNew.insert(mNew.end(), nNew2);
    if(nNew1 > nMax && nNew2 > nMax)
      break;
  }
  insert(mNew.begin(), mNew.end());
}

void SimpleSums::add(const Compulsory& nValue, const UInt& nMax) {
  iterator j(lower_bound(nValue.m_nStart.m_nLeft));
  while(j != end() && *j <= nValue.m_nStart.m_nRight) {
    UInt nSum = *j + nValue.m_pRect->m_nWidth;
    if(nSum > nMax)
      break;
    insert(nSum);
    ++j;
  }
}

void SimpleSums::initializeW(RectArray::const_iterator iBegin,
			     RectArray::const_iterator iEnd) {
  WidthHeight wh;
  initialize(iBegin, iEnd, &wh);
}

void SimpleSums::initializeH(RectArray::const_iterator iBegin,
			     RectArray::const_iterator iEnd) {
  HeightWidth hw;
  initialize(iBegin, iEnd, &hw);
}

void SimpleSums::print() const {
  for(const_iterator i = begin(); i != end(); ++i)
    std::cout << *i << " ";
  std::cout << std::endl;
}

void SimpleSums::print(const const_iterator& i) const {
  std::cout << "Sum: ";
  if(i == end())
    std::cout << "n/a" << std::endl;
  else
    std::cout << *i << std::endl;
}
