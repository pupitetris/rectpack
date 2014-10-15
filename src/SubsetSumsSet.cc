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

#include "SubsetSumsSet.h"

SubsetSumsSet::SubsetSumsSet() :
  m_pRects(NULL),
  m_pCached(NULL),
  m_nType(WIDTH) {
}

SubsetSumsSet::~SubsetSumsSet() {
}

void SubsetSumsSet::initialize(const RectArray* pRects,
			       const SubsetSums* pCached,
			       Orientation nType) {
  m_pRects = pRects;
  m_pCached = pCached;
  clear();
  resize(m_pRects->size());
  m_nType = nType;
}

const SubsetSums& SubsetSumsSet::operator[](const UInt& i) {
  
  /**
   * Should we return a cached set precomputed elsewhere?
   */


  if(i == size() - 1)
    return(*m_pCached);

  SubsetSums* pSums = &std::vector<SubsetSums>::operator[](i);
  if(pSums->empty()) {
    if(m_nType == WIDTH)
      pSums->initializeW(m_pRects->begin(),
			 m_pRects->begin() + i + 1);
    else
      pSums->initializeH(m_pRects->begin(),
			 m_pRects->begin() + i + 1);
    
  }
  return(*pSums);
}
