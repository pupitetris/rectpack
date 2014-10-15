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
#include <string>
#include <vector>

EmptyCorner::EmptyCorner() :
  m_nType(EmptyCorner::UpperLeft) {
}

EmptyCorner::EmptyCorner(Int x, Int y, Corner nType) :
  m_Location(x, y),
  m_nType(nType) {
  }

EmptyCorner::~EmptyCorner() {
}

void EmptyCorner::set(Int x, Int y, EmptyCorner::Corner nType) {
  m_Location.x = x;
  m_Location.y = y;
  m_nType = nType;
}

std::ostream& operator<<(std::ostream& os, const EmptyCorner& ec) {
  std::vector<std::string> v(4);
  v[0] = "ul"; v[1] = "ll"; v[2] = "lr"; v[3] = "ur";
  return(os << v[ec.m_nType] << '@' << ec.m_Location);
}

void EmptyCorner::constrainHeight(const UInt& nWidth, UInt& nHeight,
				  const Component* p) const {

  /**
   * For each of these cases, first check to see if the corner is out
   * of range, then check x-overlap, then check to see if a single ray
   * drawn out to the right intersects (inducing a height of 0), and
   * then finally constrain the height.
   */

  switch(m_nType) {

  case LowerLeft:
    if((UInt) m_Location.y >= p->y2())
      break;
    if((UInt) m_Location.x + nWidth <= (UInt) p->m_nX ||
       (UInt) m_Location.x >= p->x2())
      break;
    if((UInt) m_Location.y >= (UInt) p->m_nY &&
       (UInt) m_Location.y < p->y2()) {
      nHeight = 0;
      break;
    }
    nHeight = std::min(nHeight, (UInt) (p->m_nY - m_Location.y));
    break;

  case LowerRight:
    if((UInt) m_Location.y >= p->y2())
      break;
    if(m_Location.x < p->m_nX ||
       (UInt) m_Location.x + 1 - nWidth >= (UInt) p->x2())
      break;
    if(m_Location.y >= p->m_nY &&
       (UInt) m_Location.y < p->y2()) {
      nHeight = 0;
      break;
    }
    nHeight = std::min(nHeight, (UInt) (p->m_nY - m_Location.y));
    break;

  case UpperLeft:
    if(m_Location.y <= p->m_nY)
      break;
    if((UInt) m_Location.x + nWidth <= (UInt) p->m_nX ||
       (UInt) m_Location.x >= p->x2())
      break;
    if(m_Location.y >= p->m_nY &&
       (UInt) m_Location.y < p->y2()) {
      nHeight = 0;
      break;
    }
    nHeight = std::min(nHeight, (UInt) (m_Location.y + 1 - p->y2()));
    break;

  case UpperRight:
    if(m_Location.y <= p->m_nY)
      break;
    if(m_Location.x < p->m_nX ||
       (UInt) m_Location.x + 1 - nWidth >= p->x2())
      break;
    if(m_Location.y >= p->m_nY &&
       (UInt) m_Location.y < p->y2()) {
      nHeight = 0;
      break;
    }
    nHeight = std::min(nHeight, (UInt) (m_Location.y + 1 - p->y2()));
    break;
  };
}
