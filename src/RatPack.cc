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

#include "HeapBox.h"
#include "DynamicEmpty.h"
#include "UnitEmpty.h"
#include "RatPack.h"
#include "Parameters.h"
#include "Placements.h"
#include "IntPlacements.h"
#include "SpaceFill.h"

RatPack::RatPack() :
  m_pSpaceFill(NULL) {
}

RatPack::~RatPack() {
  delete m_pSpaceFill;
}

TimeSpec& RatPack::timeDomination() {
  return(m_DominationTimeStub);
}

void RatPack::initialize(const Parameters* pParams) {
  Packer::initialize(pParams);
  m_vUnits.resize(m_iFirstUnit - m_vRectPtrs.begin());
  m_vX.initialize(pParams);
  m_vX.initialize(this);
  m_DominationTimeStub.clear();
  if(pParams->m_s2.find_first_of('y') < pParams->m_s2.size())
    m_pSpaceFill = new UnitEmpty();
  else
    m_pSpaceFill = new DynamicEmpty();
  m_pSpaceFill->initialize(this);
  m_pSpaceFill->initialize(pParams);
}

void RatPack::initialize(Packer* pPacker) {
  Packer::initialize(pPacker);
  m_vX.initialize(pPacker);
}

void RatPack::initialize(const HeapBox* pBox) {
  Packer::initialize(pBox);
  m_vX.initialize(&pBox->m_Box);
  m_pSpaceFill->initialize(&pBox->m_Box);
}

void RatPack::initialize(const BoundingBoxes* pBoxes) {
  Packer::initialize(pBoxes);
}

bool RatPack::pack() {
  return(packX());
}

bool RatPack::packX() {
  return(false);
}

bool RatPack::packY() {
  if(m_pParams->m_bScheduling)
    return(true);
  else
    return(m_pSpaceFill->packY(m_vX, NULL));
}

void RatPack::get(Placements& v) const {
  v.m_Box = m_Box;
  m_vX.get(v);
  if(m_pParams->m_bScheduling) {
    m_vUnits.get(v);
    v.assignY();
  }
  else {
    m_pSpaceFill->get(v);
    m_vUnits.get(v);
  }
}

void RatPack::placeUnitRectangles() {

  /**
   * Compute the number of rectangles, unit rectangles required, and
   * reserve some space to remember their placed locations.
   */

  UInt nUnits(m_vRectPtrs.end() - m_iFirstUnit);
  m_vUnits.clear();
  m_vUnits.reserve(nUnits);

  /**
   * If we're only placing rectangles for purposes of cumulative
   * scheduling, we can do that easily and simply return.
   */

  if(m_pParams->m_bScheduling) {
    RectPtrArray::const_iterator i = m_iFirstUnit;
    Cumulative::const_iterator x = m_vX.begin();
    while(i != m_vRectPtrs.end()) {
      while(x->second.m_nHeight == m_Box.m_nHeight) ++x;
      UInt nLocal = std::min((UInt) (m_vRectPtrs.end() - i),
			     m_Box.m_nHeight - x->second.m_nHeight);
      while(nLocal) {
	m_vUnits.push_back(IntPlacement(*i, x->first, 0));
	--nLocal;
	++i;
      }
      ++x;
    }
  }
  else {
    IntPlacements ip;
    m_vX.get(ip);
    m_pSpaceFill->get(ip);
    RectPtrArray::const_iterator r = m_iFirstUnit;
    Cumulative::const_iterator x = m_vX.begin();
    while(nUnits) {

      /**
       * Find the next x-coordinate that can accommodate any unit
       * rectangles.
       */

      while(x->second.m_nHeight == m_Box.m_nHeight) ++x;

      /**
       * Set up data structures for placing a local set of unit
       * rectangles just on this column.
       */

      UInt nLocal = std::min(nUnits, m_Box.m_nHeight - x->second.m_nHeight);
      IntPlacements ipCopy(ip);
      IntPlacements ipLocal;
      ipLocal.resize(nLocal);
      for(IntPlacements::iterator i = ipLocal.begin();
	  i != ipLocal.end(); ++i) {
	i->m_nX = x->first;
	i->m_pRect = *r;
	++r;
      }

      /**
       * Make the actual y-coordinate assignments.
       */

      ipCopy.assignY(ipLocal, x->first);
      nUnits -= nLocal;
      m_vUnits.insert(m_vUnits.end(), ipLocal.begin(), ipLocal.end());
    }
  }
}
