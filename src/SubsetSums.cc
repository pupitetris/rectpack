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

#include "HeightWidth.h"
#include "Rectangle.h"
#include "SubsetSums.h"
#include "WidthHeight.h"
#include <iostream>
#include <list>

SubsetSums::SubsetSums() {
}

SubsetSums::~SubsetSums() {
}

void SubsetSums::intersect(SubsetSums::data_type& a,
			   const SubsetSums::data_type& b) {

  /**
   * Since we can't delete our iterators while we're iterating, we
   * keep a delete list and process this after we're done scanning.
   */

  std::list<SubsetSums::data_type::iterator> lDelete;

  /**
   * Loop over each rectangle in the destination subset sum map. Since
   * we're performing intersection we can limit our purview to this
   * set (or the other one).
   */

  for(SubsetSums::data_type::iterator i = a.begin();
      i != a.end(); ++i) {

    /**
     * For each rectangle, find the corresponding rectangle in the
     * read-only set.
     */

    SubsetSums::data_type::const_iterator j =
      b.find(i->first);

    /**
     * If the rectangle doesn't exist in both maps, and the rectangle
     * also doesn't exist in the current rectangle (the one that is
     * supposed to implicitly augment the new subset sum), then we
     * will be deleting this rectangle.
     */

    if(j == b.end())
      lDelete.push_back(i);
    else if(i->second != j->second)
	i->second = 2;
  }
  while(!lDelete.empty()) {
    a.erase(lDelete.back());
    lDelete.pop_back();
  }
}

void SubsetSums::initializeInts(UInt nMin, UInt nMax) {
  clear();
  while(nMin <= nMax) {
    insert(value_type(nMin, data_type()));
    ++nMin;
  }
}

void SubsetSums::initialize(RectArray::const_iterator iBegin,
			    RectArray::const_iterator iEnd,
			    const DimsFunctor* pDims) {

  clear();
  insert(value_type(0, data_type()));
  for(RectArray::const_iterator i = iBegin; i != iEnd; ++i) {

    const Rectangle* pRect = &(*i);

    /**
     * Process the first dimension. Iterator j points to the current
     * subset sum, while pMap points to the new value being created by
     * the addition of rectangle i.
     */

    SubsetSums mNew(*this);
    for(iterator j = begin(); j != end(); ++j) {
      UInt nNewKey = j->first + pDims->d1(pRect);
      iterator iMap = mNew.find(nNewKey);
      if(iMap == mNew.end()) {
	data_type* pMap = &mNew[nNewKey];
	pMap->insert(j->second.begin(), j->second.end());
	data_type::iterator k = pMap->find(pRect);
	if(k == pMap->end())
	  (*pMap)[pRect] = 0;
	else if(k->second == 1)
	  k->second = 2;
      }
      else {
	(iMap->second)[pRect] = 0;
	intersect(iMap->second, j->second);
      }
    }

    /**
     * Process its rotation if applicable.
     */

    if(pRect->rotatable())
      for(iterator j = begin(); j != end(); ++j) {
	UInt nNewKey = j->first + pDims->d2(*i);
	iterator iMap = mNew.find(nNewKey);
	if(iMap == mNew.end()) {
	  data_type* pMap = &mNew[nNewKey];
	  pMap->insert(j->second.begin(), j->second.end());
	  data_type::iterator k = pMap->find(pRect);
	  if(k == pMap->end())
	    (*pMap)[pRect] = 1;
	  else if(k->second == 0)
	    k->second = 2;
	}
	else {
	  (iMap->second)[pRect] = 1;
	  intersect(iMap->second, j->second);
	}
      }
    
    /**
     * Now insert all of our new entries into the current set. We
     * can't call the regular insertion operator because it does not
     * insert if the key already exists.
     */

    for(const_iterator j = mNew.begin(); j != mNew.end(); ++j)
      (*this)[j->first] = j->second;
  }
}

void SubsetSums::computeEpsilon() {
  m_nEpsilon = rbegin()->first;
  const_iterator iPrev = begin();
  const_iterator iNext = iPrev; ++iNext;
  for(; iNext != end(); ++iNext) {
    m_nEpsilon = std::min(m_nEpsilon, iNext->first - iPrev->first);
    iPrev = iNext;
  }
}

void SubsetSums::initializeW(RectArray::const_iterator iBegin,
			     RectArray::const_iterator iEnd) {
  WidthHeight wh;
  initialize(iBegin, iEnd, &wh);
  computeEpsilon();
}

void SubsetSums::initializeH(RectArray::const_iterator iBegin,
			     RectArray::const_iterator iEnd) {
  HeightWidth hw;
  initialize(iBegin, iEnd, &hw);
  computeEpsilon();
}

void SubsetSums::print() const {
  for(const_iterator i = begin(); i != end(); ++i)
    std::cout << i->first << " ";
  std::cout << std::endl;
}

void SubsetSums::print(UInt n) const {
  const_iterator i = find(n);
  if(i != end()) print(i);
}

void SubsetSums::print(const const_iterator& i) const {
  if(i == end())
    std::cout << "At end." << std::endl;
  else {
    std::cout << "Sum: " << i->first << std::endl;
    std::cout << "Mutex set:" << std::endl;
    i->second.print();
  }
}

void SubsetSums::print(const iterator& i) const {
  if(i == end())
    std::cout << "At end." << std::endl;
  else {
    std::cout << "Sum: " << i->first << std::endl;
    std::cout << "Mutex set:" << std::endl;
    i->second.print();
  }
}
