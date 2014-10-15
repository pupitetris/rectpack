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

#include "Bins.h"
#include "Grid.h"
#include "GridViz.h"
#include "Inferences.h"
#include "Printer.h"
#include <algorithm>
#include <iomanip>
#include <sstream>

Inferences::Inferences() :
  m_bAdjacency(true),
  m_bDistance(true),
  m_bSubsetSums(true),
  m_bComposition(true),
  m_bEmptyComposition(true) {
}

Inferences::~Inferences() {
}

void Inferences::clear() {
  m_Tops.clear();
  m_Bottoms.clear();
  m_vBitmap.clear();
  m_vComponents.clear();
  m_TopSymmetries.clear();
  m_BottomSymmetries.clear();
  m_vBottomsAdjacent.clear();
  m_lDomination.clear();
}

void Inferences::initialize(const Bins& vBins,
			    RectPtrArray::iterator i,
			    RectPtrArray::const_iterator j) {
  m_vEmptySpace.clear(); m_vEmptySpace.resize(vBins.size());
  for(UInt k = 0; k < m_vEmptySpace.size(); ++k)
    m_vEmptySpace[k] = (UInt) vBins[k];
  m_Tops.clear();
  m_Bottoms.clear();
  m_vBitmap.clear();
  m_vBitmap.resize(vBins.size());
  m_vComponents.clear();
  m_vComponents.resize(j - i, NULL);
  m_TopSymmetries.clear();
  m_BottomSymmetries.clear();
  m_X = Adjacency(j - i);
  m_Y = Adjacency(j - i);
  m_vBottomsAdjacent.clear();
  m_vBottomsAdjacent.resize(vBins.size(), false);
  m_lDomination.clear();
  for(UInt k = 0; i != j; ++i, ++k) {

    /**
     * Set the component intoparameters.
     */

    m_vComponents[k] = new Component();
    m_vComponents[k]->m_nID = k;
    m_vComponents[k]->m_nY = (*i)->y;
    m_vComponents[k]->m_Dims.initialize((*i)->m_nWidth, (*i)->m_nHeight);
    m_vComponents[k]->m_pRect = *i;

    /**
     * Draw the component into our bitmap.
     */

    for(UInt l = (*i)->y; l < (*i)->y + (*i)->m_nHeight; ++l)
      m_vBitmap[l].insert(k);
  }

  m_Tops.initializey1(m_vComponents);
  m_Bottoms.initializey2(m_vComponents);
}

void Inferences::findSymmetries() {
  m_TopSymmetries.initialize(m_Tops, m_vComponents);
  m_BottomSymmetries.initialize(m_Bottoms, m_vComponents);
}

void Inferences::compute() {

  /**
   * Applies all inferences rules until we've reached a steady
   * state. All knowledge is stored in our bitmap, top/bottom data
   * structures, and adjacency graph.
   */

  while((m_bEmptyComposition && consolidateEmptyColumns()) ||
	(m_bEmptyComposition && consolidateEmptyRows()) ||
	computeNonEmpty());
}

bool Inferences::computeNonEmpty() {
  return((m_bComposition && consolidateComponentsV()) ||
	 (m_bComposition && consolidateComponentsH()) ||
	 (m_bSubsetSums && partitioning()) ||
	 (m_bSubsetSums && boundsAdjacency()));
}

bool Inferences::consolidateComponentsV() {

  /**
   * Scan across all bins, taking a look to see if there are exactly
   * only two items one bordering the other AND that they are of the
   * same width.
   */

  for(UInt i = 0; i < m_vEmptySpace.size() - 1; ++i) {
    if(m_vEmptySpace[i] == 0 && m_vEmptySpace[i + 1] == 0) {
      if(m_Bottoms[i].size() == 1 && m_Tops[i + 1].size() == 1 &&
	 canGroupV(*m_Bottoms[i].begin(), *m_Tops[i + 1].begin())) {
	replace(*m_Bottoms[i].begin(), *m_Tops[i + 1].begin(),
		combineV(*m_Bottoms[i].begin(), *m_Tops[i + 1].begin()));
	return(true);
      }
    }
    else ++i; // We can actually skip an extra index.
  }
  return(false);
}

bool Inferences::consolidateComponentsH() {
  
  /**
   * Iterate over all X-adjacent edges and find two top-level
   * components who have the same heights and the same Y-values. Note
   * that all components in our data structures are top-level already,
   * except for those in m_vComponents.
   */

  boost::graph_traits<Adjacency>::edge_iterator ei, ei_end;
  for(boost::tie(ei, ei_end) = boost::edges(m_X); ei != ei_end; ++ei) {
    UInt n1(boost::source(*ei, m_X)), n2(boost::target(*ei, m_X));
    if((m_vComponents[n1]->m_nY != m_vComponents[n2]->m_nY) ||
       (m_vComponents[n1]->m_Dims.m_nHeight != m_vComponents[n2]->m_Dims.m_nHeight))
      continue;
    replace(n1, n2, combineH(n1, n2));
    return(true);
  }

  /**
   * Iterate over all of the vertices and see if any have more than
   * one Y-adjacent relation.
   */

  for(UInt i = 0; i < m_vComponents.size(); ++i) {
    if(boost::out_degree(i, m_Y) < 2) continue;
    boost::graph_traits<Adjacency>::adjacency_iterator vi, vi_end;
    boost::tie(vi, vi_end) = boost::adjacent_vertices(i, m_Y);
    UInt nHeight = m_vComponents[*vi]->m_Dims.m_nHeight;
    UInt y = m_vComponents[*vi]->m_nY;
    UInt nWidth = m_vComponents[*vi]->m_Dims.m_nWidth;
    std::list<UInt> l;
    l.push_back(*vi);
    for(++vi; vi != vi_end; ++vi) {
      if(m_vComponents[*vi]->m_Dims.m_nHeight != nHeight ||
	 m_vComponents[*vi]->m_nY != (int) y) break;
      else {
	nWidth += m_vComponents[*vi]->m_Dims.m_nWidth;
	l.push_back(*vi);
      }
    }
    if((vi == vi_end) && (nWidth == m_vComponents[i]->m_Dims.m_nWidth)) {
      replace(l, combineH(l));
      return(true);
    }
  }
  return(false);
}

bool Inferences::consolidateEmptyColumns() {

  /**
   * Scan across the bins, and first look for chains of empty cells
   * that must border each other (i.e. that have no other top/bottom
   * squares to border them). We trigger a chain when we find a single
   * empty cell that has no bottom border other than another empty
   * cell.
   */

  bool bWorkDone = false;
  for(UInt i = 0; i < m_vEmptySpace.size() - 1; ++i) {
    if(m_vEmptySpace[i] > 0 && m_Bottoms[i].empty()) {
      UInt j = i + 1;
      while(j < m_vEmptySpace.size() && m_vEmptySpace[j] > 0 &&
	    m_Tops[j].empty()) ++j;
      if(j - i > 1) {
	bWorkDone = true;
	newComponent();
	m_vComponents.back()->m_nY = i;
	m_vComponents.back()->m_Dims.initialize(1, j - i);
	m_vComponents.back()->m_bEmpty = true;
	for(UInt k = i; k < j; ++k)
	  --m_vEmptySpace[k];
	addComponent(m_vComponents.back()->m_nID);
	i = j - 1;
      }
    }
  }
  return(bWorkDone);
}

bool Inferences::consolidateEmptyRows() {
  for(UInt i = 0; i < m_vEmptySpace.size(); ++i) {
    if(m_vEmptySpace[i] > 0) {

      /**
       * If there are no squares that have their top borders in this
       * bin, then that means that these empty squares will act as the
       * only top borders for the bottom borders in bin i-1.
       */

      if(i > 1 && m_Tops[i].empty() && !m_Bottoms[i - 1].empty()) {
	for(std::set<UInt>::iterator j = m_Bottoms[i - 1].begin();
	    j != m_Bottoms[i - 1].end(); ++j) {

	  /**
	   * Represent the empty strip.
	   */

	  newComponent();
	  m_vComponents.back()->m_nY = i;
	  m_vComponents.back()->m_Dims.initialize(m_vComponents[*j]->m_Dims.m_nWidth, 1);
	  m_vComponents.back()->m_bEmpty = true;
	  m_vEmptySpace[i] -= m_vComponents[*j]->m_Dims.m_nWidth;

	  /**
	   * Now combine the strip with whatever square it is adjacent
	   * to. This effectively glues these two objects together.
	   */

	  UInt n = m_vComponents.size() - 1;
	  replace(n, *j, combineV(n, *j));
	  return(true);
	}
      }

      /**
       * If there are no squares that have their bottom borders in
       * this bin, then that means that these empty squares will act
       * as the only bottom borders for the top borders in bin i+1.
       */

      if((i < m_vEmptySpace.size() - 1) && m_Bottoms[i].empty() &&
	 !m_Tops[i + 1].empty()) {
	for(std::set<UInt>::iterator j = m_Tops[i + 1].begin();
	    j != m_Tops[i + 1].end(); ++j) {

	  /**
	   * Represent the empty strip.
	   */

	  newComponent();
	  m_vComponents.back()->m_nY = i;
	  m_vComponents.back()->m_Dims.initialize(m_vComponents[*j]->m_Dims.m_nWidth, 1);
	  m_vComponents.back()->m_bEmpty = true;

	  /**
	   * Remove the empty cells from our empty cell array.
	   */

	  m_vEmptySpace[i] -= m_vComponents.back()->m_Dims.m_nWidth;

	  /**
	   * Now combine the strip with whatever square it is adjacent
	   * to. This effectively glues these two objects together.
	   */

	  UInt n = m_vComponents.size() - 1;
	  replace(n, *j, combineV(n, *j));
	  return(true);
	}
      }
    }
  }
  return(false);
}

UInt Inferences::combineH(UInt n1, UInt n2) {
  newComponent();
  m_vComponents.back()->m_bEmpty =
    m_vComponents[n1]->m_bEmpty && m_vComponents[n2]->m_bEmpty;
  m_vComponents.back()->m_nY = m_vComponents[n1]->m_nY;
  m_vComponents.back()->m_Dims.initialize(m_vComponents[n1]->m_Dims.m_nWidth +
					  m_vComponents[n2]->m_Dims.m_nWidth,
					  m_vComponents[n1]->m_Dims.m_nHeight);
  m_vComponents.back()->m_Members.insert(m_vComponents[n1]);
  m_vComponents.back()->m_Members.insert(m_vComponents[n2]);
  m_vComponents[n1]->m_bTopLevel = false;
  m_vComponents[n2]->m_bTopLevel = false;
  return(m_vComponents.size() - 1);
}

UInt Inferences::combineH(const std::list<UInt>& l) {
  newComponent();
  std::list<UInt>::const_iterator i = l.begin();
  m_vComponents.back()->m_bEmpty = m_vComponents[*i]->m_bEmpty;
  m_vComponents.back()->m_nY = m_vComponents[*i]->m_nY;
  m_vComponents.back()->m_Dims.initialize(m_vComponents[*i]->m_Dims.m_nWidth,
					  m_vComponents[*i]->m_Dims.m_nHeight);
  m_vComponents.back()->m_Members.insert(m_vComponents[*i]);
  m_vComponents[*i]->m_bTopLevel = false;
  for(++i; i != l.end(); ++i) {
    m_vComponents.back()->m_bEmpty &= m_vComponents[*i]->m_bEmpty;
    m_vComponents.back()->m_Dims.m_nWidth += m_vComponents[*i]->m_Dims.m_nWidth;
    m_vComponents.back()->m_Members.insert(m_vComponents[*i]);
    m_vComponents[*i]->m_bTopLevel = false;
  }
  return(m_vComponents.size() - 1);
}

UInt Inferences::combineV(UInt n1, UInt n2) {
  newComponent();
  m_vComponents.back()->m_bEmpty =
    m_vComponents[n1]->m_bEmpty && m_vComponents[n2]->m_bEmpty;
  m_vComponents.back()->m_nY =
    std::min(m_vComponents[n1]->m_nY, m_vComponents[n2]->m_nY);
  m_vComponents.back()->m_Dims.initialize(m_vComponents[n1]->m_Dims.m_nWidth,
					  m_vComponents[n1]->m_Dims.m_nHeight +
					  m_vComponents[n2]->m_Dims.m_nHeight);
  m_vComponents.back()->m_Members.insert(m_vComponents[n1]);
  m_vComponents.back()->m_Members.insert(m_vComponents[n2]);
  m_vComponents[n1]->m_bTopLevel = false;
  m_vComponents[n2]->m_bTopLevel = false;
  return(m_vComponents.size() - 1);
}

void Inferences::replace(const std::list<UInt>& l, UInt nNew) {

  /**
   * Remove the old components.
   */

  for(std::list<UInt>::const_iterator i = l.begin(); i != l.end(); ++i) {
    m_Tops[m_vComponents[*i]->m_nY].erase(*i);
    m_Bottoms[m_vComponents[*i]->m_nY + m_vComponents[*i]->m_Dims.m_nHeight - 1].erase(*i);
    for(UInt j = m_vComponents[*i]->m_nY;
	j < m_vComponents[*i]->m_nY + m_vComponents[*i]->m_Dims.m_nHeight; ++j)
      m_vBitmap[j].erase(*i);
  }

  /**
   * Add in the new one.
   */
  
  addComponent(nNew);

  /**
   * Create new vertices to represent the adjacency relations, and
   * perform the necessary remapping.
   */

  remap(l, nNew, m_X);
  remap(l, nNew, m_Y);
}

void Inferences::replace(UInt n1, UInt n2, UInt nNew) {

  /**
   * Remove the previous components from the top/bottom arrays as well
   * as from the bitmap.
   */

  m_Tops[m_vComponents[n1]->m_nY].erase(n1);
  m_Tops[m_vComponents[n2]->m_nY].erase(n2);
  m_Bottoms[m_vComponents[n1]->m_nY + m_vComponents[n1]->m_Dims.m_nHeight - 1].erase(n1);
  m_Bottoms[m_vComponents[n2]->m_nY + m_vComponents[n2]->m_Dims.m_nHeight - 1].erase(n2);
  for(UInt i = m_vComponents[n1]->m_nY;
      i < m_vComponents[n1]->m_nY + m_vComponents[n1]->m_Dims.m_nHeight; ++i)
    m_vBitmap[i].erase(n1);
  for(UInt i = m_vComponents[n2]->m_nY;
      i < m_vComponents[n2]->m_nY + m_vComponents[n2]->m_Dims.m_nHeight; ++i)
    m_vBitmap[i].erase(n2);

  /**
   * Add in the new one.
   */
  
  addComponent(nNew);

  /**
   * Create new vertices to represent the adjacency relations, and
   * perform the necessary remapping.
   */

  boost::add_vertex(m_X);
  boost::add_vertex(m_Y);
  boost::remove_edge(n1, n2, m_X);
  boost::remove_edge(n1, n2, m_Y);
  remap(n1, nNew, m_X);
  remap(n2, nNew, m_X);
  remap(n1, nNew, m_Y);
  remap(n2, nNew, m_Y);
}

void Inferences::remap(UInt nOld, UInt nNew, Adjacency& g) {
  boost::graph_traits<Adjacency>::adjacency_iterator vi, vi_end;
  for(boost::tie(vi, vi_end) = boost::adjacent_vertices(nOld, g); vi != vi_end; ++vi)
    if(!boost::edge(*vi, nNew, g).second)
      boost::add_edge(*vi, nNew, g);
  boost::clear_vertex(nOld, g);
}

void Inferences::remap(const std::list<UInt>& l, UInt nNew,
		       Adjacency& g) {
  for(std::list<UInt>::const_iterator i = l.begin(); i != l.end(); ++i) {
    boost::graph_traits<Adjacency>::adjacency_iterator vi, vi_end;
    for(boost::tie(vi, vi_end) = boost::adjacent_vertices(*i, g); vi != vi_end; ++vi)
      if(!boost::edge(*vi, nNew, g).second)
	boost::add_edge(*vi, nNew, g);
  }
  for(std::list<UInt>::const_iterator i = l.begin(); i != l.end(); ++i)
    boost::clear_vertex(*i, g);
}


bool Inferences::canGroupH(UInt n1, UInt n2) {
  return(m_vComponents[n1]->m_Dims.m_nHeight == m_vComponents[n2]->m_Dims.m_nHeight);
}

bool Inferences::canGroupV(UInt n1, UInt n2) {
  return(m_vComponents[n1]->m_Dims.m_nWidth == m_vComponents[n2]->m_Dims.m_nWidth);
}

bool Inferences::partitioning(UInt nBottom, UInt nTop) {
  UInt nBottomItems = m_Bottoms[nBottom].size() + m_vEmptySpace[nBottom];
  UInt nTopItems = m_Tops[nTop].size() + m_vEmptySpace[nTop];
  if((nBottomItems > 2 && nTopItems > 2) ||
     (m_Bottoms[nBottom].empty() && m_Tops[nTop].empty()))
    return(false);
  if(nBottomItems == 1 || nTopItems == 1) {
    m_vBottomsAdjacent[nBottom] = true;
    return(updateAdjacencyList(nBottomItems, nTopItems, m_Bottoms[nBottom],
			       m_Tops[nTop]));
  }
  
  /**
   * Which side has exactly two elements? Pick the smallest square out
   * of that pair.
   */

  UInt nSmallestSquare = std::numeric_limits<UInt>::max();
  bool bSquareInBottom = false;
  if(nTopItems == 2) {
    if(m_vEmptySpace[nTop] > 0)
      nSmallestSquare = 1;
    else
      for(std::set<UInt>::const_iterator i = m_Tops[nTop].begin();
	  i != m_Tops[nTop].end(); ++i)
	nSmallestSquare = std::min(m_vComponents[*i]->m_Dims.m_nWidth,
				   nSmallestSquare);
  }
  if(nBottomItems == 2) {
    if(m_vEmptySpace[nBottom] > 0) {
      nSmallestSquare = 1;
      bSquareInBottom = true;
    }
    else {
      for(std::set<UInt>::const_iterator i = m_Bottoms[nBottom].begin();
	  i != m_Bottoms[nBottom].end(); ++i)
	if(m_vComponents[*i]->m_Dims.m_nWidth < nSmallestSquare) {
	  nSmallestSquare = m_vComponents[*i]->m_Dims.m_nWidth;
	  bSquareInBottom = true;
	}
    }
  }

  /**
   * Now we have the smallest square and we can try finding a subset
   * in the other border that sums to exactly this small square.
   */
  
  std::set<UInt>::const_iterator ib, ie;
  UInt nExcluded(0);
  if(bSquareInBottom) {
    excludeWidth(m_Tops[nTop], ib, ie, nExcluded);
    if(hasSubset(nSmallestSquare, 0, m_vEmptySpace[nTop] + nExcluded,
		 ib, ie))
      return(false);
    else {
      m_vBottomsAdjacent[nBottom] = true;
      return(updateAdjacencyList(nBottomItems, nTopItems, m_Bottoms[nBottom],
				 m_Tops[nTop]));
    }
  }
  else {
    excludeWidth(m_Bottoms[nBottom], ib, ie, nExcluded);
    if(hasSubset(nSmallestSquare, 0,
		 m_vEmptySpace[nBottom] + nExcluded, ib, ie))
      return(false);
    else {
      m_vBottomsAdjacent[nBottom] = true;
      return(updateAdjacencyList(nBottomItems, nTopItems, m_Bottoms[nBottom],
				 m_Tops[nTop]));
    }
  }
}

bool Inferences::partitioning() {
  bool bWorkDone = false;
  for(UInt i = 0; i < m_vEmptySpace.size() - 1; ++i)
    if(partitioning(i, i + 1))
      bWorkDone = true;
  return(bWorkDone);
}

bool Inferences::boundsAdjacency() {
  bool bWorkDone = false;
  for(UInt i = 0; i < m_vEmptySpace.size() - 1; ++i)
    if(boundsAdjacency(i, i + 1))
      bWorkDone = true;
  return(bWorkDone);
}

bool Inferences::boundsAdjacency(UInt nBottom, UInt nTop) {

  /**
   * Get the border length.
   */

  UInt nBorder = m_vEmptySpace[nBottom];
  for(std::set<UInt>::iterator i = m_Bottoms[nBottom].begin();
      i != m_Bottoms[nBottom].end(); ++i)
    nBorder += m_vComponents[*i]->m_Dims.m_nWidth;

  /**
   * Pick the largest square out of the two. We will reason only about
   * the other side.
   */

  UInt nLargest = 0;
  bool bBottom = false;
  for(std::set<UInt>::const_iterator i = m_Bottoms[nBottom].begin();
      i != m_Bottoms[nBottom].end(); ++i)
    if(m_vComponents[*i]->m_Dims.m_nWidth > nLargest) {
      nLargest = m_vComponents[*i]->m_Dims.m_nWidth;
      bBottom = true;
    }
  for(std::set<UInt>::const_iterator i = m_Tops[nTop].begin();
      i != m_Tops[nTop].end(); ++i)
    if(m_vComponents[*i]->m_Dims.m_nWidth > nLargest) {
      nLargest = m_vComponents[*i]->m_Dims.m_nWidth;
      bBottom = false;
    }

  return(boundsAdjacency(nLargest, nBorder - nLargest, bBottom,
			 bBottom ? m_Tops[nTop] : m_Bottoms[nBottom]));
}

bool Inferences::boundsAdjacency(UInt nWidth, UInt nRest, bool bBottom,
				 const std::set<UInt>& s) {

  /**
   * The candidates data structure will map, for each item in the
   * given set, the ID of that item to a value representing the
   * minimum overlap of this item to the square (on the other side)
   * represented by nWidth. We only insert items if there is a
   * guaranteed overlap.
   */

  std::map<int, int> candidates; // Component ID and minimum overlap.
  for(std::set<UInt>::iterator i = s.begin(); i != s.end(); ++i)
    if(m_vComponents[*i]->m_Dims.m_nWidth > nRest)
      candidates[*i] = m_vComponents[*i]->m_Dims.m_nWidth - nRest;
  if(candidates.size() == 2) {
    UInt n1 = candidates.begin()->first;
    UInt n2 = candidates.rbegin()->first;
    UInt nSpace = nWidth - candidates.begin()->second - candidates.rbegin()->second;
    UInt nStart, nEnd;
    if(bBottom) {
      nStart = m_vComponents[n1]->m_nY;
      nEnd = std::min(m_vComponents[n1]->m_nY + m_vComponents[n1]->m_Dims.m_nHeight,
		      m_vComponents[n2]->m_nY + m_vComponents[n2]->m_Dims.m_nHeight);
    }
    else {
      nStart = std::max(m_vComponents[n1]->m_nY, m_vComponents[n2]->m_nY);
      nEnd = m_vComponents[n1]->m_nY + m_vComponents[n1]->m_Dims.m_nHeight;
    }

    /**
     * If we can fill in the gap, then we can't claim adjacency
     * between n1 and n2. Otherwise, we can assert their X-adjacency.
     */

    if(canFill(nStart, nEnd, n1, n2, nSpace) ||
       boost::edge(n1, n2, m_X).second)
      return(false);
    else {
      boost::add_edge(n1, n2, m_X);
      return(true);
    }
  }
  else
    return(false);
}

bool Inferences::canFill(UInt nStart, UInt nEnd, UInt n1, UInt n2,
			 UInt nSpace) {
  for(UInt i = nStart; i < nEnd; ++i) {

    /**
     * The pruning rule is that if there is no empty space and no
     * square can fit (the lower bound is true over all squares), then
     * we can prune.
     */

    if(m_vEmptySpace[i] == 0 &&
       allGreaterThan(n1, n2, nSpace, m_vBitmap[i]))
      return(false);
  }
  return(true);
}

bool Inferences::allGreaterThan(UInt nIgnore1, UInt nIgnore2, UInt nMin,
				const std::set<UInt>& s) {
  for(std::set<UInt>::const_iterator i = s.begin(); i != s.end(); ++i)
    if(*i != nIgnore1 && *i != nIgnore2 &&
       m_vComponents[*i]->m_Dims.m_nWidth <= nMin)
      return(false);
  return(true);
}

bool Inferences::hasSubset(UInt nTarget, UInt nCurrent, UInt nEmpty,
			   std::set<UInt>::const_iterator iBegin,
			   std::set<UInt>::const_iterator iEnd) const {
  if(nCurrent >= nTarget)
    return(nTarget == nCurrent);
  if(iBegin == iEnd)
    return(nEmpty >= nTarget - nCurrent);
  for(; iBegin != iEnd; ++iBegin) {
    std::set<UInt>::const_iterator iNext(iBegin);
    if(hasSubset(nTarget, nCurrent + m_vComponents[*iBegin]->m_Dims.m_nWidth,
		 nEmpty, ++iNext, iEnd) ||
       hasSubset(nTarget, nCurrent, nEmpty, iNext, iEnd))
      return(true);
  }
  return(nEmpty >= nTarget - nCurrent);
}

bool Inferences::updateAdjacencyList(UInt nBottomItems,
				     UInt nTopItems,
				     const std::set<UInt>& s1,
				     const std::set<UInt>& s2) {
  bool bWorkDone = false;
  if((nBottomItems == 2 && s1.size() == 2) &&
     (!boost::edge(*s1.begin(), *s1.rbegin(), m_X).second)) {
    bWorkDone = true;
    boost::add_edge(*s1.begin(), *s1.rbegin(), m_X);
  }
  if((nTopItems == 2 && s2.size() == 2) &&
     (!boost::edge(*s2.begin(), *s2.rbegin(), m_X).second)) {
    bWorkDone = true;
    boost::add_edge(*s2.begin(), *s2.rbegin(), m_X);
  }
  if(nBottomItems == 1 && s1.size() == 1)
    for(std::set<UInt>::const_iterator i = s2.begin();
	i != s2.end(); ++i)
      if(!boost::edge(*s1.begin(), *i, m_Y).second) {
	bWorkDone = true;
	boost::add_edge(*s1.begin(), *i, m_Y);
      }
  if(nTopItems == 1 && s2.size() == 1)
    for(std::set<UInt>::const_iterator i = s1.begin();
	i != s1.end(); ++i)
      if(!boost::edge(*s2.begin(), *i, m_Y).second) {
	bWorkDone = true;
	boost::add_edge(*s2.begin(), *i, m_Y);
      }
  return(bWorkDone);
}

void Inferences::print2() const {
  for(UInt i = 0; i < m_vComponents.size(); ++i) {
    m_vComponents[i]->print();
    std::cout << std::endl;
  }
  std::cout << std::endl;

  /**
   * Component indices...
   */

  std::cout << "Component Indices:" << std::endl;
  UInt nMaxElements(0), nMaxTop(0);
  for(UInt i = 0; i < m_vBitmap.size(); ++i) {
    nMaxElements = std::max(nMaxElements, (UInt) m_vBitmap[i].size());
    nMaxTop = std::max(nMaxTop, m_Tops.size(i));
  }
  UInt nMaxWidth = nMaxElements * 2 + (nMaxElements - 1) + 2;
  UInt nMaxTopWidth = nMaxTop * 5 + (nMaxTop - 1) + 2;
  UInt t1 = 10 + nMaxWidth;
  UInt t2 = t1 + 3 + nMaxTopWidth;
  for(UInt i = 0; i < m_vEmptySpace.size(); ++i) {
    std::ostringstream oss;
    oss << std::setw(2) << i
	<< " e=" << std::setw(2) << m_vEmptySpace[i]
	<< " c=";
    Printer::print(m_vBitmap[i], oss);
    while(oss.str().size() < t1)
      oss << " " << std::flush;
    oss << " t={";
    AlignedComponents::const_iterator k = m_Tops.find(i);
    if(k != m_Tops.end() && !k->second.empty()) {
      std::set<UInt>::const_iterator j = k->second.begin();
      oss << m_vComponents[*j]->m_Dims;
      for(++j; j != k->second.end(); ++j)
	oss << "," << m_vComponents[*j]->m_Dims;
    }
    oss << "}";
    while(oss.str().size() < t2)
      oss << " " << std::flush;
    oss << " b={";
    k = m_Bottoms.find(i);
    if(k != m_Bottoms.end() && !k->second.empty()) {
      std::set<UInt>::const_iterator j = k->second.begin();
      oss << m_vComponents[*j]->m_Dims;
      for(++j; j != k->second.end(); ++j)
	oss << "," << m_vComponents[*j]->m_Dims;
    }
    oss << "}";
    std::cout << oss.str() << std::endl;
  }
  std::cout << std::endl;

  /**
   * Symmetries...
   */

  std::cout << "Top Symmetries:" << std::endl;
  for(AlignedCongruencies::const_iterator i = m_TopSymmetries.begin();
      i != m_TopSymmetries.end(); ++i) {
    std::cout << std::setw(2) << i->first << ": ";
    i->second.print();
    std::cout << std::endl;
  }
  std::cout << std::endl;
  std::cout << "Bottom Symmetries:" << std::endl;
  for(AlignedCongruencies::const_iterator i = m_BottomSymmetries.begin();
      i != m_BottomSymmetries.end(); ++i) {
    std::cout << std::setw(2) << i->first << ": ";
    i->second.print();
    std::cout << std::endl;
  }
  std::cout << std::endl;

  /**
   * Adjacency graphs...
   */

  std::cout << "Adjacency Graphs:" << std::endl;
  UInt nMaxX = 0;
  boost::graph_traits<Adjacency>::vertex_iterator vi, vi_end;
  for(tie(vi, vi_end) = boost::vertices(m_X); vi != vi_end; ++vi)
    nMaxX = std::max(nMaxX, (UInt) boost::out_degree(*vi, m_X));
  UInt nMaxXWidth = nMaxX * 5 + nMaxX - 1 + 2;
  t1 = 8 + nMaxXWidth;
  for(UInt i = 0; i < m_vComponents.size(); ++i) {
    std::ostringstream oss;
    oss << std::setw(5) << m_vComponents[i]->m_Dims
	<< " x={";
    if(boost::out_degree(i, m_X) > 0) {
      boost::graph_traits<Adjacency>::adjacency_iterator ji, ji_end;
      tie(ji, ji_end) = boost::adjacent_vertices(i, m_X);
      oss << m_vComponents[*ji]->m_Dims;
      for(++ji; ji != ji_end; ++ji)
	oss << "," << m_vComponents[*ji]->m_Dims;
    }
    oss << "}";
    while(oss.str().size() < t1)
      oss << " ";
    oss << " y={";
    if(boost::out_degree(i, m_Y) > 0) {
      boost::graph_traits<Adjacency>::adjacency_iterator ji, ji_end;
      tie(ji, ji_end) = boost::adjacent_vertices(i, m_Y);
      oss << m_vComponents[*ji]->m_Dims;
      for(++ji; ji != ji_end; ++ji)
	oss << "," << m_vComponents[*ji]->m_Dims;
    }
    oss << "}";
    std::cout << oss.str() << std::endl;
  }
  std::cout << std::endl;

  /**
   * Adjacent bottom borders.
   */

  std::cout << "Adjacent Bottom Borders..." << std::endl;
  for(UInt i = 0; i < m_vBottomsAdjacent.size(); ++i)
    std::cout << std::setw(2) << i << ": "
	      << (m_vBottomsAdjacent[i] ? "Yes" : "No") << std::endl;
}

void Inferences::print() const {
  for(UInt i = 0; i < m_vComponents.size(); ++i) {
    m_vComponents[i]->print();
    std::cout << std::endl;
  }
  std::cout << std::endl;

  /**
   * Component indices...
   */

  std::cout << "Component Indices:" << std::endl;
  UInt nMaxElements(0), nMaxTop(0);
  for(UInt i = 0; i < m_vBitmap.size(); ++i) {
    nMaxElements = std::max(nMaxElements, (UInt) m_vBitmap[i].size());
    nMaxTop = std::max(nMaxTop, m_Tops.size(i));
  }
  UInt nMaxWidth = nMaxElements * 2 + (nMaxElements - 1) + 2;
  UInt nMaxTopWidth = nMaxTop * 2 + (nMaxTop - 1) + 2;
  UInt t1 = 10 + nMaxWidth;
  UInt t2 = t1 + 3 + nMaxTopWidth;
  for(UInt i = 0; i < m_vEmptySpace.size(); ++i) {
    std::ostringstream oss;
    oss << std::setw(2) << i
	<< " e=" << std::setw(2) << m_vEmptySpace[i]
	<< " c=";
    Printer::print(m_vBitmap[i], oss);
    while(oss.str().size() < t1)
      oss << " " << std::flush;
    oss << " t=";
    AlignedComponents::const_iterator j = m_Tops.find(i);
    if(j == m_Tops.end()) oss << "{}";
    else Printer::print(j->second, oss);
    while(oss.str().size() < t2)
      oss << " " << std::flush;
    oss << " b=";
    j = m_Bottoms.find(i);
    if(j == m_Bottoms.end()) oss << "{}";
    else Printer::print(j->second, oss);
    std::cout << oss.str() << std::endl;
  }
  std::cout << std::endl;

  /**
   * Symmetries...
   */

  std::cout << "Top Symmetries:" << std::endl;
  for(AlignedCongruencies::const_iterator i =
	m_TopSymmetries.begin(); i != m_TopSymmetries.end(); ++i) {
    std::cout << std::setw(2) << i->first << ": ";
    i->second.print(std::cout);
    std::cout << std::endl;
  }
  std::cout << std::endl;
  std::cout << "Bottom Symmetries:" << std::endl;
  for(AlignedCongruencies::const_iterator i =
	m_BottomSymmetries.begin(); i != m_BottomSymmetries.end(); ++i) {
    std::cout << std::setw(2) << i->first << ": ";
    i->second.print(std::cout);
    std::cout << std::endl;
  }
  std::cout << std::endl;

  /**
   * Adjacency graphs...
   */

  std::cout << "Adjacency Graphs:" << std::endl;
  UInt nMaxX = 0;
  boost::graph_traits<Adjacency>::vertex_iterator vi, vi_end;
  for(tie(vi, vi_end) = boost::vertices(m_X); vi != vi_end; ++vi)
    nMaxX = std::max(nMaxX, (UInt) boost::out_degree(*vi, m_X));
  UInt nMaxXWidth = nMaxX * 2 + nMaxX - 1 + 2;
  t1 = 5 + nMaxXWidth;
  for(UInt i = 0; i < m_vComponents.size(); ++i) {
    std::ostringstream oss;
    oss << std::setw(2) << i
	<< " x={";
    if(boost::out_degree(i, m_X) > 0) {
      boost::graph_traits<Adjacency>::adjacency_iterator ji, ji_end;
      tie(ji, ji_end) = boost::adjacent_vertices(i, m_X);
      oss << *ji;
      for(++ji; ji != ji_end; ++ji)
	oss << "," << *ji;
    }
    oss << "}";
    while(oss.str().size() < t1)
      oss << " ";
    oss << " y={";
    if(boost::out_degree(i, m_Y) > 0) {
      boost::graph_traits<Adjacency>::adjacency_iterator ji, ji_end;
      tie(ji, ji_end) = boost::adjacent_vertices(i, m_Y);
      oss << *ji;
      for(++ji; ji != ji_end; ++ji)
	oss << "," << *ji;
    }
    oss << "}";
    std::cout << oss.str() << std::endl;
  }
  std::cout << std::endl;

  /**
   * Adjacent bottom borders.
   */

  std::cout << "Adjacent Bottom Borders..." << std::endl;
  for(UInt i = 0; i < m_vBottomsAdjacent.size(); ++i)
    std::cout << std::setw(2) << i << ": "
	      << (m_vBottomsAdjacent[i] ? "Yes" : "No") << std::endl;
}

int Inferences::newComponent() {
  boost::add_vertex(m_X);
  boost::add_vertex(m_Y);
  m_vComponents.push_back(new Component());
  return(m_vComponents.back()->m_nID = m_vComponents.size() - 1);
}

void Inferences::addComponent(UInt n) {
  m_Tops.addy1(m_vComponents[n]);
  m_Bottoms.addy2(m_vComponents[n]);
  for(UInt i = m_vComponents[n]->m_nY;
      i < m_vComponents[n]->m_nY + m_vComponents[n]->m_Dims.m_nHeight; ++i)
    m_vBitmap[i].insert(n);
}

void Inferences::emptyToComponents() {
  for(UInt i = 0; i < m_vEmptySpace.size(); ++i) {
    for(UInt j = 0; j < m_vEmptySpace[i]; ++j) {
      newComponent();
      m_vComponents.back()->m_nY = i;
      m_vComponents.back()->m_Dims.initialize(1, 1);
      m_vComponents.back()->m_bEmpty = true;
      addComponent(m_vComponents.back()->m_nID);
    }
    m_vEmptySpace[i] = 0;
  }
}

void Inferences::updateComponentAreas() {
  for(ComponentPtrs::iterator i = m_vComponents.begin();
      i != m_vComponents.end(); ++i)
    (*i)->m_Dims.setArea();
}

void Inferences::updateAdjacency(const BoxDimensions& b) {

  /**
   * Perform the computation (so long as we haven't disabled the
   * distance computation algorithms).
   */

  if(!m_bDistance) return;
  m_XDist.initialize(m_vComponents, b);
  m_XDist.compute(m_Tops, m_Bottoms, m_vBottomsAdjacent, m_vBitmap,
		  m_Y);

  /**
   * Extract all of the X-adjacency constraints.
   */

  for(UInt i = 0; i < m_XDist.size(); ++i)
    for(UInt j = i + 1; j < m_XDist.size(); ++j)
      if(m_XDist.xAdjacent(i, j) && 
	 m_XDist.xAdjacent(j, i) &&
	 m_XDist.yOverlaps(i, j)) {
	UInt m = m_XDist.m_vComponents[i]->m_nID;
	UInt n = m_XDist.m_vComponents[j]->m_nID;
	if(!boost::edge(m, n, m_X).second)
	  boost::add_edge(m, n, m_X);
      }
}

void Inferences::updateValidValues() {
}

void Inferences::updateDomination(const BoxDimensions& b) {

  /**
   * Single components all by themselves.
   */

  for(UInt i = 0; i < m_vComponents.size(); ++i)
    if(m_vComponents[i]->m_bTopLevel &&
       (m_vComponents[i]->m_Dims.m_nHeight == (UInt) b.m_nHeight))
      m_lDomination.push_back(std::pair<UInt, UInt>(i, i));

  /**
   * Pairs of components.
   */

  for(UInt i = 0; i < m_Bottoms.size() - 1; ++i) {
    if(!m_vBottomsAdjacent[i]) continue;
    std::list<Int> lTop, lBottom;
    if(m_BottomSymmetries.find(i) != m_BottomSymmetries.end())
      m_BottomSymmetries[i].hasHeight(i + 1, lBottom);
    if(m_TopSymmetries.find(i + 1) != m_TopSymmetries.end())
      m_TopSymmetries[i + 1].hasHeight(b.m_nHeight - i - 1, lTop);
    if(lBottom.empty() || lTop.empty()) continue;

    /**
     * The bottom border has exactly one element and it has the exact
     * size that we want. Also, the top border has at least one
     * element that spans the remaining height.
     */

    if(m_Bottoms[i].size() == 1 &&
       isBookEnd(lTop.front(), m_Tops[i + 1])) {
      m_lDomination.push_back(std::pair<UInt, UInt>(lBottom.front(),
						    lTop.front()));
      continue;
    }

    /**
     * Check to see if we've got a single element in the top border.
     */

    if(m_Tops[i + 1].size() == 1 &&
       isBookEnd(lBottom.front(), m_Bottoms[i])) {
      m_lDomination.push_back(std::pair<UInt, UInt>(lBottom.front(),
						    lTop.front()));
      continue;
    }

    /**
     * Check to see if there are at least two items in both
     * borders. If so, then we have to ensure that the one side has
     * the appropriate height on each endpoint of the sequence, and
     * the other side has at least one endpoint of the proper size.
     */

    if(lBottom.size() >= 2 && lTop.size() >= 2) {

      /**
       * Check to see if we have the required height on either
       * endpoint. This is done by following adjacency relations to
       * prove a sequence.
       */

      std::list<UInt> lBottomEnds, lTopEnds;
      surroundingHeight(i + 1, m_Bottoms[i], lBottomEnds);
      surroundingHeight(b.m_nHeight - i - 1, m_Tops[i + 1], lTopEnds);

      /**
       * We have to ensure that at least one side has two eligible
       * height elements.
       */

      if(lBottomEnds.size() < 2 && lTopEnds.size() < 2) continue;
      m_lDomination.push_back(std::pair<UInt, UInt>(lBottomEnds.front(),
						    lTopEnds.front()));

    }
  }
}

void Inferences::surroundingHeight(UInt h, const std::set<UInt>& s,
				   std::list<UInt>& l) const {

  /**
   * The map represents our histogram. The key is the component ID and
   * the value is going to be the number of edges that mention this
   * square and has both endpoints as members of the given set.
   */

  std::map<int, UInt> m;
  
  /**
   * The easiest way to do this is to iterate over every edge in our
   * adjacency graph, and check to ensure both endpoints are inside of
   * the given set.
   */

  boost::graph_traits<Adjacency>::edge_iterator ei, ei_end;
  for(boost::tie(ei, ei_end) = boost::edges(m_X); ei != ei_end; ++ei) {
    UInt n1(boost::source(*ei, m_X)), n2(boost::target(*ei, m_X));
    if(s.find(n1) != s.end() && s.find(n2) != s.end()) {
      ++m[n1]; ++m[n2];
    }
  }

  /**
   * Now we verify that exactly two have a count of 1 and everything
   * else must have a count of 2. So we keep a histogram.
   */

  std::vector<UInt> v(3, 0);
  for(std::map<int, UInt>::iterator i = m.begin(); i != m.end(); ++i) {
    ++v[i->second];
    if(i->second == 1 &&
       m_vComponents[i->first]->m_Dims.m_nHeight == h)
      l.push_back(i->first);
  }

  /**
   * We expect exactly 2 components will have just one relation and
   * the rest will have two relations.
   */

  if(v[1] != 2 || v[2] != (s.size() - 2))
    l.clear();
}

bool Inferences::checkRows() const {
  for(UInt i = 0; i < m_vBitmap.size(); ++i) {
    UInt nWidth(0);
    for(std::set<UInt>::const_iterator j = m_vBitmap[i].begin();
	j != m_vBitmap[i].end(); ++j)
      nWidth += m_vComponents[*j]->m_Dims.m_nWidth;
    nWidth += m_vEmptySpace[i];
    if(nWidth != (UInt) myb->m_nWidth) {
      std::cout << "Error at row " << i << std::endl;
      return(false);
    }
  }
  return(true);
}

bool Inferences::isBookEnd(UInt c, const std::set<UInt>& s) const {

  /**
   * The map represents our histogram. The key is the component ID and
   * the value is going to be the number of edges that mention this
   * square and has both endpoints as members of the given set.
   */

  std::map<int, UInt> m;
  
  /**
   * The easiest way to do this is to iterate over every edge in our
   * adjacency graph, and check to ensure both endpoints are inside of
   * the given set.
   */

  boost::graph_traits<Adjacency>::edge_iterator ei, ei_end;
  for(boost::tie(ei, ei_end) = boost::edges(m_X); ei != ei_end; ++ei) {
    UInt n1(boost::source(*ei, m_X)), n2(boost::target(*ei, m_X));
    if(s.find(n1) != s.end() && s.find(n2) != s.end()) {
      ++m[n1]; ++m[n2];
    }
  }

  /**
   * Now we verify that exactly two have a count of 1 and everything
   * else must have a count of 2. So we keep a histogram.
   */

  std::vector<UInt> v(3, 0);
  for(std::map<int, UInt>::iterator i = m.begin(); i != m.end(); ++i) {
    ++v[i->second];
  }

  /**
   * We expect exactly 2 components will have just one relation and
   * the rest will have two relations. Furthermore, the component
   * being queried will have only one relation.
   */

  return(v[1] == 2 && v[2] == (s.size() - 2) && m[c] == 1);
}

void Inferences::excludeWidth(const std::set<UInt>& s,
			      std::set<UInt>::const_iterator& iBegin,
			      std::set<UInt>::const_iterator& iEnd,
			      UInt& nExcluded) const {
  nExcluded = 0;
  iBegin = s.begin();
  for(; iBegin != s.end(); ++iBegin) {
    if(m_vComponents[*iBegin]->m_Dims.m_nWidth != 1) break;
    ++nExcluded;
  }
  if(iBegin == s.end()) return;
  iEnd = s.end(); --iEnd;
  for(; iEnd != iBegin; --iEnd) {
    if(m_vComponents[*iEnd]->m_Dims.m_nWidth != 1) break;
    ++nExcluded;
  }
}
