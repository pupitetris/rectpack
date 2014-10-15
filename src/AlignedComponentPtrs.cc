/**
 * This file is part of rectpack.
 *
 * rectpack is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "AlignedComponentPtrs.h"
#include "Component.h"
#include "ComponentPtrs.h"
#include <iomanip>

AlignedComponentPtrs::AlignedComponentPtrs() {
}

AlignedComponentPtrs::~AlignedComponentPtrs() {
}

void AlignedComponentPtrs::initializey1(const ComponentPtrs& v) {
  clear();
  for(ComponentPtrs::const_iterator i = v.begin();
      i != v.end(); ++i)
    addy1(*i);
}

void AlignedComponentPtrs::initializey2(const ComponentPtrs& v) {
  clear();
  for(ComponentPtrs::const_iterator i = v.begin();
      i != v.end(); ++i)
    addy2(*i);
}

void AlignedComponentPtrs::initializex1(const ComponentPtrs& v) {
  clear();
  for(ComponentPtrs::const_iterator i = v.begin();
      i != v.end(); ++i)
    addx1(*i);
}

void AlignedComponentPtrs::initializex2(const ComponentPtrs& v) {
  clear();
  for(ComponentPtrs::const_iterator i = v.begin();
      i != v.end(); ++i)
    addx2(*i);
}

void AlignedComponentPtrs::addy1(Component* c) {
  operator[](c->m_nY).insert(c);
}

void AlignedComponentPtrs::addy2(Component* c) {
  operator[](c->m_nY + c->m_Dims.m_nHeight - 1).insert(c);
}

void AlignedComponentPtrs::addx1(Component* c) {
  operator[](c->m_nX).insert(c);
}

void AlignedComponentPtrs::addx2(Component* c) {
  operator[](c->m_nX + c->m_Dims.m_nWidth - 1).insert(c);
}

void AlignedComponentPtrs::dely1(Component* c) {
  operator[](c->m_nY).erase(c);
}

void AlignedComponentPtrs::dely2(Component* c) {
  operator[](c->m_nY + c->m_Dims.m_nHeight - 1).erase(c);
}

void AlignedComponentPtrs::delx1(Component* c) {
  operator[](c->m_nX).erase(c);
}

void AlignedComponentPtrs::delx2(Component* c) {
  operator[](c->m_nX + c->m_Dims.m_nWidth - 1).erase(c);
}

UInt AlignedComponentPtrs::size(UInt i) const {
  const_iterator j = find(i);
  if(j == end()) return(0);
  else return(j->second.size());
}

bool AlignedComponentPtrs::empty(UInt i) const {
  const_iterator j = find(i);
  if(j == end()) return(true);
  else return(j->second.empty());
}

UInt AlignedComponentPtrs::size() const {
  return((UInt) std::map<UInt, std::set<Component*> >::size());
}

bool AlignedComponentPtrs::empty() const {
  return(std::map<UInt, std::set<Component*> >::empty());
}

void AlignedComponentPtrs::print() const {
  for(const_iterator i = begin(); i != end(); ++i) {
    std::cout << std::setw(2) << i->first
	      << ": ";
    for(std::set<Component*>::const_iterator j = i->second.begin();
	j != i->second.end(); ++j)
      std::cout << (*j)->m_nID << " ";
    std::cout << std::endl;
  }
}

void AlignedComponentPtrs::print2() const {
  for(const_iterator i = begin(); i != end(); ++i) {
    std::cout << std::setw(2) << i->first
	      << ": ";
    for(std::set<Component*>::const_iterator j = i->second.begin();
	j != i->second.end(); ++j)
      std::cout << (*j)->m_Dims << " ";
    std::cout << std::endl;
  }
}
