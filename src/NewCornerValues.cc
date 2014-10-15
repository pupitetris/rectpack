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

#include "NewCornerValues.h"
#include "Component.h"
#include "NewCornerVariables.h"
#include "Inferences.h"
#include "Grid.h"
#include "Printer.h"
#include <iomanip>
#include <sstream>

NewCornerValues::NewCornerValues() :
  std::vector<AlignedCongruencies>(2) {
}

NewCornerValues::~NewCornerValues() {
}

void NewCornerValues::initialize(const AlignedCongruencies& right,
				 const AlignedCongruencies& left) {
  clear();
  resize(2);
  operator[](Right) = right;
  operator[](Left) = left;
}

void NewCornerValues::erase(Component* c) {
  operator[](Left)[c->m_nX][c->m_Dims].erase(c);
  operator[](Right)[c->x2() - 1][c->m_Dims].erase(c);
}

void NewCornerValues::insert(Component* c) {
  operator[](Left)[c->m_nX][c->m_Dims].insert(c);
  operator[](Right)[c->x2() - 1][c->m_Dims].insert(c);
}

void NewCornerValues::print() const {
  std::vector<UInt> vMax(2, 0);
  std::vector<UInt> vStart(2, 0);
  for(UInt i = 0; i < 2; ++i)
    for(AlignedCongruencies::const_iterator j =
	  operator[](i).begin(); j != operator[](i).end(); ++j) {
      std::ostringstream oss;
      j->second.print(oss);
      vMax[i] = std::max(vMax[i], (UInt) oss.str().size());
    }
  vStart[0] = 0;
  vStart[1] = 6 + vMax[0];
  UInt nMax(0);
  if(!operator[](Right).empty())
    nMax = std::max(nMax, (UInt) operator[](Right).rbegin()->first);
  if(!operator[](Left).empty())
    nMax = std::max(nMax, (UInt) operator[](Left).rbegin()->first);
  for(UInt i = 0; i <= nMax; ++i) {
    std::ostringstream oss;
    AlignedCongruencies::const_iterator k = operator[](Right).find(i);
    if(k != operator[](Right).end()) {
      oss << std::setw(2) << i << ": r=";
      k->second.print(oss);
    }
    else oss << std::setw(2) << i << ": r={}";
    while(oss.str().size() < vStart[1]) oss << ' ';
    oss << " l=";
    AlignedCongruencies::const_iterator j = operator[](Left).find(i);
    if(j == operator[](Left).end()) oss << "{}";
    else j->second.print(oss);
    std::cout << oss.str() << std::endl;
  }
}

bool NewCornerValues::emptyDomain(const NewCornerVariables& cv) const {
  for(AlignedCongruencies::const_iterator i = operator[](Right).begin();
      i != operator[](Right).end(); ++i) {

    /**
     * If there were sets of values and now there aren't any valid
     * values we check to see if there there are any unassigned
     * variables that still require values from this set.
     */

    if(!i->second.empty() &&
       (!cv.empty(NewCornerVariables::UpperLeft, i->first) ||
	!cv.empty(NewCornerVariables::UpperRight, i->first) &&
	i->second.emptyDomain()))
      return(true);
  }

  for(AlignedCongruencies::const_iterator i = operator[](Left).begin();
      i != operator[](Left).end(); ++i) {
    if(!i->second.empty() &&
       (!cv.empty(NewCornerVariables::LowerLeft, i->first) ||
	!cv.empty(NewCornerVariables::LowerRight, i->first)) &&
       i->second.emptyDomain())
      return(true);
  }
  return(false);
}
