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

#include "BoxDimensions.h"
#include "DomDecorator.h"
#include "Domination.h"
#include "EmptyDom.h"
#include "HeapBox.h"
#include "HeightFirst.h"
#include "Packer.h"
#include "PackingCache.h"
#include "Parameters.h"
#include "Rectangle.h"
#include "RectDecArray.h"
#include "RectDecDeq.h"
#include "SquareDom.h"
#include "URectDom.h"
#include "WidthFirst.h"
#include <iomanip>

Domination::Domination() :
  m_pParams(NULL),
  m_pRects(NULL),
  m_pDuplicates(NULL),
  m_bEnabled(false) {
}

Domination::~Domination() {
}

bool Domination::enabled() const {
  return(m_bEnabled);
}

void Domination::initialize(const Parameters* pParams,
			    const RectPtrArray* pRects,
			    const DuplicateIndex* pDuplicates) {
  m_pParams = pParams;
  m_pRects = pRects;
  m_pDuplicates = pDuplicates;
  m_Time = 0;

  /**
   * Reset the data structures.
   */

  m_vTableW1.clear();
  m_vTableW1.resize(m_pParams->m_vInstance.size());
  m_vTableW2.clear();
  m_vTableW2.resize(m_pParams->m_vInstance.size());
  m_vTableH1.clear();
  m_vTableH1.resize(m_pParams->m_vInstance.size());
  m_vTableH2.clear();
  m_vTableH2.resize(m_pParams->m_vInstance.size());
  m_vGapH1.clear();
  m_vGapH1.resize(m_pParams->m_vInstance.size(), 0);
  m_vGapH2.clear();
  m_vGapH2.resize(m_pParams->m_vInstance.size(), 0);
  m_vGapW1.clear();
  m_vGapW1.resize(m_pParams->m_vInstance.size(), 0);
  m_vGapW2.clear();
  m_vGapW2.resize(m_pParams->m_vInstance.size(), 0);

  /**
   * Build the table, unless this feature were disabled.
   */

  if(pParams->m_s1.find_first_of('d') >= pParams->m_s1.size()) {
    this->buildTable();
    m_bEnabled = true;
  }
}

Domination* Domination::create(const Parameters* pParams,
			       const RectPtrArray* pRects,
			       const DuplicateIndex* pDuplicates,
			       bool bSupportsDomination) {
  Domination* pReturn(NULL);

  /**
   * Test to see if the requested packer can handle building this
   * table. This is mainly a temporary hack for now, until we enable
   * all packers to be able to build dynamic domination tables.
   */

  if(!pParams->m_sRegression.empty()) {
    Packer* pTest = Packer::create(pParams);
    if(!pTest->m_bCanBuildDomination) {
      delete pTest;
      pReturn = new EmptyDom();
      pReturn->initialize(pParams, pRects, pDuplicates);
      return(pReturn);
    }
    delete pTest;
  }

  if(pParams->m_sRegression.find_first_of('e') < pParams->m_sRegression.size() ||
     pParams->m_s1.find_first_of('d') < pParams->m_s1.size() ||
     !bSupportsDomination)
    pReturn = new EmptyDom();
  else if(pParams->m_vInstance.m_bAllSquares &&
	  pParams->m_vInstance.m_bUnique &&
	  pParams->m_vInstance.m_bSortedDecreasingArea &&
	  pParams->m_vInstance.m_bSequential)
    pReturn = new SquareDom();
  else if(pParams->m_vInstance.m_nBenchmark == 2 &&
	  pParams->m_vInstance.m_bUnoriented &&
	  pParams->m_vInstance.m_bSortedDecreasingArea &&
	  pParams->m_vInstance.m_bSequential)
    pReturn = new URectDom();
  else
    pReturn = new Domination();
  pReturn->initialize(pParams, pRects, pDuplicates);
  return(pReturn);
}

bool Domination::dominated(const Rectangle* r, UInt nGap) const {
  return(dominatedw1(r, nGap));
}

bool Domination::dominatedw(const Rectangle* r, UInt nGap) const {
  return(dominatedw1(r, nGap));
}

bool Domination::dominatedh(const Rectangle* r, UInt nGap) const {
  return(dominatedh1(r, nGap));
}

bool Domination::dominatedw1(const Rectangle* r, UInt nGap) const {
  return(dominated(r, nGap, m_vTableW1, m_vTableH1));
}

bool Domination::dominatedw2(const Rectangle* r, UInt nGap) const {
  return(dominated(r, nGap, m_vTableW2, m_vTableH2));
}

bool Domination::dominatedh1(const Rectangle* r, UInt nGap) const {
  return(dominated(r, nGap, m_vTableH1, m_vTableW1));
}

bool Domination::dominatedh2(const Rectangle* r, UInt nGap) const {
  return(dominated(r, nGap, m_vTableH2, m_vTableW2));
}

UInt Domination::gapw1(const Rectangle* r) const {
  return(gap(r, m_vGapW1, m_vGapH1));
}

UInt Domination::gapw2(const Rectangle* r) const {
  return(gap(r, m_vGapW2, m_vGapH2));
}

UInt Domination::gaph1(const Rectangle* r) const {
  return(gap(r, m_vGapH1, m_vGapW1));
}

UInt Domination::gaph2(const Rectangle* r) const {
  return(gap(r, m_vGapH2, m_vGapW2));
}

UInt Domination::gap(const Rectangle* r) const {
  return(gapw1(r));
}

UInt Domination::gapw(const Rectangle* r) const {
  return(gapw1(r));
}

UInt Domination::gaph(const Rectangle* r) const {
  return(gaph1(r));
}

UInt Domination::entries(const Rectangle* r) const {
  return(entriesw1(r));
}

UInt Domination::entriesw1(const Rectangle* r) const {
  return(entries(r, m_vTableW1, m_vTableH1));
}

UInt Domination::entriesw2(const Rectangle* r) const {
  return(entries(r, m_vTableW2, m_vTableH2));
}

UInt Domination::entriesh1(const Rectangle* r) const {
  return(entries(r, m_vTableH1, m_vTableW1));
}

UInt Domination::entriesh2(const Rectangle* r) const {
  return(entries(r, m_vTableH2, m_vTableW2));
}

UInt Domination::entriesw(const Rectangle* r) const {
  return(entriesw1(r));
}

UInt Domination::entriesh(const Rectangle* r) const {
  return(entriesh1(r));
}

void Domination::buildTableStage1() {
  TimeSpec t;
  t.tick();

  PackingCache c;
  RectDecArray rda;
  WidthFirst wf;
  HeightFirst hf;

  rda.initialize(*m_pRects, &wf);
  buildTable(rda, c, m_vTableW1);
  rda.initialize(*m_pRects, &hf);
  buildTable(rda, c, m_vTableH1);

  t.tock();
  m_Time += t;
}

void Domination::buildTableStage2() {
  TimeSpec t;
  t.tick();

  /**
   * Duplicate the tables and ensure that first, interior, and final
   * duplicates are handled correctly.
   */

  DuplicateMap m;
  getDuplicates(m);
  m_vTableH2 = m_vTableH1;
  m_vTableW2 = m_vTableW1;
  constrainDuplicatesW(m);
  constrainDuplicatesH(m);

  /**
   * Detect and remove any entries that may cause deadlocks. We must
   * process both sides of the box as well as both the widths and
   * heights.
   */

  WidthFirst wf;
  HeightFirst hf;
  RectDecArray rdaW(*m_pRects, &wf);
  RectDecArray rdaH(*m_pRects, &hf);
  DomDecorator dd;

  dd.initialize(m_vTableW1, m_vTableH2); deadlocks(rdaW, dd);
  dd.initialize(m_vTableH1, m_vTableW1); deadlocks(rdaH, dd);
  dd.initialize(m_vTableW2, m_vTableH2); deadlocks(rdaW, dd);
  dd.initialize(m_vTableH2, m_vTableW2); deadlocks(rdaH, dd);

  /**
   * Infer the gap tables.
   */

  buildGaps(m_vTableW1, m_vGapW1);
  buildGaps(m_vTableH1, m_vGapH1);
  buildGaps(m_vTableW2, m_vGapW2);
  buildGaps(m_vTableH2, m_vGapH2);

  /**
   * Resize the tables to match the actual number of significant
   * entries.
   */

  buildEntries(m_vTableW1);
  buildEntries(m_vTableW2);
  buildEntries(m_vTableH1);
  buildEntries(m_vTableH2);

  t.tock();
  m_Time += t;
}

void Domination::buildTable() {
  buildTableStage1();
}

void Domination::getDuplicates(Domination::DuplicateMap& m) const {
  for(RectPtrArray::const_iterator i = m_pRects->begin();
      i != m_pRects->end(); ++i) {
    if(m_pDuplicates->first(*i) && !m_pDuplicates->last(*i)) {
      m[*i].push_back(*i);
      const Rectangle* pNext = *i;
      while(pNext = m_pDuplicates->next(pNext))
	m[*i].push_back(pNext);
    }
  }
}

void Domination::constrainDuplicatesW(const Domination::DuplicateMap& m) {

  /**
   * Iterate over each list of duplicates.
   */

  for(DuplicateMap::const_iterator i = m.begin(); i != m.end(); ++i) {

    /**
     * Handle the first duplicate. We allow all entries against the
     * first edge to be valid, but not entries against the second edge
     * to be of gap r->height and greater.
     */
    
    const Rectangle* r = i->second.front();
    if(r->m_bRotatable)
      m_vTableW2[r->m_nID].resize(r->m_nMinDim, 0);
    else
      m_vTableW2[r->m_nID].resize(r->m_nHeight, 0);

    /**
     * Handle the last duplicate. We allow all entries against the
     * second edge to be valid, but not entries against the first edge
     * to be of gap h and greater.
     */

    r = i->second.back();
    if(r->m_bRotatable)
      m_vTableW1[r->m_nID].resize(r->m_nMinDim, 0);
    else
      m_vTableW1[r->m_nID].resize(r->m_nHeight, 0);

    /**
     * All interior duplicates cannot have domination entries greater
     * than the height of the rectangle.
     */

    std::list<const Rectangle*>::const_iterator j = i->second.begin(); ++j;
    std::list<const Rectangle*>::const_iterator k = i->second.end(); --k;
    for(; j != k; ++j) {
      r = *j;
      if(r->m_bRotatable) {
	m_vTableW1[r->m_nID].resize(r->m_nMinDim, 0);
	m_vTableW2[r->m_nID].resize(r->m_nMinDim, 0);
      }
      else {
	m_vTableW1[r->m_nID].resize(r->m_nHeight, 0);
	m_vTableW2[r->m_nID].resize(r->m_nHeight, 0);
      }
    }
  }
}

void Domination::constrainDuplicatesH(const Domination::DuplicateMap& m) {

  /**
   * Iterate over each list of duplicates.
   */

  for(DuplicateMap::const_iterator i = m.begin(); i != m.end(); ++i) {

    /**
     * Handle the first duplicate. We allow all entries against the
     * first edge to be valid, but not entries against the second edge
     * to be of gap h and greater.
     */

    const Rectangle* r = i->second.front();
    if(r->m_bRotatable)
      m_vTableH2[r->m_nID].resize(r->m_nMinDim, 0);
    else
      m_vTableH2[r->m_nID].resize(r->m_nWidth, 0);

    /**
     * Handle the last duplicate. We allow all entries against the
     * second edge to be valid, but not entries against the first edge
     * to be of gap h and greater.
     */

    r = i->second.back();
    if(r->m_bRotatable)
      m_vTableH1[r->m_nID].resize(r->m_nMinDim, 0);
    else
      m_vTableH1[r->m_nID].resize(r->m_nWidth, 0);

    /**
     * All interior duplicates cannot have domination entries greater
     * than the height of the rectangle.
     */

    std::list<const Rectangle*>::const_iterator j = i->second.begin(); ++j;
    std::list<const Rectangle*>::const_iterator k = i->second.end(); --k;
    for(; j != k; ++j) {
      r = *j;
      if(r->m_bRotatable) {
	m_vTableH1[r->m_nID].resize(r->m_nMinDim, 0);
	m_vTableH2[r->m_nID].resize(r->m_nMinDim, 0);
      }
      else {
	m_vTableH1[r->m_nID].resize(r->m_nWidth, 0);
	m_vTableH2[r->m_nID].resize(r->m_nWidth, 0);
      }
    }
  }
}

void Domination::buildTable(const RectDecArray& rda, PackingCache& c,
			    std::vector<std::vector<UInt> >& vTable) {
  BoxDimensions b;
  Parameters p(*m_pParams);
  p.m_s1 += 'd'; // Disable recursive domination table building.
  for(RectDecArray::const_iterator i = rda.begin();
      i != rda.end(); ++i) {

    /**
     * For each gap, find the eligible rectangles. It's also possible
     * that we find a rectangle that simply sticks out (but can fit in
     * the gap). We limit ourselves to gaps at most equal to the width
     * (for a square empty region).
     */

    const RectDecorator* rd = *i;
    const Rectangle* r = rd->m_pRect;
    vTable[r->m_nID].clear();
    vTable[r->m_nID].resize(rd->d1() + 1, 0); // Default is not dominated.
    for(UInt g = 1; g <= rd->d1(); ++g) {

      /**
       * Get rectangles that can fit in the gap.
       */

      RectDecDeq d;
      getEligible(rd, rda, g, d);

      /**
       * If there are no items to pack, this position is automatically
       * dominated.
       */
      
      if(d.empty()) {
	vTable[r->m_nID][g] = 1; // Dominated.
	continue;
      }

      /**
       * Check to see if we have enough area in the gap to accommodate
       * the area of all of these rectangles.
       */

      UInt nArea(totalArea(d));
      if(nArea > g * rd->d1()) // Not dominated.
	continue;

      /**
       * Check if any single rectangle sticks out of the bucket. If
       * so, then the position is automatically not dominated.
       */

      if(sticksOut(g, rd->d1(), d)) // Not dominated.
	break;

      /**
       * Check the cache to see if we've already tried packing
       * something like this instance.
       */

      BoxDimensions b;
      if(rd->d1() == r->m_nWidth) b.initialize(rd->d1(), g);
      else b.initialize(g, rd->d1());

      PackingCacheKey k(b, d);
      PackingCache::iterator pc = c.find(k);
      if(pc != c.end()) {
	vTable[r->m_nID][g] = pc->second;

	/**
	 * If this entry involves moving the root rectangle, we must
	 * note this fact. The first value is the coordinate to which
	 * the root rectangle is moved.
	 */

	if(pc->second && d.find(rda.front()->m_pRect) != d.end())
	  m_MovesRootY.insert(SymKey(g + rd->d2(), r->m_nID, &vTable, g));
	continue;
      }

      /**
       * Possible packing attempt, so let's make a copy of the
       * parameters and construct the instance.
       */

      p.m_vInstance.clear();
      for(RectDecDeq::iterator j = d.begin(); j != d.end(); ++j)
	p.m_vInstance.push_back(RDimensions(*((*j)->m_pRect)));
      p.m_vInstance.inferInstanceProperties();
      if(pack(&p, b)) {
	vTable[r->m_nID][g] = 1; // Dominated.
	c.set(k, !p.m_vInstance.m_bOrientedNonSquares, 1);

	/**
	 * If this entry involves moving the root rectangle, we must
	 * note this fact. The first value is the coordinate to which
	 * the root rectangle is moved.
	 */

	if(d.find(rda.front()->m_pRect) != d.end())
	  m_MovesRootY.insert(SymKey(g + rd->d2(), r->m_nID, &vTable, g));
      }
      else c.set(k, !p.m_vInstance.m_bOrientedNonSquares, 0);
    }
  }
}

void Domination::deadlocks(const RectDecArray& rda, DomDecorator& t) const {
  for(RectDecArray::const_reverse_iterator i = rda.rbegin();
      i != rda.rend(); ++i) {

    /**
     * For each gap, find the eligible rectangles. It's also possible
     * that we find a rectangle that simply sticks out (but can fit in
     * the gap). We limit ourselves to gaps at most equal to the width
     * (for a square empty region).
     */

    const Rectangle* r = (*i)->m_pRect;
    for(UInt g = 1; g < t.t1()[r->m_nID].size(); ++g) {

      /**
       * No danger of deadlock.
       */
      
      if(t.t1()[r->m_nID][g] == 0) continue;

      /**
       * Get rectangles that can fit in the gap.
       */

      RectDecDeq d;
      getEligible(*i, rda, g, d);

      /**
       * If there are no items to pack, there is no danger of deadlock.
       */
      
      if(d.empty()) continue;

      /**
       * Check to see if having this domination entry would create a
       * circular case with a previously placed rectangle. If so, we
       * must remove this entry.
       */

      if(deadlocks(*i, d, t)) t.t1()[r->m_nID][g] = 0;
    }
  }
}

bool Domination::sticksOut(UInt g, UInt l,
			   const RectDecDeq& d) const {
  for(RectDecDeq::const_iterator j = d.begin();
      j != d.end(); ++j)

    /**
     * If the rectangle in question is rotatable, we need to be
     * optimistic. It only sticks out if its minimum dimension sticks
     * out. Otherwise if it's not rotatable, we check its appropriate
     * dimension to see if it sticks out of the bucket.
     */

    if((*j)->m_pRect->m_bRotatable) {
      if((*j)->m_pRect->m_nMinDim > l) return(true);

      /**
       * Consider the possibility that one orientation may be forced
       * due to the constraints of the gap.
       */

      if((*j)->m_pRect->m_nHeight > g && (*j)->m_pRect->m_nHeight > l)
	return(true);
      if((*j)->m_pRect->m_nWidth > g && (*j)->m_pRect->m_nWidth > l)
	return(true);
    }
    else if((*j)->d1() > l) return(true);
  return(false);
}

UInt Domination::totalArea(const RectDecDeq& d) const {
  UInt nArea(0);
  for(RectDecDeq::const_iterator j = d.begin();
      j != d.end(); ++j)
    nArea += (*j)->m_pRect->m_nArea;
  return(nArea);
}

bool Domination::pack(Parameters* pParams, const BoxDimensions& b) {
  Packer* p = Packer::create(pParams);
  p->initialize(pParams);
  p->m_pBoxes = NULL;
  HeapBox hb;
  hb.m_Box = b;
  p->initialize(&hb);
  bool bResult = p->pack(hb);
  delete p;
  return(bResult);
}

void Domination::print() const {
  std::cout << "Height1:" << std::endl;
  printTable(m_vTableH1);
  std::cout << std::endl << "Width1: " << std::endl;
  printTable(m_vTableW1);
  std::cout << "Height2:" << std::endl;
  printTable(m_vTableH2);
  std::cout << std::endl << "Width2: " << std::endl;
  printTable(m_vTableW2);
  std::cout << std::endl;
  printMeta();
}

void Domination::printTable(const std::vector<std::vector<UInt> >& v) const {
  UInt nMax = 0;
  for(UInt i = 0; i < v.size(); ++i)
    nMax = std::max(nMax, (UInt) v[i].size());
  std::cout << "   ";
  for(UInt i = 0; i < nMax; ++i)
    std::cout << std::setw(3) << i;
  std::cout << std::endl;
  for(UInt i = 0; i < v.size(); ++i) {
    std::cout << std::setw(3) << i;
    for(UInt j = 0; j < v[i].size(); ++j)
      if(v[i][j] == 1)
	std::cout << "  1";
      else
	std::cout << "  0";
    std::cout << std::endl;
  }
}

void Domination::printMeta() const {
  std::cout << "   GapH1 GapW1 GapH2 GapW2 EntriesH1 EntriesW1 EntriesH2 EntriesW2" << std::endl;
  for(UInt i = 0; i < m_vGapW1.size(); ++i) {
    std::cout << std::setw(2) << i;
    if(i < m_vGapH1.size())
      std::cout << std::setw(6) << m_vGapH1[i];
    else
      std::cout << "     ";

    if(i < m_vGapW1.size())
      std::cout << std::setw(6) << m_vGapW1[i];
    else
      std::cout << "     ";

    if(i < m_vGapH2.size())
      std::cout << std::setw(6) << m_vGapH2[i];
    else
      std::cout << "     ";

    if(i < m_vGapW2.size())
      std::cout << std::setw(6) << m_vGapW2[i];
    else
      std::cout << "     ";

    if(i < m_vTableH1.size())
      std::cout << std::setw(10) << m_vTableH1[i].size();
    else
      std::cout << "         ";
    
    if(i < m_vTableW1.size())
      std::cout << std::setw(10) << m_vTableW1[i].size();
    else
      std::cout << "         ";

    if(i < m_vTableH2.size())
      std::cout << std::setw(10) << m_vTableH2[i].size();
    else
      std::cout << "         ";
    
    if(i < m_vTableW2.size())
      std::cout << std::setw(10) << m_vTableW2[i].size();
    else
      std::cout << "         ";

    std::cout << std::endl;
  }
}

void Domination::buildGaps(const std::vector<std::vector<UInt> >& vTable,
			   std::vector<UInt>& vGap) {
  for(UInt i = 0; i < vTable.size(); ++i) {
    UInt j;
    for(j = 1; j < vTable[i].size(); ++j)
      if(vTable[i][j] == 0) {
	vGap[i] = j - 1;
	break;
      }
    if(j == vTable[i].size())
      vGap[i] = j - 1;
  }
}

void Domination::buildEntries(std::vector<std::vector<UInt> >& vTable) {
  for(UInt i = 0; i < vTable.size(); ++i) {
    int j;
    for(j = vTable[i].size() - 1; j >= 0; --j)
      if(vTable[i][j] == 1) {
	vTable[i].resize(j + 1);
	break;
      }
    if(j < 0) vTable[i].resize(1);
  }
}

bool Domination::deadlocks(const RectDecorator* rd,
			   const RectDecDeq& d,
			   DomDecorator& t) const {

  /**
   * Iterate over all eligible rectangles that were previously placed.
   */

  for(UInt i = 0; i < d.size(); ++i) {

    /**
     * Deadlocks will only occur when it's possible for one rectangle
     * to have the same height/width as the other rectangle (that is,
     * the gap wall of one can equal the gap wall of the other). If
     * they are different, then one would obviously stick out from the
     * other, and the position wouldn't have been dominated in the
     * first place. If there's no match, then we move on to check the
     * next rectangle.
     */

    if((rd->m_pRect->m_bRotatable || d[i]->m_pRect->m_bRotatable) &&
       d[i]->d2() != rd->d1() && d[i]->d1() != rd->d1())
      continue;
    else if(d[i]->d1() != rd->d1())
      continue;
    
    /**
     * Iterate over all domination entries whose gaps can accommodate
     * the current rectangle.
     */

    if(rd->m_pRect->m_bRotatable) {
      
      /**
       * If the case for the wall-creating rectangle is unoriented, we
       * need to check two tables. First is the table where the other
       * rectangle provides a wall with its width, and second where
       * the other rectangle provides a wall with its height.
       */

      /**
       * Consider a gap wall provided by the other rectangle's
       * width. We'll only conduct the check if our current rectangle
       * doesn't stick out. During the check, we assume the gap size
       * must be at least the height of the current rectangle.
       */

      if(d[i]->d2() == rd->d1())
	for(UInt j = rd->d2(); j < t.t2()[d[i]->m_pRect->m_nID].size(); ++j)
	  if(t.t2()[d[i]->m_pRect->m_nID][j] == 1) return(true);

      /**
       * Consider a gap wall provided by the other rectangle's
       * height. We'll only conduct the check if our current rectangle
       * doesn't stick out. During the check, we assume the gap size
       * must be at least the height of the current rectangle.
       */

      if(d[i]->d1() == rd->d1())
	for(UInt j = rd->d2(); j < t.t1()[d[i]->m_pRect->m_nID].size(); ++j)
	  if(t.t1()[d[i]->m_pRect->m_nID][j] == 1) return(true);
    }
    else {
      for(UInt j = rd->d2(); j < t.t1()[d[i]->m_pRect->m_nID].size(); ++j) {

	/**
	 * If the entry is dominated, then there is a possible deadlock!
	 */

	if(t.t1()[d[i]->m_pRect->m_nID][j] == 1) return(true);
      }
    }
  }

  /**
   * No deadlock detected.
   */

  return(false);
}

void Domination::getEligible(const RectDecorator* rd,
			     const RectDecArray& rda, UInt nGap,
			     RectDecDeq& d) const {
  for(RectDecArray::const_iterator j = rda.begin(); j != rda.end();
      ++j) {

    /**
     * The rectangle creating the wall is ineligible to be placed in
     * the gap area.
     */

    if(*j != rd)

      /**
       * Check if it's possible for j to even fit in the bucket (note
       * we're not checking to see if it sticks out yet).
       */

      if((*j)->d2() <= nGap ||
	 ((*j)->m_pRect->m_bRotatable && (*j)->d1() <= nGap))

	/**
	 * If we're handling duplicates, the assumption is that
	 * all duplicate rectangles down the line in our variable
	 * ordering will be forced to have a greater than or equal
	 * coordinate. Therefore they are ineligible to fit in
	 * this gap created by our current rectangle i.
	 *
	 * If at least i or j were unoriented, then sometimes they
	 * will be duplicates and therefore we must make j
	 * eligible for packing.
	 */

	if((!rd->m_pRect->m_bRotatable &&
	    !(*j)->m_pRect->m_bRotatable &&
	    !rd->m_pRect->equal((*j)->m_pRect)) ||
	   ((rd->m_pRect->m_bRotatable || (*j)->m_pRect->m_bRotatable) &&
	    !rd->m_pRect->uequal((*j)->m_pRect)))
	  d.push_back(*j);
  }
}

Domination* Domination::clone() const {
  return(new Domination(*this));
}

Domination::Domination(const Domination& d) :
  m_Time(d.m_Time),
  m_pParams(d.m_pParams),
  m_pRects(d.m_pRects),
  m_pDuplicates(d.m_pDuplicates),
  m_vTableH1(d.m_vTableH1),
  m_vTableH2(d.m_vTableH2),
  m_vTableW1(d.m_vTableW1),
  m_vTableW2(d.m_vTableW2),
  m_vGapH1(d.m_vGapH1),
  m_vGapH2(d.m_vGapH2), 
  m_vGapW1(d.m_vGapW1),
  m_vGapW2(d.m_vGapW2),
  m_bEnabled(d.m_bEnabled) {

  /**
   * m_MovesRootY is special. It contains data structures that refer
   * to the current class members, so we will have to remap them to
   * the new class member objects.
   */

  for(std::set<SymKey>::const_iterator i = d.m_MovesRootY.begin();
      i != d.m_MovesRootY.end(); ++i) {
    SymKey s(*i);
    if(s.m_pTable == &d.m_vTableH1)
      s.m_pTable = &m_vTableH1;
    else if(s.m_pTable == &d.m_vTableH2)
      s.m_pTable = &m_vTableH2;
    else if(s.m_pTable == &d.m_vTableW1)
      s.m_pTable = &m_vTableW1;
    else if(s.m_pTable == &d.m_vTableW2)
      s.m_pTable = &m_vTableW2;
    else {
      std::cout << "Domination::Domination(...): Error!" << std::endl;
      exit(0);
    }
    m_MovesRootY.insert(m_MovesRootY.end(), s);
  }
}

void Domination::preserveTopSymmetry(UInt nMax) {
  TimeSpec t;
  t.tick();

  /**
   * Index into the set data structure with the given coordinate, and
   * then iterate processing all larger minimum coordinates to the end
   * of the set. We process first entries that move the root's
   * y-coordinate.
   */

  SymKey s;
  s.m_nMinCoordinate = nMax;

  /**
   * Find the location in our set where we've got the start of a
   * sequence of domination entries where the root rectangle is moved
   * further and further out of its upper left quadrant. The sequence
   * before this point doesn't matter to us. We only have to eliminate
   * the entries after that point.
   */

  std::set<SymKey>::iterator i = m_MovesRootY.upper_bound(s);
  for(; i != m_MovesRootY.end(); ++i)
    if(i->m_nMinCoordinate > nMax) {

      /**
       * If enforcing the top-level symmetry policy rules out the
       * dominating position, then we must explore the dominated
       * position by removing the entry from our table.
       */
      
      i->m_pTable->operator[](i->m_nID)[i->m_nGap] = 0;
    }

  t.tock();
  m_Time += t;
}

void Domination::swap() {
  
  /**
   * Not implemented.
   */

}
