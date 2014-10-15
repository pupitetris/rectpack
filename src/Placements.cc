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

#include "Placements.h"
#include "HeightFirstPlacement.h"
#include "OrderedCuSP.h"
#include "Parameters.h"
#include "WidthFirstPlacement.h"
#include <iomanip>
#include <sstream>

Placements::Placements() {
}

Placements::~Placements() {
}

void Placements::assignX() {
  std::vector<PlacementDec*> v;
  v.reserve(size());
  for(iterator i = begin(); i != end(); ++i)
    v.push_back(new HeightFirstPlacement(&(*i)));
  assign(v);
  while(!v.empty()) {
    delete v.back();
    v.pop_back();
  }

  /**
   * Derive a new width for the bounding box.
   */

  for(const_iterator i = begin(); i != end(); ++i)
    m_Box.m_nWidth = std::max(m_Box.m_nWidth, i->x2());
}

void Placements::assignY() {
  std::vector<PlacementDec*> v;
  v.reserve(size());
  for(iterator i = begin(); i != end(); ++i)
    v.push_back(new WidthFirstPlacement(&(*i)));
  assign(v);
  while(!v.empty()) {
    delete v.back();
    v.pop_back();
  }

  /**
   * Derive a new height for the bounding box.
   */

  for(const_iterator i = begin(); i != end(); ++i)
    m_Box.m_nHeight = std::max(m_Box.m_nHeight, i->y2());
}

void Placements::assign(std::vector<PlacementDec*>& v,
			bool bSort) const {
  
  /**
   * We assume the first dimension coordinates are set, and will now
   * figure out the second dimension coordinates. First we sort.
   */

  if(bSort)
    std::sort(v.begin(), v.end(), OrderedCuSP());
  std::vector<PlacementDec*> vAssigned;
  vAssigned.reserve(v.size());
  for(std::vector<PlacementDec*>::const_iterator i = v.begin();
      i != v.end(); ++i) {
    (*i)->c2() = (UInt) 0;
    std::vector<PlacementDec*> vCurrent(vAssigned);
    std::vector<PlacementDec*>::iterator j = vCurrent.begin();
    while(j != vCurrent.end()) {
      if((*i)->m_pPlacement->overlaps(*(*j)->m_pPlacement)){
	(*i)->c2() = (*j)->c2() + (URational) (*j)->d2();
	std::swap(*j, vCurrent.back());
	vCurrent.pop_back();
	j = vCurrent.begin();
      } else ++j;
    }
    vAssigned.push_back(*i);
  }
}

void Placements::print() const {
  UInt nDims(0), nCoords(0);
  std::vector<std::string> v;
  v.reserve(size() * 2);
  for(const_iterator i = begin(); i != end(); ++i) {
    std::ostringstream oss1, oss2;
    oss1 << i->m_Dims << std::flush;
    oss2 << i->m_nLocation << std::flush;
    v.push_back(oss1.str());
    v.push_back(oss2.str());
    nDims = std::max(nDims, (UInt) oss1.str().size());
    nCoords = std::max(nCoords, (UInt) oss2.str().size());
  }

  /**
   * We'll consider different numbers of columns. Of course, if we
   * ever exceed 80 columns, we'll just abort this consideration.
   */

  UInt nCols(2);
  for(; nCols <= v.size(); nCols += 2)
    if(widthLine(nCols, v) + widthSpace(nCols) > 80)
      break;
  nCols -= 2;

  /**
   * At this point, nCols represents the number of columns we can have
   * without breaking the width of 80 columns in our terminal. Let's
   * now get the width of every column.
   */

  std::vector<UInt> vColumnWidths(nCols, 0);
  for(UInt i = 0; i < nCols; ++i)
    vColumnWidths[i] = widthColumn(i, nCols, v);

  /**
   * Now simply print the formatted table out.
   */

  for(UInt i = 0; i < v.size(); ++i) {
    
    /**
     * If this is the first on a new line, we should prepend some
     * space.
     */

    UInt nCurrent(i % nCols);
    if(nCurrent == 0)
      std::cout << "   ";
    
    /**
     * If we're printing out the width and height, we will justify to
     * the right. Otherwise, we're printing the coordinates.
     */

    if(i % 2 == 0)
      std::cout << " "
		<< std::resetiosflags(std::ios::adjustfield)
		<< std::setiosflags(std::ios::right)
		<< std::setw(vColumnWidths[nCurrent])
		<< v[i] << "="
		<< std::flush;
    else
      std::cout	<< std::resetiosflags(std::ios::adjustfield)
		<< std::setiosflags(std::ios::left)
		<< std::setw(vColumnWidths[nCurrent])
		<< v[i]
		<< std::flush;

    /**
     * If we're at the end of the line, insert a newline.
     */

    if(((i + 1) % nCols) == 0)
      std::cout << std::endl;
  }
}

UInt Placements::widthColumn(UInt nColumn, UInt nColumns,
			     const std::vector<std::string>& v) const {
  UInt nMax(0);
  for(; nColumn < v.size(); nColumn += nColumns)
    nMax = std::max(nMax, (UInt) v[nColumn].size());
  return(nMax);
}

UInt Placements::widthLine(UInt nColumns,
			   const std::vector<std::string>& v) const {
  UInt nWidth(0);
  for(UInt i = 0; i < nColumns; ++i)
    nWidth += widthColumn(i, nColumns, v);
  return(nWidth);
}

UInt Placements::widthSpace(UInt nColumns) const {
  if(nColumns < 2) return(0);
  
  /**
   * For each pair of columns, we have an association between the
   * rectangle dimensions and their (x,y) coordinates, which will
   * simply be the equal sign. So every pair will have that marker.
   */

  UInt nPairs(nColumns / 2);
  UInt nSpacing(nPairs);

  /**
   * Next, if there is just a single pair, then we don't need any
   * separation between these column pairs.
   */

  if(nPairs < 2) return(nSpacing);

  /**
   * Otherwise, we have multiple pairs. Depending on the number of
   * pairs p, we need p-1 white space to separate them.
   */

  nSpacing += nPairs - 1;
  return(nSpacing);
}

Placements& Placements::operator*=(const URational& ur) {
  for(iterator i = begin(); i != end(); ++i)
    (*i) *= ur;
  m_Box *= ur;
  return(*this);
}

Placements& Placements::operator/=(const URational& ur) {
  for(iterator i = begin(); i != end(); ++i)
    (*i) /= ur;
  m_Box /= ur;
  return(*this);
}

URational Placements::minLabelScale() const {
  URational r(0, 1);
  for(const_iterator i = begin(); i != end(); ++i)
    r = std::max(r, i->m_Dims.minLabelScale());
  return(r);
}

bool Placements::verify() const {
  for(const_iterator i = begin(); i != end(); ++i) {
    if(!i->inside(m_Box))
      return(false);
    for(const_iterator j = i + 1; j != end(); ++j)
      if(i->overlaps(*j))
	return(false);
  }
  return(true);
}

void Placements::rotate() {
  m_Box.rotate();
  for(iterator i = begin(); i != end(); ++i)
    i->rotate();
}
