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
#include "Rectangle.h"
#include "RectPtrArray.h"
#include <iostream>
#include <iomanip>

Component::Component() :
  m_nID(0),
  m_nX(0),
  m_nY(0),
  m_bEmpty(false),
  m_bTopLevel(true),
  m_pRect(NULL),
  m_pEmpty(NULL) {
}

Component::~Component() {
  if(m_pEmpty)
    delete m_pEmpty;
}

void Component::print() const {
  std::cout << "id=" << (Int) m_nID << " "
	    << "@ " << "(" << m_nX << "," << m_nY << "), "
	    << "WxH=" << m_Dims.m_nWidth << "x" << m_Dims.m_nHeight << ", "
	    << "empty=" << (m_bEmpty ? "y" : "n") << ", "
	    << "top=" << (m_bTopLevel ? "y" : "n");
  if(!m_Members.empty()) {
    std::set<Component*>::const_iterator i = m_Members.begin();
    std::cout << ", contains={" << (*i)->m_nID;
    for(++i; i != m_Members.end(); ++i)
      std::cout << "," << (*i)->m_nID;
    std::cout << "}";
  }
  if(m_pEmpty) {
    std::cout << ", template=";
    m_pEmpty->print();
  }
  std::cout << std::flush;
}

bool Component::yOverlaps(const Component& c) const {
  return((m_nY < (Int) (c.m_nY + c.m_Dims.m_nHeight)) &&
	 (c.m_nY < (Int) (m_nY + m_Dims.m_nHeight)));
}

bool Component::xOverlaps(const Component& c) const {
  return((m_nX < (Int) (c.m_nX + c.m_Dims.m_nWidth)) &&
	 (c.m_nX < (Int) (m_nX + m_Dims.m_nWidth)));
}

bool Component::sameDims(const Component& c) const {
  return(m_Dims == c.m_Dims);
}

void Component::assignX(const EmptyCorner& ec) {
  if(ec.m_nType == EmptyCorner::UpperLeft ||
     ec.m_nType == EmptyCorner::LowerLeft)
    m_nX = ec.m_Location.x;
  else
    m_nX = ec.m_Location.x + 1 - m_Dims.m_nWidth;
}

void Component::assignY(const EmptyCorner& ec) {
  if(ec.m_nType == EmptyCorner::LowerLeft ||
     ec.m_nType == EmptyCorner::LowerRight)
    m_nY = ec.m_Location.y;
  else
    m_nY = ec.m_Location.y + 1 - m_Dims.m_nHeight;
}

void Component::yOverlap(const Component& c, UInt& nStart,
			 UInt& nEnd) const {
  nStart = std::max(m_nY, c.m_nY);
  nEnd = std::min(m_nY + m_Dims.m_nHeight,
		  c.m_nY + c.m_Dims.m_nHeight);
}

void Component::instantiate() {
  if(m_bEmpty || m_Members.empty())
    return;
  if(m_Dims.m_nWidth == (*m_Members.begin())->m_Dims.m_nWidth) {
    UInt nY(m_nY);
    for(std::set<Component*>::iterator i = m_Members.begin();
	i != m_Members.end(); ++i) {
      (*i)->m_nX = m_nX;
      (*i)->m_nY = nY;
      nY = (*i)->y2();
      (*i)->instantiate();
    }
  }
  else {
    UInt nX(m_nX);
    for(std::set<Component*>::iterator i = m_Members.begin();
	i != m_Members.end(); ++i) {
      (*i)->m_nY = m_nY;
      (*i)->m_nX = nX;
      nX = (*i)->x2();
      (*i)->instantiate();
    }
  }
}

void Component::instantiateX() {
  if(m_bEmpty || m_Members.empty())
    return;
  if(m_Dims.m_nWidth == (*m_Members.begin())->m_Dims.m_nWidth)
    for(std::set<Component*>::iterator i = m_Members.begin();
	i != m_Members.end(); ++i) {
      (*i)->m_nX = m_nX;
      (*i)->instantiateX();
    }
  else {
    UInt nX(m_nX);
    for(std::set<Component*>::iterator i = m_Members.begin();
	i != m_Members.end(); ++i) {
      (*i)->m_nX = nX;
      nX = (*i)->x2();
      (*i)->instantiateX();
    }
  }
}

void Component::instantiateY() {
  if(m_bEmpty || m_Members.empty())
    return;
  if(m_Dims.m_nWidth == (*m_Members.begin())->m_Dims.m_nWidth) {
    UInt nY(m_nY);
    for(std::set<Component*>::iterator i = m_Members.begin();
	i != m_Members.end(); ++i) {
      (*i)->m_nY = nY;
      nY = (*i)->y2();
      (*i)->instantiateY();
    }
  }
  else
    for(std::set<Component*>::iterator i = m_Members.begin();
	i != m_Members.end(); ++i) {
      (*i)->m_nY = m_nY;
      (*i)->instantiateY();
    }
}

bool Component::checkMembers() const {
  if(m_Members.size() < 2) return(false);
  for(std::set<Component*>::const_iterator i = m_Members.begin();
      i != m_Members.end(); ++i) {
    if(*i == NULL) return(false);
    if((*i)->m_nID >= m_nID) return(false);
  }
  return(true);
}

bool Component::fills(const EmptyCorner& ec) const {
  switch(ec.m_nType) {
  case EmptyCorner::UpperLeft:
    return(ec.m_Location.x == m_nX &&
	   ec.m_Location.y == (Int) (m_nY + m_Dims.m_nHeight - 1));
  case EmptyCorner::LowerLeft:
    return(ec.m_Location.x == m_nX &&
	   ec.m_Location.y == m_nY);
  case EmptyCorner::LowerRight:
    return(ec.m_Location.x == (Int) (m_nX + m_Dims.m_nWidth - 1) &&
	   ec.m_Location.y == m_nY);
  case EmptyCorner::UpperRight:
    return(ec.m_Location.x == (Int) (m_nX + m_Dims.m_nWidth - 1) &&
	   ec.m_Location.y == (Int) (m_nY + m_Dims.m_nHeight - 1));
  };
  return(false);
}

UInt Component::y2() const {
  return(m_nY + m_Dims.m_nHeight);
}

UInt Component::x2() const {
  return(m_nX + m_Dims.m_nWidth);
}

bool Component::contains(const Rectangle* r) const {
  if(m_pRect == r)
    return(true);
  else
    for(std::set<Component*>::const_iterator i = m_Members.begin();
	i != m_Members.end(); ++i)
      if((*i)->contains(r))
	return(true);
  return(false);
}

const Rectangle* Component::getFirstRect() const {
  if(m_pRect)
    return(m_pRect);
  const Rectangle* pReturn(NULL);
  for(std::set<Component*>::const_iterator i = m_Members.begin();
      i != m_Members.end(); ++i) {
    pReturn = (*i)->getFirstRect();
    if(pReturn)
      return(pReturn);
  }
  return(NULL);
}
