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
#include "ComponentPtrs.h"
#include "EmptySpace.h"
#include "Rectangle.h"
#include <assert.h>

ComponentPtrs::ComponentPtrs() {
}

ComponentPtrs::~ComponentPtrs() {
  for(iterator i = begin(); i != end(); ++i)
    delete *i;
}

void ComponentPtrs::clear() {
  for(iterator i = begin(); i != end(); ++i)
    delete *i;
  std::deque<Component*>::clear();
}

void ComponentPtrs::getCumulativeProfile(std::map<UInt, UInt>& m) const {
  for(const_iterator i = begin(); i != end(); ++i) {
    const Component* c(*i);
    if(c->m_bTopLevel) {
      std::pair<std::map<UInt, UInt>::iterator, bool> j =
	m.insert(std::pair<UInt, UInt>(c->m_nX, 0));

      /**
       * If the insertion created a new point then we need to
       * initialize the height appropriately either by setting it to 0
       * or by copying it over from a previous height.
       */

      if(j.second && j.first != m.begin()) {
	std::map<UInt, UInt>::iterator k = j.first;
	--k;
	j.first->second = k->second;
      }
      
      /**
       * Do the same thing for the end point.
       */

      std::pair<std::map<UInt, UInt>::iterator, bool> k =
	m.insert(std::pair<UInt, UInt>(c->x2(), 0));
      if(k.second && k.first != m.begin()) {
	std::map<UInt, UInt>::iterator l = k.first;
	--l;
	k.first->second = l->second;
      }

      /**
       * Now for all control points from the starting iterator to the
       * ending iterator, we'll add in the height.
       */

      for(; j.first != k.first; ++j.first)
	j.first->second += c->m_Dims.m_nHeight;
    }
  }
}

void ComponentPtrs::print() const {
  for(const_iterator i = begin(); i != end(); ++i) {
    (*i)->print();
    std::cout << std::endl;
  }
}

void ComponentPtrs::instantiate() {
  for(iterator i = begin(); i != end(); ++i)
    if((*i)->m_bTopLevel)
      (*i)->instantiate();
}

void ComponentPtrs::instantiateY() {
  for(iterator i = begin(); i != end(); ++i)
    if((*i)->m_bTopLevel)
      (*i)->instantiateY();
}

void ComponentPtrs::instantiateX() {
  for(iterator i = begin(); i != end(); ++i)
    if((*i)->m_bTopLevel)
      (*i)->instantiateX();
}

void ComponentPtrs::instantiate(const RectPtrArray::iterator& iBegin,
				const RectPtrArray::const_iterator& iEnd) {
  instantiate();
  for(RectPtrArray::iterator i = iBegin; i != iEnd; ++i) {
    (*i)->x = operator[]((*i)->m_nID)->m_nX;
    (*i)->y = operator[]((*i)->m_nID)->m_nY;
  }
}

void ComponentPtrs::instantiateX(const RectPtrArray::iterator& iBegin,
				 const RectPtrArray::const_iterator& iEnd) {
  instantiateX();
  for(RectPtrArray::iterator i = iBegin; i != iEnd; ++i) {
    (*i)->x = operator[]((*i)->m_nID)->m_nX;
    (*i)->y = operator[]((*i)->m_nID)->m_nY;
  }
}

void ComponentPtrs::instantiateY(const RectPtrArray::iterator& iBegin,
				 const RectPtrArray::const_iterator& iEnd) {
  instantiateY();
  for(RectPtrArray::iterator i = iBegin; i != iEnd; ++i) {
    (*i)->x = operator[]((*i)->m_nID)->m_nX;
    (*i)->y = operator[]((*i)->m_nID)->m_nY;
  }
}

Component* ComponentPtrs::newEmptyUnit(const UInt& nX) {
  Component* pNew = new Component();
  pNew->m_nID = size();
  pNew->m_Dims.initialize(1, 1);
  pNew->m_nX = nX;
  pNew->m_bEmpty = true;
  push_back(pNew);
  return(pNew);
}

Component* ComponentPtrs::newEmptyTemplateX(const UInt& nX, const UInt& nWidth,
					    const UInt& nMaxHeight) {
  Component* pNew = new Component();
  pNew->m_nID = size();
  pNew->m_Dims.initialize(nWidth, 1);
  pNew->m_nX = nX;
  pNew->m_bEmpty = true;
  pNew->m_pEmpty = new EmptySpace();
  pNew->m_pEmpty->m_nMaxHeight = nMaxHeight;
  pNew->m_pEmpty->m_nHeightLeft = nMaxHeight;
  push_back(pNew);
  return(pNew);
}

Component* ComponentPtrs::newEmptyComponentX(const UInt& nX,
					     const UInt& nWidth,
					     const UInt& nHeight) {
  Component* pNew = new Component();
  pNew->m_nID = size();
  pNew->m_Dims.initialize(nWidth, nHeight);
  pNew->m_nX = nX;
  pNew->m_bEmpty = true;
  push_back(pNew);
  return(pNew);
}

Component* ComponentPtrs::combineHorizontally(Component* p1, Component* p2) {
  Component* pNew = new Component();
  pNew->m_nID = size();
  pNew->m_Dims.initialize(p1->m_Dims.m_nWidth + p2->m_Dims.m_nWidth,
			  p1->m_Dims.m_nHeight);
  pNew->m_nX = std::min(p1->m_nX, p2->m_nX);
  pNew->m_bEmpty = p1->m_bEmpty && p2->m_bEmpty;
  pNew->m_Members.insert(p1);
  pNew->m_Members.insert(p2);
  p1->m_bTopLevel = false;
  p2->m_bTopLevel = false;
  push_back(pNew);
  return(pNew);
}

Component* ComponentPtrs::extendRight(Component* c, const UInt& nX) {
  Component* pNewEmpty =
    newEmptyComponentX(c->x2(), nX - c->x2(),
		       c->m_Dims.m_nHeight);
  Component* pNew = combineHorizontally(c, pNewEmpty);
  return(pNew);
}

Component* ComponentPtrs::forkEmptyX(Component* pEmpty, const UInt& nHeight) {
  Component* pNew = newEmptyComponentX(pEmpty->m_nX,
				       pEmpty->m_Dims.m_nWidth,
				       nHeight);
  pEmpty->m_pEmpty->m_nHeightLeft -= nHeight;
  pEmpty->m_pEmpty->m_Instantiations.insert(pNew);
  return(pNew);
}

void ComponentPtrs::unforkEmptyX(Component* pEmpty, Component* pForked) {
  pEmpty->m_pEmpty->m_nHeightLeft += pForked->m_Dims.m_nHeight;
  pEmpty->m_pEmpty->m_Instantiations.erase(pForked);
  assert(pForked->m_nID == back()->m_nID);
  delete back();
  pop_back();
}

Component* ComponentPtrs::contains(const Rectangle* r) {
  for(const_iterator i = begin(); i != end(); ++i)
    if((*i)->m_bTopLevel)
      if((*i)->contains(r))
	return(*i);
  return(NULL);
}
