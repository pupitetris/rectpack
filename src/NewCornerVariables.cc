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

#include "AlignedCongruencies.h"
#include "BoxDimensions.h"
#include "Component.h"
#include "CornerValues.h"
#include "NewCornerVariables.h"
#include "EmptyCorner.h"
#include "Grid.h"
#include "Printer.h"
#include <iomanip>
#include <sstream>

NewCornerVariables::NewCornerVariables() :
  std::vector<std::map<UInt, std::set<Int> > >(4) {
}

NewCornerVariables::~NewCornerVariables() {
}

void NewCornerVariables::initialize(const BoxDimensions& b) {
  clear();
  resize(4);
  operator[](UpperLeft)[0].insert(b.m_nHeight - 1);
  operator[](LowerLeft)[0].insert(0);
  operator[](LowerRight)[b.m_nWidth - 1].insert(0);
  operator[](UpperRight)[b.m_nWidth - 1].insert(b.m_nHeight - 1);
}

void NewCornerVariables::erase(const EmptyCorner& ec) {
  operator[](ec.m_nType)[ec.m_Location.x].erase(ec.m_Location.y);
}

void NewCornerVariables::insert(const EmptyCorner& ec) {
  operator[](ec.m_nType)[ec.m_Location.x].insert(ec.m_Location.y);
}

void NewCornerVariables::insert(std::vector<EmptyCorner>& vErased) {
  for(std::vector<EmptyCorner>::const_iterator i = vErased.begin();
      i != vErased.end(); ++i)
    insert(*i);
}

void NewCornerVariables::erase(const Component* c,
			       std::vector<EmptyCorner>& vErased) {

  std::set<Int>::iterator i;

  /**
   * Check upper left.
   */

  Int y2(c->m_nY + c->m_Dims.m_nHeight - 1);
  i = operator[](UpperLeft)[c->m_nX].find(y2);
  if(i != operator[](UpperLeft)[c->m_nX].end()) {
    vErased.push_back(EmptyCorner(c->m_nX, y2,
				  EmptyCorner::UpperLeft));
    operator[](UpperLeft)[c->m_nX].erase(i);
  }

  /**
   * check lower left.
   */

  i = operator[](LowerLeft)[c->m_nX].find(c->m_nY);
  if(i != operator[](LowerLeft)[c->m_nX].end()) {
    vErased.push_back(EmptyCorner(c->m_nX, c->m_nY,
				  EmptyCorner::LowerLeft));
    operator[](LowerLeft)[c->m_nX].erase(i);
  }

  /**
   * Check lower right.
   */

  Int x2(c->m_nX + c->m_Dims.m_nWidth - 1);
  i = operator[](LowerRight)[x2].find(c->m_nY);
  if(i != operator[](LowerRight)[x2].end()) {
    vErased.push_back(EmptyCorner(x2, c->m_nY,
				  EmptyCorner::LowerRight));
    operator[](LowerRight)[x2].erase(i);
  }

  /**
   * Check upper right.
   */

  i = operator[](UpperRight)[x2].find(y2);
  if(i != operator[](UpperRight)[x2].end()) {
    vErased.push_back(EmptyCorner(x2, y2,
				  EmptyCorner::UpperRight));
    operator[](UpperRight)[x2].erase(i);
  }
}

void NewCornerVariables::print() const {
  std::vector<UInt> vMax(4, 0);
  std::vector<UInt> vStart(4, 0);
  for(UInt i = 0; i < 4; ++i)
    for(std::map<UInt, std::set<Int> >::const_iterator j =
	  operator[](i).begin(); j != operator[](i).end(); ++j) {
      std::ostringstream oss;
      Printer::print(j->second, oss);
      vMax[i] = std::max(vMax[i], (UInt) oss.str().size());
    }

  /**
   * Starting positions of the sets.
   */

  vStart[0] = 0;
  vStart[1] = vStart[0] + vMax[0] + 7;
  vStart[2] = vStart[1] + vMax[1] + 4;
  vStart[3] = vStart[2] + vMax[2] + 4;

  UInt nMax(0);
  nMax = std::max(nMax, operator[](0).rbegin()->first);
  nMax = std::max(nMax, operator[](1).rbegin()->first);
  nMax = std::max(nMax, operator[](2).rbegin()->first);
  nMax = std::max(nMax, operator[](3).rbegin()->first);
  for(UInt m = 0; m <= nMax; ++m) {
    std::map<UInt, std::set<Int> >::const_iterator
      i(operator[](0).find(m)),
      j(operator[](1).find(m)),
      k(operator[](2).find(m)),
      l(operator[](3).find(m));
    
    std::ostringstream oss;
    oss << std::setw(2) << m << ": ul=";
    if(i == operator[](0).end()) oss << "{}";
    else Printer::print(i->second, oss);
    while(oss.str().size() < vStart[1]) oss << ' ';
    oss << " ll=";
    if(j == operator[](1).end()) oss << "{}";
    else Printer::print(j->second, oss);
    while(oss.str().size() < vStart[2]) oss << ' ';
    oss << " lr=";
    if(k == operator[](2).end()) oss << "{}";
    else Printer::print(k->second, oss);
    while(oss.str().size() < vStart[3]) oss << ' ';
    oss << " ur=";
    if(l == operator[](3).end()) oss << "{}";
    else Printer::print(l->second, oss);
    std::cout << oss.str() << std::endl;
  }
}

bool NewCornerVariables::empty(Corner c, UInt n) const {
  std::map<UInt, std::set<Int> >::const_iterator i =
    operator[](c).find(n);
  return(i == operator[](c).end() || i->second.empty());
}
