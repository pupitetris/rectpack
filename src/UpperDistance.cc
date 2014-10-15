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
#include "BoxDimensions.h"
#include "Component.h"
#include "ComponentPtrs.h"
#include "UpperDistance.h"
#include <iomanip>
#include <limits>

UpperDistance::UpperDistance() {
}

UpperDistance::~UpperDistance() {
}

void UpperDistance::initialize(const ComponentPtrs& v,
			       const BoxDimensions& b) {

  /**
   * Grab only the top-level components.
   */

  m_vComponents.clear();
  m_vComponents.reserve(v.size());
  m_vComponentMap.clear();
  m_vComponentMap.resize(v.size(), std::numeric_limits<UInt>::max());
  for(UInt i = 0; i < v.size(); ++i)
    if(v[i]->m_bTopLevel) {
      m_vComponents.push_back(v[i]);
      m_vComponentMap[i] = m_vComponents.size() - 1;
    }

  /**
   * Initialize our upper bound matrix using the box dimensions.
   */

  clear();
  resize(m_vComponents.size(),
	 std::vector<UInt>(m_vComponents.size(), 0));
  for(UInt i = 0; i < size(); ++i)
    for(UInt j = i + 1; j < size(); ++j) {
      operator[](i)[j] = b.m_nWidth - m_vComponents[j]->m_Dims.m_nWidth;
      operator[](j)[i] = b.m_nWidth - m_vComponents[i]->m_Dims.m_nWidth;
    }
}

void UpperDistance::compute(const AlignedComponents& vTops,
			    const AlignedComponents& vBottoms,
			    const std::vector<bool>& vBottomsAdjacent,
			    const std::vector<std::set<UInt> >& vBitmap,
			    Adjacency& y) {
  for(UInt i = 0; i < vBottoms.size() - 1; ++i) {
    AlignedComponents::const_iterator j = vBottoms.find(i);
    if(j != vBottoms.end() && !j->second.empty()) {
      AlignedComponents::const_iterator k = vTops.find(i + 1);
      std::set<UInt> sEmpty;
      const std::set<UInt>* pTop;
      pTop = (k == vTops.end()) ? &sEmpty : &k->second;
      UInt nWidth(width(j->second));
      computeConsumption(nWidth, *pTop, j->second);

      /**
       * If the border must be completely adjacent, then we can
       * enforce the bounds imposed by the border length.
       */

      if(vBottomsAdjacent[i]) {
	forceBorder(nWidth, *pTop);
	forceBorder(nWidth, j->second);
	forceBorder(nWidth, *pTop, j->second);
      }
    }
  }
  bool bWorkDone(true);
  while(bWorkDone) {
    asps();
    bWorkDone = tightenBounds(vBitmap);
    bWorkDone |= tightenY(vTops, vBottoms, y);
  }
}

bool UpperDistance::asps() {
  bool bWorkDone(false);
  for(UInt k = 0; k < size(); ++k)
    for(UInt i = 0; i < size(); ++i)
      for(UInt j = 0; j < size(); ++j) {
	UInt nNew(get(i, k) + get(k, j));
	if(nNew < get(i, j)) {
	  get(i, j) = nNew;
	  bWorkDone = true;
	}
      }
  return(bWorkDone);
}

UInt& UpperDistance::get(UInt i, UInt j) {
  return(operator[](i)[j]);
}

const UInt& UpperDistance::get(UInt i, UInt j) const {
  return(operator[](i)[j]);
}

void UpperDistance::forceBorder(UInt nWidth, const std::set<UInt>& s1,
				const std::set<UInt>& s2) {
  for(std::set<UInt>::const_iterator i = s1.begin(); i != s1.end(); ++i)
    for(std::set<UInt>::const_iterator j = s2.begin(); j != s2.end(); ++j)
      updateBound(*i, *j, nWidth - component(*j)->m_Dims.m_nWidth);
}

void UpperDistance::forceBorder(UInt nWidth, const std::set<UInt>& s) {
  for(std::set<UInt>::const_iterator i = s.begin(); i != s.end(); ++i) {
    std::set<UInt>::const_iterator j(i);
    for(++j; j != s.end(); ++j)
      updateBound(*i, *j, nWidth - component(*j)->m_Dims.m_nWidth);
  }
}

void UpperDistance::forceBorder(UInt nWidth, int c,
				const std::set<UInt>& s) {
  for(std::set<UInt>::const_iterator i = s.begin(); i != s.end(); ++i)
    updateBound(c, *i, nWidth - component(*i)->m_Dims.m_nWidth);
}


void UpperDistance::computeConsumption(UInt nWidth,
				       const std::set<UInt>& tops,
				       const std::set<UInt>& bottoms) {

  /**
   * For each top item, check it against each bottom item to see if
   * there is an enforced overlap. This test is actually rather
   * simple. If their widths is greater than the border with, then
   * they must overlap by the overflow amount.
   */

  for(std::set<UInt>::const_iterator i = tops.begin();
      i != tops.end(); ++i)
    for(std::set<UInt>::const_iterator j = bottoms.begin();
	j != bottoms.end(); ++j) {
      UInt nSum(component(*i)->m_Dims.m_nWidth +
		component(*j)->m_Dims.m_nWidth);
      if(nSum > nWidth)
	updateBound(*i, *j, component(*i)->m_Dims.m_nWidth -
		    (nSum - nWidth));
    }
}

bool UpperDistance::updateBound(UInt i, UInt j, UInt nNew) {
  i = m_vComponentMap[i];
  j = m_vComponentMap[j];
  if(nNew < operator[](i)[j]) {
    operator[](i)[j] = nNew;
    operator[](j)[i] =
      nNew - m_vComponents[i]->m_Dims.m_nWidth +
      m_vComponents[j]->m_Dims.m_nWidth;
    return(true);
  }
  return(false);
}

bool UpperDistance::updateBound2(UInt i, UInt j, UInt nNew) {
  if(nNew < operator[](i)[j]) {
    operator[](i)[j] = nNew;
    operator[](j)[i] =
      nNew - m_vComponents[i]->m_Dims.m_nWidth +
      m_vComponents[j]->m_Dims.m_nWidth;
    return(true);
  }
  return(false);
}

const Component* UpperDistance::component(int i) const {
  return(m_vComponents[m_vComponentMap[i]]);
}

UInt UpperDistance::width(const std::set<UInt>& s) const {

  /**
   * Get the total width for this border. It doesn't matter which
   * array (top or bottom) we scan since the total border width will
   * be the same in either case.
   */

  UInt nWidth(0);
  for(std::set<UInt>::const_iterator i = s.begin();
      i != s.end(); ++i)
    nWidth += component(*i)->m_Dims.m_nWidth;
  return(nWidth);
}

bool UpperDistance::allGreaterThan(UInt nIgnore1, UInt nIgnore2, UInt nMin,
				   const std::set<UInt>& s) const {
  for(std::set<UInt>::const_iterator i = s.begin(); i != s.end(); ++i)
    if(*i != m_vComponents[nIgnore1]->m_nID &&
       *i != m_vComponents[nIgnore2]->m_nID &&
       component(*i)->m_Dims.m_nWidth <= nMin)
      return(false);
  return(true);
}

bool UpperDistance::tightenY(const AlignedComponents& vTops,
			     const AlignedComponents& vBottoms,
			     Adjacency& y) {
  bool bWorkDone = false;
  for(UInt i = 0; i < m_vComponents.size(); ++i)
    for(UInt j = i + 1; j < m_vComponents.size(); ++j)
      if(yAdjacent(i, j) &&
	 !boost::edge(m_vComponents[i]->m_nID, m_vComponents[j]->m_nID, y).second) 
	boost::add_edge(m_vComponents[i]->m_nID, m_vComponents[j]->m_nID, y);
  
  for(AlignedComponents::const_iterator i = vBottoms.begin();
      i != vBottoms.end(); ++i) {

    /**
     * This only works if we haven't previously constrained values
     * based on single objects (there must be more than one object in
     * both borders.
     */

    AlignedComponents::const_iterator h = vTops.find(i->first + 1);
    if(i->second.size() <= 1 || h == vTops.end() ||
       h->second.size() <= 1) continue;

    /**
     * For a particular border, see if we have a relation where one
     * side is Y-adjacent to a box on the other side and that box on
     * the other side is Y-adjacent to a differnet box back on the
     * original side.
     */

    typedef std::map<UInt, std::list<UInt> > MyMap;
    MyMap mBottom, mTop;
    AdjacencyTraits::edge_iterator ei, ei_end;
    for(boost::tie(ei, ei_end) = boost::edges(y); ei != ei_end; ++ei) {
      UInt n1(boost::source(*ei, y)), n2(boost::target(*ei, y));
      if(i->second.find(n1) != i->second.end() &&
	 h->second.find(n2) != h->second.end()) {
	mBottom[n1].push_back(n2);
	mTop[n2].push_back(n1);
      }
      else if(i->second.find(n2) != i->second.end() &&
	      h->second.find(n1) != h->second.end()) {
	mBottom[n2].push_back(n1);
	mTop[n1].push_back(n2);
      }
    }
    
    /**
     * Look for cases where we have one item that has two Y-adjacent
     * relations to items in the other border. For each of these, we
     * will attempt to tighten the bounds
     */

    UInt nWidth(width(i->second));
    for(MyMap::iterator j = mBottom.begin(); j != mBottom.end(); ++j)
      if(j->second.size() > 1)
	for(std::list<UInt>::iterator k = j->second.begin();
	    k != j->second.end(); ++k) {
	  std::list<UInt>::iterator l = k;
	  for(++l; l != j->second.end(); ++l)
	    bWorkDone |= updateBound(*k, *l, nWidth - component(*l)->m_Dims.m_nWidth);
	}
    for(MyMap::iterator j = mTop.begin(); j != mTop.end(); ++j)
      if(j->second.size() > 1)
	for(std::list<UInt>::iterator k = j->second.begin();
	    k != j->second.end(); ++k) {
	  std::list<UInt>::iterator l = k;
	  for(++l; l != j->second.end(); ++l)
	    bWorkDone |= updateBound(*k, *l, nWidth - component(*l)->m_Dims.m_nWidth);
	}
  }
  return(bWorkDone);
}

bool UpperDistance::tightenBounds(const std::vector<std::set<UInt> >& vBitmap) {
  bool bWorkDone = false;
  for(UInt i = 0; i < size(); ++i)
    for(UInt j = i + 1; j < size(); ++j) {
      
      /**
       * If the two squares are already X-adjacent, then we don't need
       * to do any more work.
       */

      if(xAdjacent(i, j) || !yOverlaps(i, j)) continue;

      /**
       * Currently the two aren't X-adjacent and they Y-overlap. Test
       * to see if the gap can be filled. If not, then we can tighten
       * the bounds and assert that we've done some work.
       */

      if(!canFill(i, j, vBitmap)) {
	updateBound2(i, j, m_vComponents[i]->m_Dims.m_nWidth);
	bWorkDone = true;
      }
    }
  return(bWorkDone);
}

bool UpperDistance::xAdjacent(UInt i, UInt j) const {
  return(operator[](i)[j] == m_vComponents[i]->m_Dims.m_nWidth);
}

bool UpperDistance::yAdjacent(UInt i, UInt j,
			      const std::vector<UInt>& vSliced) const {
  UInt nMax(0);
  bool bStacked(false);
  if(m_vComponents[i]->m_nY +
     (int) m_vComponents[i]->m_Dims.m_nHeight ==
     m_vComponents[j]->m_nY) {
    bStacked = true;
    nMax = vSliced[m_vComponents[j]->m_nY];
  }
  else if(m_vComponents[j]->m_nY +
	  (int) m_vComponents[j]->m_Dims.m_nHeight ==
	  m_vComponents[i]->m_nY) {
    bStacked = true;
    nMax = vSliced[m_vComponents[i]->m_nY];
  }
  return(bStacked &&
	 get(i, j) < (m_vComponents[i]->m_Dims.m_nWidth + nMax));
}

bool UpperDistance::yAdjacent(UInt i, UInt j) const {
  return(get(i, j) <= m_vComponents[i]->m_Dims.m_nWidth &&
	 ((m_vComponents[i]->m_nY +
	   (int) m_vComponents[i]->m_Dims.m_nHeight ==
	   m_vComponents[j]->m_nY) ||
	  (m_vComponents[j]->m_nY +
	   (int) m_vComponents[j]->m_Dims.m_nHeight ==
	   m_vComponents[i]->m_nY)));
}

bool UpperDistance::canFill(UInt i, UInt j,
			    const std::vector<std::set<UInt> >& vBitmap) const {

  /**
   * Compute the interval of Y-overlap between the two components
   * first.
   */

  UInt nStart, nEnd;
  m_vComponents[i]->yOverlap(*m_vComponents[j], nStart, nEnd);

  /**
   * For that narrow space, see if we can fill that space with another
   * square.
   */

  return(canFill(nStart, nEnd, i, j,
		 operator[](i)[j] - m_vComponents[i]->m_Dims.m_nWidth,
		 vBitmap));
}

bool UpperDistance::canFill(UInt nStart, UInt nEnd, UInt n1, UInt n2,
			    UInt nSpace,
			    const std::vector<std::set<UInt> >& vBitmap) const {
  for(UInt i = nStart; i < nEnd; ++i)
    if(allGreaterThan(n1, n2, nSpace, vBitmap[i]))
      return(false);
  return(true);
}

bool UpperDistance::yOverlaps(UInt i, UInt j) const {
  return(m_vComponents[i]->yOverlaps(*m_vComponents[j]));
}

void UpperDistance::print() const {
  UInt nMax(std::max(m_vComponents.size(), m_vComponentMap.size()));
  std::cout << " Y  intid c->id" << std::endl;
  for(UInt i = 0; i < nMax; ++i) {
    std::cout << std::setw(2) << i << ": ";
    if(i < m_vComponentMap.size())
      if(m_vComponentMap[i] == std::numeric_limits<UInt>::max())
	std::cout << "     ";
      else
	std::cout << std::setw(5) << (int) m_vComponentMap[i];
    else
      std::cout << "     ";
    if(i < m_vComponents.size())
      std::cout << std::setw(5) << m_vComponents[i]->m_nID;
    else
      std::cout << "     ";
    std::cout << std::endl;
  }

  std::cout << std::endl << "Upper Bound Matrix:" << std::endl;
  std::cout << "  ";
  for(int x = 0; x < (int) size(); ++x)
    if(x % 5 > 0) std::cout << "   ";
    else std::cout << std::setw(3) << x;
  std::cout << std::endl;
  for(UInt i = 0; i < size(); ++i) {
    std::cout << std::setw(2) << i << " ";
    for(UInt j = 0; j < size(); ++j)
      std::cout << std::setw(2) << (int) operator[](i)[j] << " ";
    std::cout << std::endl;
  }
}
