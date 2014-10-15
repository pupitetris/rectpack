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

#include "BinomialCache.h"
#include "Component.h"
#include "EmptySpace.h"
#include "Printer.h"
#include "SymmetryMap.h"

SymmetryMap::SymmetryMap() {
}

SymmetryMap::~SymmetryMap() {
}

UInt SymmetryMap::symPermutations(BinomialCache& c) const {
  UInt nProduct(1), nElements(0);
  for(const_iterator i = begin(); i != end(); ++i) {
    if(i->second.empty()) continue;
    nProduct *= c.choose(i->second.size() + nElements, i->second.size());
    nElements += i->second.size();
  }
  return(nElements == 0 ? 0 : nProduct);
}

UInt SymmetryMap::symPermutations2(BinomialCache& c) const {
  UInt nProduct(1), nElements(0);
  for(const_iterator i = begin(); i != end(); ++i) {
    UInt nActualSize(0);
    if(i->second.empty()) continue;
    for(std::set<Component*>::const_iterator j = i->second.begin();
	j != i->second.end(); ++j)
      if((*j)->m_pEmpty)
	nActualSize += (*j)->m_pEmpty->m_nHeightLeft;
      else 
	++nActualSize;
    nProduct *= c.choose(nActualSize + nElements, nActualSize);
    nElements += nActualSize;
  }
  return(nElements == 0 ? 0 : nProduct);
}

UInt SymmetryMap::symPermutations3(BinomialCache& c) const {
  UInt nElements(0);
  for(const_iterator i = begin(); i != end(); ++i) {
    if(i->second.empty()) continue;
    for(std::set<Component*>::const_iterator j = i->second.begin();
	j != i->second.end(); ++j)
      if((*j)->m_pEmpty)
	nElements += (*j)->m_pEmpty->m_nHeightLeft;
      else 
	++nElements;
  }
  return(nElements);
}

UInt SymmetryMap::symPermutations(BinomialCache& c, Int nWidth,
				  Int nHeight) const {
  UInt nProduct(1), nElements(0);
  for(const_iterator i = begin(); i != end(); ++i) {
    if(i->second.empty() || !i->first.canFit(nWidth, nHeight)) continue;
    nProduct *= c.choose(i->second.size() + nElements, i->second.size());
    nElements += i->second.size();
  }
  return(nElements == 0 ? 0 : nProduct);
}

bool SymmetryMap::emptyDomain() const {
  for(const_iterator i = begin(); i != end(); ++i)
    if(!i->second.empty()) return(false);
  return(true);
}

UInt SymmetryMap::components() const {
  UInt nComponents(0);
  for(const_iterator i = begin(); i != end(); ++i)
    nComponents += i->second.size();
  return(nComponents);
}

UInt SymmetryMap::components(Int nWidth, Int nHeight) const {
  UInt nComponents(0);
  for(const_iterator i = begin(); i != end(); ++i)
    if(i->first.canFit(nWidth, nHeight))
      nComponents += i->second.size();
  return(nComponents);
}

bool SymmetryMap::emptyDomain(Int nWidth, Int nHeight) const {

  /**
   * See if there is something that fits, and the set is non-empty. If
   * that's the case, then we do in fact have an assignable value.
   */

  for(const_iterator i = begin(); i != end(); ++i)
    if(i->first.canFit(nWidth, nHeight) &&
       !i->second.empty())
      return(false);
  return(true);
}

Dimensions SymmetryMap::largestArea() const {
  for(const_iterator i = begin(); i != end(); ++i)
    if(!i->second.empty())
      return(i->first);
  return(Dimensions());
}

Dimensions SymmetryMap::largestHeight() const {
  Dimensions d;
  for(const_iterator i = begin(); i != end(); ++i)
    if(!i->second.empty() && d.m_nHeight < i->first.m_nHeight)
      d = i->first;
  return(d);
}

Dimensions SymmetryMap::largestWidth() const {
  Dimensions d;
  for(const_iterator i = begin(); i != end(); ++i)
    if(!i->second.empty() && d.m_nWidth < i->first.m_nWidth)
      d = i->first;
  return(d);
}

void SymmetryMap::print() const {
  print(std::cout);
}

void SymmetryMap::print(std::ostream& os) const {
  os << "{";
  if(!empty()) {
    const_iterator i = begin();
    os << i->first << "=";
    Printer::print(i->second, os);
    for(++i; i != end(); ++i) {
      os << "," << i->first << "=";
      Printer::print(i->second, os);
    }
  }
  os << "}" << std::flush;
}

void SymmetryMap::hasHeight(UInt h, std::list<Int>& l) const {
  for(const_iterator i = begin(); i != end(); ++i)
    if(i->first.m_nHeight == h)
      for(std::set<Component*>::const_iterator j = i->second.begin();
	  j != i->second.end(); ++j)
	l.push_back((*j)->m_nID);
}
