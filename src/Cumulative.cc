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

#include "AsciiGrid.h"
#include "BoxDimensions.h"
#include "Cumulative.h"
#include "GreaterCuSP.h"
#include "IntPlacements.h"
#include "Packer.h"
#include "Parameters.h"
#include "Placements.h"
#include "Rectangle.h"
#include "RectArray.h"
#include <iomanip>

Cumulative::Cumulative() :
  m_pPacker(NULL) {
}

Cumulative::~Cumulative() {
}

void Cumulative::initialize(const Parameters* pParams) {
  m_vValues.clear();
  m_vValues.resize(m_pPacker->m_nNonUnits);
  m_vCompulsory.clear();
  m_vCompulsory.reserve(m_pPacker->m_nNonUnits);
  m_vSingular.clear();
  m_vMaxHeight.clear();
  m_vMaxHeight.resize(m_pPacker->m_nNonUnits);
  m_vRMaxHeight.clear();
  m_vRMaxHeight.resize(m_pPacker->m_nNonUnits);
  m_vStack.initialize(pParams);
  m_vCStack.clear();
  m_vCStack.reserve(m_pPacker->m_nNonUnits);
}

void Cumulative::initialize(const BoxDimensions* pBox) {
  clear();
  m_vCompulsory.clear();
  m_vSingular.clear();
  for(RectPtrArray::const_iterator i = m_pPacker->m_vRectPtrs.begin();
      i != m_pPacker->m_iFirstUnit; ++i) {
    m_vMaxHeight[(*i)->m_nID] = pBox->m_nHeight -
      ((*i)->m_bRotated ? (*i)->m_nWidth : (*i)->m_nHeight);
    m_vRMaxHeight[(*i)->m_nID] = pBox->m_nHeight -
      ((*i)->m_bRotated ? (*i)->m_nHeight : (*i)->m_nWidth);
  }
  m_vStack.initialize(pBox);
  m_vCStack.clear();

  /**
   * Now insert the box's begin and end points.
   */

  std::map<UInt, RefHeight>::
    insert(end(), value_type(UInt(0), RefHeight(UInt(0), 1)));
  std::map<UInt, RefHeight>::
    insert(end(), value_type(pBox->m_nWidth, RefHeight(UInt(0), 1)));
}

void Cumulative::initialize(const Packer* pPacker) {
  m_pPacker = pPacker;
}

bool Cumulative::canFit(const Compulsory& c) {
  CompulsoryAssign* p(&m_vValues[c.m_pRect->m_nID]);
  p->m_iLeft = --upper_bound(c.left());
  p->m_iRight = p->m_iLeft;
  bool bReturn = canFitR(p->m_iRight, c.right(),
			 c.m_pRect->m_bRotated ?
			 m_vRMaxHeight[c.m_pRect->m_nID] :
			 m_vMaxHeight[c.m_pRect->m_nID]);
  return(bReturn);
}

void Cumulative::push(const Rectangle* r, const UInt& n) {
  m_vStack.save();
  m_vCStack.push_back(NULL);
  m_vCompulsory.push();
  m_vSingular.push();

  CompulsoryAssign* p(&m_vValues[r->m_nID]);
  if(!p->m_nValue.m_nStart.isPoint()) {
    m_vStack.push(p->m_nValue);
    m_vSingular.push(p);

    /**
     * Set the coordinates.
     */

    p->m_nValue.m_nStart = n;
    p->m_nValue.m_nEnd = n + r->m_nWidth;

    /**
     * Extend both left and right.
     */

    extendL(p->m_iLeft, p->m_nValue.m_nStart.m_nLeft, r->m_nHeight);
    extendR(p->m_iRight, p->m_nValue.m_nEnd.m_nLeft, r->m_nHeight);
  }
}

void Cumulative::push(const Compulsory& c) {
  CompulsoryAssign* p(&m_vValues[c.m_pRect->m_nID]);
  p->m_nValue = c;
  p->m_iLeft = insert(c.left()).first;
  p->m_iRight = insert(c.right()).first;
  pushAux(p);
}

void Cumulative::pushHinted(const Compulsory& c) {
  CompulsoryAssign* p(&m_vValues[c.m_pRect->m_nID]);
  p->m_nValue = c;
  p->m_iLeft = insert(p->m_iLeft, c.left());
  p->m_iRight = insert(p->m_iRight, c.right());
  pushAux(p);
}

void Cumulative::pushAux(CompulsoryAssign* p) {

  /**
   * Check to see if the left and right pointers are new control
   * points. If so, we must copy the previous heights as appropriate
   * (because these new control points should start with a height
   * value that is consistent with what was already implied).
   */

  if(p->m_iLeft->second.m_nReferences == 0) {
    iterator i = p->m_iLeft; --i;
    p->m_iLeft->second.m_nHeight = i->second.m_nHeight;
  }
  if(p->m_iRight->second.m_nReferences == 0) {
    iterator i = p->m_iRight; --i;
    p->m_iRight->second.m_nHeight = i->second.m_nHeight;
  }

  for(iterator i = p->m_iLeft; i != p->m_iRight; ++i)
    i->second.m_nHeight += p->m_nValue.m_pRect->m_nHeight;
  ++p->m_iLeft->second.m_nReferences;
  ++p->m_iRight->second.m_nReferences;

  m_vStack.save();
  m_vCStack.push_back(p);
  m_vCompulsory.push();
  m_vSingular.push();
  if(!p->m_nValue.isFull())
    m_vCompulsory.push_back(p);
  else
    m_vSingular.push_back(p);
}

void Cumulative::pop() {
  while(!m_vStack.empty()) {
    UInt nID(m_vStack.top().m_pRect->m_nID);
    pop(m_vValues[nID], m_vStack.top());
    m_vValues[nID].m_nValue = m_vStack.top();
    m_vStack.pop();
  }
  m_vStack.restore();
  m_vCompulsory.pop();
  m_vSingular.pop();

  /**
   * Now completely remove the last compulsory part that this stack
   * frame is associated with.
   */

  CompulsoryAssign* p(m_vCStack.back());
  m_vCStack.pop_back();
  if(p) {
    const Rectangle* r(p->m_nValue.m_pRect);
    for(iterator i = p->m_iLeft; i != p->m_iRight; ++i)
      i->second.m_nHeight -= r->m_nHeight;
    dereference(p->m_iLeft);
    dereference(p->m_iRight);
  }
}

void Cumulative::pop(CompulsoryAssign& ca, const Compulsory& c) {

  /**
   * If constraints were tightened on the starting coordinates, it
   * means that the current is now less than the previous. So to undo
   * this, we will iterate from the current to the previous and to
   * remove the given height.
   */
  
  if(ca.m_nValue.m_nStart.m_nRight != c.m_nStart.m_nRight) {
    iterator i = ca.m_iLeft;
    iterator j = insert(c.m_nStart.m_nRight).first;
    ++j->second.m_nReferences;
    for(iterator k = i; k != j; ++k)
      k->second.m_nHeight -= c.m_pRect->m_nHeight;
    dereference(i);
    ca.m_iLeft = j;
    ca.m_nValue.m_nStart.m_nRight = c.m_nStart.m_nRight;
    ca.m_nValue.m_nEnd.m_nRight = c.m_nEnd.m_nRight;
  }

  /**
   * If constraints were tightened on the ending coordinates, it means
   * that the current is now greater than the previous. So to undo
   * this, we will iterate from the current down to the previous to
   * remove the given height.
   */

  if(ca.m_nValue.m_nEnd.m_nLeft != c.m_nEnd.m_nLeft) {
    iterator i = insert(c.m_nEnd.m_nLeft).first;
    iterator j = ca.m_iRight;
    ++i->second.m_nReferences;
    for(iterator k = i; k != j; ++k)
      k->second.m_nHeight -= c.m_pRect->m_nHeight;
    dereference(j);
    ca.m_iRight = i;
    ca.m_nValue.m_nEnd.m_nLeft = c.m_nEnd.m_nLeft;
    ca.m_nValue.m_nStart.m_nLeft = c.m_nStart.m_nLeft;
  }
}

bool Cumulative::propagate() {

  /**
   * Loop until we've emptied both queues.
   */

  bool bUpdated(true);
  while(bUpdated) {
    bUpdated = false;
    UInt i(0);
    while(i < m_vCompulsory.size()) {
      CompulsoryAssign* p = m_vCompulsory[i];
      if(p->m_nValue.isFull()) {
	m_vCompulsory.remove(i);
	m_vSingular.push_back(p);
	continue;
      }

      /**
       * Check the left side.
       */

      int n = propagateLeft(p);
      if(n == -1) return(false);
      if(n == 1) bUpdated = true;
      if(p->m_nValue.isFull()) {
	m_vCompulsory.remove(i);
	m_vSingular.push_back(p);
	continue;
      }

      /**
       * Check the right side.
       */

      n = propagateRight(p);
      if(n == -1) return(false);
      if(n == 1) bUpdated = true;
      if(p->m_nValue.isFull()) {
	m_vCompulsory.remove(i);
	m_vSingular.push_back(p);
	continue;
      }
      ++i;
    }
  }
  return(true);
}

int Cumulative::propagateLeft(CompulsoryAssign* p) {

  /**
   * Get the range we must iterate over to check for
   * obstructions.
   */

  const Rectangle* r = p->m_nValue.m_pRect;
  UInt* pMax(r->m_bRotated ?
	     &m_vRMaxHeight[p->id()] :
	     &m_vMaxHeight[p->id()]);
  iterator iHint = p->m_iLeft;
  if(canFitL(iHint, p->m_nValue.m_nStart.m_nLeft, *pMax)) return(0);

  /**
   * Update the data structures in the following order. 1) Check
   * that we can resize the compulsory part, 2) Remove both
   * missing intervals from the skip lists 3) Enqueue the new
   * control points for our compulsory part, and 4) Reinsert the
   * new missing intervals into both skip lists.
   */

  ++iHint;
  UInt nNewLeft(iHint->first);
  UInt nExtension(iHint->first + r->m_nWidth);
  iHint = p->m_iRight;
  if(!canFitR(iHint, nExtension, *pMax)) return(-1);

  /**
   * Now resize the compulsory part. Recall we're extending the
   * current compulsory part to the right, so we remove the
   * previous endpoint, and add in the new endpoint from the
   * Cumulative map.
   */

  m_vStack.push(p->m_nValue);
  p->m_nValue.m_nStart.m_nLeft = nNewLeft;
  p->m_nValue.m_nEnd.m_nLeft = nExtension;
  extendR(p->m_iRight, nExtension, iHint, r->m_nHeight);
  return(1);
}

int Cumulative::propagateRight(CompulsoryAssign* p) {

  /**
   * Get the range we must iterate over to check for
   * obstructions.
   */

  const Rectangle* r = p->m_nValue.m_pRect;
  UInt* pMax(r->m_bRotated ?
	     &m_vRMaxHeight[p->id()] :
	     &m_vMaxHeight[p->id()]);
  iterator iHint = p->m_iRight;
  if(canFitR(iHint, p->m_nValue.m_nEnd.m_nRight, *pMax)) return(0);

  /**
   * Update the data structures in the following order. 1) Check
   * that we can resize the compulsory part, 2) Remove both
   * missing intervals from the skip lists 3) Enqueue the new
   * control points for our compulsory part, and 4) Reinsert the
   * new missing intervals into both skip lists.
   */

  UInt nNewRight(iHint->first);
  UInt nExtension(iHint->first - r->m_nWidth);
  iHint = p->m_iLeft;
  if(!canFitL(iHint, nExtension, *pMax)) return(-1);

  /**
   * Now resize the compulsory part. Recall we're extending the
   * current compulsory part to the left, so we remove the
   * previous endpoint, and add in the new endpoint from the
   * Cumulative map.
   */

  m_vStack.push(p->m_nValue);
  p->m_nValue.m_nEnd.m_nRight = nNewRight;
  p->m_nValue.m_nStart.m_nRight = nExtension;
  extendL(p->m_iLeft, nExtension, iHint, r->m_nHeight);
  return(1);
}

bool Cumulative::canFitR(iterator& iHint, const UInt& j,
			 const UInt& nMaxHeight) {
  for(; iHint->first < j; ++iHint)  {
    if(iHint->second.m_nHeight > nMaxHeight)
      return(false);
  }
  return(true);
}

bool Cumulative::canFitL(iterator& iHint, const UInt& j,
			 const UInt& nMaxHeight) {

  /**
   * We don't need to check the starting point, since we're assuming
   * that there is a non-empty extension from the starting point
   * downward.
   */

  for(--iHint; iHint->first > j; --iHint)
    if(iHint->second.m_nHeight > nMaxHeight)
      return(false);

  /**
   * When we've exited the loop the value is less than j or it's equal
   * to j. If it's less than j, then we want to make sure the height
   * can accommodate it. If it's equal to j, we also want to ensure
   * the same thing.
   */

  return(iHint->second.m_nHeight <= nMaxHeight);
}

bool Cumulative::dereference(const iterator& i) {
  if(--i->second.m_nReferences == 0) {
    erase(i);
    return(true);
  }
  return(false);
}

void Cumulative::extendL(iterator& iCurrent, const UInt& nDest,
			 iterator& iHint, const UInt& nHeight) {
  iHint = insert(iHint, nDest);
  ++iHint->second.m_nReferences;
  for(iterator i = iHint; i != iCurrent; ++i) // Iterates forward (same thing).
    i->second.m_nHeight += nHeight;
  dereference(iCurrent);
  iCurrent = iHint;
}

void Cumulative::extendL(iterator& iCurrent, const UInt& nDest,
			 const UInt& nHeight) {
  iterator iHint(insert(iCurrent, nDest));
  ++iHint->second.m_nReferences;
  for(iterator i = iHint; i != iCurrent; ++i) // Iterates forward (same thing).
    i->second.m_nHeight += nHeight;
  dereference(iCurrent);
  iCurrent = iHint;
}

void Cumulative::extendR(iterator& iCurrent, const UInt& nDest,
			 iterator& iHint, const UInt& nHeight) {
  iHint = insert(iHint, nDest);
  ++iHint->second.m_nReferences;
  for(iterator i = iCurrent; i != iHint; ++i)
    i->second.m_nHeight += nHeight;
  dereference(iCurrent);
  iCurrent = iHint;
}

void Cumulative::extendR(iterator& iCurrent, const UInt& nDest,
			 const UInt& nHeight) {
  iterator iHint = insert(iCurrent, nDest);
  ++iHint->second.m_nReferences;
  for(iterator i = iCurrent; i != iHint; ++i)
    i->second.m_nHeight += nHeight;
  dereference(iCurrent);
  iCurrent = iHint;
}

void Cumulative::largestAreaPlacement(UInt& n,
				      const Compulsory*& c) {
  n = 0;
  if(m_vCompulsory.empty()) c = NULL;
  else
    for(FastRemove<CompulsoryAssign*>::const_iterator i = m_vCompulsory.begin();
	i != m_vCompulsory.end(); ++i) {
      const Compulsory* d = &(*i)->m_nValue;
      UInt m(d->carea());
      if(m > n) {
	n = m;
	c = d;
      }
    }
}

UInt Cumulative::epsilon() const {
  UInt nMin(rbegin()->first);
  if(empty()) return(nMin);
  UInt nPrevious(begin()->first);
  const_iterator i = begin();
  for(++i; i != end(); ++i) {
    nMin = std::min(nMin, i->first - nPrevious);
    nPrevious = i->first;
  }
  return(nMin);
}

std::pair<Cumulative::iterator, bool>
Cumulative::insert(const key_type& x) {
  if(x == 0)
    return(std::pair<Cumulative::iterator, bool>(begin(), false));
  else {
    std::pair<Cumulative::iterator, bool> r
      (std::map<UInt, RefHeight>::insert(value_type(x, RefHeight())));
    if(r.second) {
      iterator iPrevious(r.first);
      --iPrevious;
      r.first->second.m_nHeight = iPrevious->second.m_nHeight;
    }
    return(r);
  }
}

Cumulative::iterator
Cumulative::insert(iterator pos, const key_type& x) {
  if(x == 0) return(begin());
  else {
    pos = std::map<UInt, RefHeight>::insert(pos, value_type(x, RefHeight()));
    if(pos->second.m_nReferences == 0) {
      iterator iPrevious(pos);
      --iPrevious;
      pos->second.m_nHeight = iPrevious->second.m_nHeight;
    }
    return(pos);
  }
}

void Cumulative::print() const {
  std::cout << "Cumulative profile:" << std::endl;
  if(empty()) std::cout << "  empty" << std::endl;
  else {
    UInt n(0);
    for(const_iterator i = begin(); i != end(); ++i) {
      ++n;
      std::cout << "  " << n << ": "
		<< "x=" << i->first
		<< ", y=" << i->second.m_nHeight 
		<< ", r=" << i->second.m_nReferences << std::endl;
    }
  }
  std::cout << std::endl << "Assignments:" << std::endl;
  for(size_t i = 0; i < m_vValues.size(); ++i)
    std::cout << "  " << std::setw(2) << i << ": " << m_vValues[i]
	      << std::endl;
}

void Cumulative::print(const_iterator j) const {
  printAux(j);
}

void Cumulative::print(iterator j) const {
  printAux(j);
}

void Cumulative::printAux(const_iterator j) const {
  if(j == end()) {
    std::cout << "Iterator points to the end of this sequence." << std::endl;
    return;
  }
  UInt n(0);
  for(const_iterator i = begin(); i != end(); ++i) {
    ++n;
    if(i == j) {
      std::cout << n << ": "
		<< "x=" << i->first
		<< ", y=" << i->second.m_nHeight 
		<< ", r=" << i->second.m_nReferences << std::endl;
      return;
    }
  }
  std::cout << "Iterator points outside of this sequence." << std::endl;
}

void Cumulative::get(Placements& v) const {
  v.clear();
  v.reserve(m_vValues.size());
  for(std::vector<CompulsoryAssign>::const_iterator i =
	m_vValues.begin(); i != m_vValues.end(); ++i)
    v.push_back(Placement(i->m_nValue.m_pRect,
			  Coordinates(i->m_nValue.left(), UInt(0))));
}

void Cumulative::get(IntPlacements& v) const {
  v.clear();
  v.reserve(m_vValues.size());
  for(std::vector<CompulsoryAssign>::const_iterator i = m_vValues.begin();
      i != m_vValues.end(); ++i)
    v.push_back(IntPlacement(i->m_nValue.m_pRect,
			     i->m_nValue.left(), 0));
}

bool Cumulative::valid(const iterator& j) const {
  return(validAux(j));
}

bool Cumulative::valid(const const_iterator& j) const {
  return(validAux(j));
}

bool Cumulative::validAux(const const_iterator& j) const {
  for(const_iterator i = begin(); i != end(); ++i)
    if(i == j) return(true);
  return(false);
}

bool Cumulative::valid() const {
  for(std::vector<CompulsoryAssign*>::const_iterator i = m_vCStack.begin();
      i != m_vCStack.end(); ++i) {
    if(*i) {
      if(!valid((*i)->m_iLeft)) return(false);
      if(!valid((*i)->m_iRight)) return(false);
    }
  }
  return(true);
}

const std::vector<CompulsoryAssign>& Cumulative::values() const {
  return(m_vValues);
}

const FastRemove<CompulsoryAssign*>& Cumulative::intervals() const {
  return(m_vCompulsory);
}

const SingleStack& Cumulative::singles() const {
  return(m_vSingular);
}
