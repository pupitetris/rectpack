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

#include "Component.h"
#include "EmptyCorner.h"
#include "EmptySpace.h"
#include <iostream>

EmptySpace::EmptySpace() :
  m_nMaxHeight(0),
  m_nHeightLeft(0) {
}

EmptySpace::~EmptySpace() {
}

void EmptySpace::print() const {
  std::cout << m_nHeightLeft << "/" << m_nMaxHeight;
  if(!m_Instantiations.empty())
    std::cout << ":";
  for(std::set<Component*>::const_iterator i = m_Instantiations.begin();
      i != m_Instantiations.end(); ++i)
    std::cout << " " << (*i)->m_nID;
  std::cout << std::flush;
}

UInt EmptySpace::maxNonAdjacentHeight(const EmptyCorner& ec) const {
  UInt nHeight = m_nHeightLeft;
  for(std::set<Component*>::const_iterator i = m_Instantiations.begin();
      i != m_Instantiations.end(); ++i) {
    if(ec.m_nType == EmptyCorner::LowerLeft ||
       ec.m_nType == EmptyCorner::LowerRight) {
      if((*i)->y2() == (UInt) ec.m_Location.y)
	return(0);
      if((*i)->m_nY > ec.m_Location.y)
	nHeight = std::min(nHeight, (UInt) ((*i)->m_nY - ec.m_Location.y - 1));
    }
    else {
      if((*i)->m_nY == ec.m_Location.y + 1)
	return(0);
      if((*i)->m_nY < ec.m_Location.y)
	nHeight = std::min(nHeight, (UInt) (ec.m_Location.y - (*i)->y2()));
    }
  }
  return(nHeight);
}
