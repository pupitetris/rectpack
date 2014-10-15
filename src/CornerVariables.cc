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
#include "CornerVariables.h"
#include "EmptyCorner.h"
#include "Grid.h"
#include "Printer.h"
#include <iomanip>
#include <sstream>

CornerVariables::CornerVariables() :
  std::vector<std::map<UInt, std::set<Int> > >(4) {
}

CornerVariables::~CornerVariables() {
}

void CornerVariables::initialize(const BoxDimensions& b) {
  clear();
  resize(4);
  operator[](UpperLeft)[0].insert(0);
  operator[](LowerLeft)[b.m_nHeight - 1].insert(0);
  operator[](LowerRight)[b.m_nHeight - 1].insert(b.m_nWidth - 1);
  operator[](UpperRight)[0].insert(b.m_nWidth - 1);
}

void CornerVariables::erase(const EmptyCorner& ec) {
  operator[](ec.m_nType)[ec.m_Location.y].erase(ec.m_Location.x);
}

void CornerVariables::insert(const EmptyCorner& ec) {
  operator[](ec.m_nType)[ec.m_Location.y].insert(ec.m_Location.x);
}

void CornerVariables::insert(std::vector<EmptyCorner>& vErased) {
  for(std::vector<EmptyCorner>::const_iterator i = vErased.begin();
      i != vErased.end(); ++i)
    insert(*i);
}

void CornerVariables::erase(const Component* c,
			    std::vector<EmptyCorner>& vErased) {

  std::set<Int>::iterator i;

  /**
   * Check upper left.
   */

  i = operator[](0)[c->m_nY].find(c->m_nX);
  if(i != operator[](0)[c->m_nY].end()) {
    vErased.push_back(EmptyCorner(c->m_nX, c->m_nY,
				  EmptyCorner::UpperLeft));
    operator[](0)[c->m_nY].erase(i);
  }

  /**
   * check lower left.
   */

  UInt ys = c->m_nY + c->m_Dims.m_nHeight - 1;
  i = operator[](1)[ys].find(c->m_nX);
  if(i != operator[](1)[ys].end()) {
    vErased.push_back(EmptyCorner(c->m_nX, ys,
				  EmptyCorner::LowerLeft));
    operator[](1)[ys].erase(i);
  }

  /**
   * Check lower right.
   */

  UInt xs = c->m_nX + c->m_Dims.m_nWidth - 1;
  i = operator[](2)[ys].find(xs);
  if(i != operator[](2)[ys].end()) {
    vErased.push_back(EmptyCorner(xs, ys,
				  EmptyCorner::LowerRight));
    operator[](2)[ys].erase(i);
  }

  /**
   * Check upper right.
   */

  i = operator[](3)[c->m_nY].find(xs);
  if(i != operator[](3)[c->m_nY].end()) {
    vErased.push_back(EmptyCorner(xs, c->m_nY,
				  EmptyCorner::UpperRight));
    operator[](3)[c->m_nY].erase(i);
  }
}

void CornerVariables::print() const {
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

bool CornerVariables::emptyDomain(const CornerValues& cv,
				  const Grid* pGrid) const {
  for(std::map<UInt, std::set<Int> >::const_iterator y = front().begin();
      y != front().end(); ++y) {

    /**
     * Iterate over all of the upper left corners, and for each one,
     * check the values to make sure that we don't have an empty
     * domain. If it's empty then we'll immediately return.
     */

    for(std::set<Int>::const_iterator i = y->second.begin();
	i != y->second.end(); ++i) {
      AlignedCongruencies::const_iterator j =
	cv[CornerValues::Above].find(y->first);
      if(j == cv[CornerValues::Above].end() ||
	 j->second.emptyDomain(pGrid->m_vHor[*i][y->first],
			       pGrid->m_vVer[*i][y->first]))
	return(true);
    }
  }

  for(std::map<UInt, std::set<Int> >::const_iterator y =
	operator[](UpperRight).begin();
      y != operator[](UpperRight).end(); ++y) {

    /**
     * Check the upper right corners as well.
     */

    for(std::set<Int>::const_iterator i = y->second.begin();
	i != y->second.end(); ++i) {
      AlignedCongruencies::const_iterator j =
	cv[CornerValues::Above].find(y->first);
      if(j == cv[CornerValues::Above].end() ||
	 j->second.emptyDomain(pGrid->m_vHor[*i][y->first],
			       pGrid->m_vVer[*i][y->first]))
	return(true);
    }
  }

  for(std::map<UInt, std::set<Int> >::const_iterator y =
	operator[](LowerLeft).begin();
      y != operator[](LowerLeft).end(); ++y) {

    /**
     * Check the lower left corners.
     */

    for(std::set<Int>::const_iterator i = y->second.begin();
	i != y->second.end(); ++i) {
      AlignedCongruencies::const_iterator j =
	cv[CornerValues::Below].find(y->first);
      if(j == cv[CornerValues::Below].end() ||
	 j->second.emptyDomain(pGrid->m_vHor[*i][y->first],
			       pGrid->m_vVer[*i][y->first]))
	return(true);
    }
  }

  /**
   * Check the lower right corners.
   */

  for(std::map<UInt, std::set<Int> >::const_iterator y =
	operator[](LowerRight).begin();
      y != operator[](LowerRight).end(); ++y) {

    for(std::set<Int>::const_iterator i = y->second.begin();
	i != y->second.end(); ++i) {
      AlignedCongruencies::const_iterator j =
	cv[CornerValues::Below].find(y->first);
      if(j == cv[CornerValues::Below].end() ||
	 j->second.emptyDomain(pGrid->m_vHor[*i][y->first],
			       pGrid->m_vVer[*i][y->first]))
	return(true);
    }
  }
  return(false);
}

bool CornerVariables::empty(Corner c, UInt n) const {
  std::map<UInt, std::set<Int> >::const_iterator i =
    operator[](c).find(n);
  return(i == operator[](c).end() || i->second.empty());
}
