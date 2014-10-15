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

#include "AlignedComponents.h"
#include "Component.h"
#include "ComponentPtrs.h"

AlignedComponents::AlignedComponents() {
}

AlignedComponents::~AlignedComponents() {
}

void AlignedComponents::initializey1(const ComponentPtrs& v) {
  clear();
  for(ComponentPtrs::const_iterator i = v.begin();
      i != v.end(); ++i)
    addy1(*i);
}

void AlignedComponents::initializey2(const ComponentPtrs& v) {
  clear();
  for(ComponentPtrs::const_iterator i = v.begin();
      i != v.end(); ++i)
    addy2(*i);
}

void AlignedComponents::initializex1(const ComponentPtrs& v) {
  clear();
  for(ComponentPtrs::const_iterator i = v.begin();
      i != v.end(); ++i)
    addx1(*i);
}

void AlignedComponents::initializex2(const ComponentPtrs& v) {
  clear();
  for(ComponentPtrs::const_iterator i = v.begin();
      i != v.end(); ++i)
    addx2(*i);
}

void AlignedComponents::addy1(const Component* c) {
  operator[](c->m_nY).insert(c->m_nID);
}

void AlignedComponents::addy2(const Component* c) {
  operator[](c->m_nY + c->m_Dims.m_nHeight - 1).insert(c->m_nID);
}

void AlignedComponents::addx1(const Component* c) {
  operator[](c->m_nX).insert(c->m_nID);
}

void AlignedComponents::addx2(const Component* c) {
  operator[](c->m_nX + c->m_Dims.m_nWidth - 1).insert(c->m_nID);
}

UInt AlignedComponents::size(UInt i) const {
  const_iterator j = find(i);
  if(j == end()) return(0);
  else return(j->second.size());
}

bool AlignedComponents::empty(UInt i) const {
  const_iterator j = find(i);
  if(j == end()) return(true);
  else return(j->second.empty());
}

UInt AlignedComponents::size() const {
  return((UInt) std::map<UInt, std::set<UInt> >::size());
}

bool AlignedComponents::empty() const {
  return(std::map<UInt, std::set<UInt> >::empty());
}
