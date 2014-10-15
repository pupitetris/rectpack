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

#include "CornerValues.h"
#include "Component.h"
#include "CornerVariables.h"
#include "Inferences.h"
#include "Grid.h"
#include "Printer.h"
#include <iomanip>
#include <sstream>

CornerValues::CornerValues() :
  std::vector<AlignedCongruencies>(2) {
}

CornerValues::~CornerValues() {
}

void CornerValues::initialize(const AlignedCongruencies& above,
			      const AlignedCongruencies& below) {
  clear();
  resize(2);
  front() = above;
  back() = below;
}

void CornerValues::erase(Component* c) {
  operator[](Above)[c->m_nY][c->m_Dims].erase(c);
  operator[](Below)[c->y2() - 1][c->m_Dims].erase(c);
}

void CornerValues::insert(Component* c) {
  operator[](Above)[c->m_nY][c->m_Dims].insert(c);
  operator[](Below)[c->y2() - 1][c->m_Dims].insert(c);
}

void CornerValues::print() const {
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
  if(!front().empty()) nMax = std::max(nMax, (UInt) front().rbegin()->first);
  if(!back().empty()) nMax = std::max(nMax, (UInt) back().rbegin()->first);
  for(UInt i = 0; i <= nMax; ++i) {
    std::ostringstream oss;
    AlignedCongruencies::const_iterator k = front().find(i);
    if(k != front().end()) {
      oss << std::setw(2) << i << ": a=";
      k->second.print(oss);
    }
    else oss << std::setw(2) << i << ": a={}";
    while(oss.str().size() < vStart[1]) oss << ' ';
    oss << " b=";
    AlignedCongruencies::const_iterator j = back().find(i);
    if(j == back().end()) oss << "{}";
    else j->second.print(oss);
    std::cout << oss.str() << std::endl;
  }
}

bool CornerValues::emptyDomain(const CornerVariables& cv) const {
  for(AlignedCongruencies::const_iterator i = front().begin();
      i != front().end(); ++i) {

    /**
     * If there were sets of values and now there aren't any valid
     * values we check to see if there there are any unassigned
     * variables that still require values from this set.
     */

    if(!i->second.empty() &&
       (!cv.empty(CornerVariables::UpperLeft, i->first) ||
	!cv.empty(CornerVariables::UpperRight, i->first) &&
	i->second.emptyDomain()))
      return(true);
  }

  for(AlignedCongruencies::const_iterator i = back().begin();
      i != back().end(); ++i) {
    if(!i->second.empty() &&
       (!cv.empty(CornerVariables::LowerLeft, i->first) ||
	!cv.empty(CornerVariables::LowerRight, i->first)) &&
       i->second.emptyDomain())
      return(true);
  }
  return(false);
}

bool CornerValues::emptyDomain(const Grid* pGrid,
			       const Inferences& inf) const {
  for(AlignedCongruencies::const_iterator y = front().begin();
      y != front().end(); ++y) {

    /**
     * Get the value that we want to check for.
     */

    if(y->second.components() == 0) continue;
    Dimensions d = y->second.largestWidth();
#if 0
    Dimensions d = y->second.largestArea();
    Dimensions d = y->second.largestHeight();
#endif

    /**
     * Scan across the row looking for a place to put this component.
     */

    UInt x;
    for(x = 0; x <= pGrid->box().m_nWidth - d.m_nWidth; ++x) {
      UInt xn1(x + d.m_nWidth - 1), yn1(y->first + d.m_nHeight - 1);
      if(pGrid->occupied(x, y->first))
	x = inf.m_vComponents[pGrid->get(x, y->first)]->m_nX +
	  inf.m_vComponents[pGrid->get(x, y->first)]->m_Dims.m_nWidth - 1;
      else if(pGrid->occupied(xn1, y->first))
	x = inf.m_vComponents[pGrid->get(xn1, y->first)]->m_nX +
	  inf.m_vComponents[pGrid->get(xn1, y->first)]->m_Dims.m_nWidth - 1;
      else if(pGrid->occupied(x, yn1))
	x = inf.m_vComponents[pGrid->get(x, yn1)]->m_nX +
	  inf.m_vComponents[pGrid->get(x, yn1)]->m_Dims.m_nWidth - 1;
      else if(pGrid->occupied(xn1, yn1))
	x = inf.m_vComponents[pGrid->get(xn1, yn1)]->m_nX +
	  inf.m_vComponents[pGrid->get(xn1, yn1)]->m_Dims.m_nWidth - 1;
      else if(pGrid->m_vVer[x][y->first] == pGrid->m_vVer[x][yn1] &&
	      pGrid->m_vVer[xn1][y->first] == pGrid->m_vVer[xn1][yn1] &&
	      pGrid->m_vHor[x][y->first] == pGrid->m_vHor[xn1][y->first] &&
	      pGrid->m_vHor[x][yn1] == pGrid->m_vHor[xn1][yn1])
	break; // We found a placement.
    }
    if(x > pGrid->box().m_nWidth - d.m_nWidth) // No placement found
      return(true);
  }
  return(false);
}
