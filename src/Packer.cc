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

#include "BoundingBoxes.h"
#include "ConflictBT.h"
#include "ConflictSums.h"
#include "CorrectedIjcai.h"
#include "Duplicate.h"
#include "DynamicSums.h"
#include "IntBalOpt.h"
#include "Packer.h"
#include "Parameters.h"
#include "IntPlacements.h"
#include "MetaCSP.h"
#include "PrecomputedSums.h"
#include "Rational.h"
#include "Rectangle.h"
#include "SearchControl.h"
#include "SquarePacker.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <assert.h>

Packer::Packer() :
  m_pBoxes(NULL),
  m_pParams(NULL),
  m_nCuSP(0),
  m_bInitialized(false),
  m_bYSmallerThanX(false),
  m_bCanBuildDomination(false),
  m_nNonUnits(0),
  m_pControl(NULL),
  m_nDeepestConflict(0),
  m_bInstanceRotated(false) {
}

Packer* Packer::create(const Parameters* pParams) {
  Packer* pReturn(NULL);
  if(pParams->m_nWeakening == 0)
    pReturn = new Duplicate();
  else if(pParams->m_nWeakening == 1)
    pReturn = new CorrectedIjcai();
  else if(pParams->m_nWeakening == 2)
    pReturn = new IntBalOpt();
  else if(pParams->m_nWeakening == 3)
    pReturn = new PrecomputedSums();
  else if(pParams->m_nWeakening == 4)
    pReturn = new DynamicSums();
  else if(pParams->m_nWeakening == 5)
    pReturn = new ConflictSums();
  else if(pParams->m_nWeakening == 6)
    pReturn = new MetaCSP();
  if(pReturn)
    pReturn->initialize(pReturn);
  return(pReturn);
}

Packer::~Packer() {
}

void Packer::initialize(Packer* pPacker) {
}

void Packer::initialize(const Parameters* pParams) {
  m_pParams = pParams;
  m_vRects.initialize(pParams);
  m_vRectPtrs.initialize(m_vRects);
  m_vOriginalRects = m_vRects;
  m_Duplicates.initialize(pParams, m_vRectPtrs);
  firstUnitRectangle();
  m_nDeepestConflict = pParams->m_vInstance.size() - 1;
}

void Packer::initialize(const HeapBox* pBox) {
  m_BoxMutex.lock();
  m_Box = pBox->m_Box;
  m_BoxMutex.unlock();
  m_vRects.reset(m_pParams->m_vInstance);

  /**
   * Consider rotating the entire instance if it's beneficial for the
   * packer to do so.
   */

  m_bInstanceRotated = false;
  if(m_bYSmallerThanX && (m_Box.m_nHeight > m_Box.m_nWidth)) {
    m_Box.rotate();
    if(!m_pParams->m_vInstance.m_bDiagonalSymmetry)
      m_vRects.rotate();
    m_vRects.constrain(m_Box);
    m_bInstanceRotated = true;
  }
  else
    m_vRects.constrain(*pBox);
  m_vRects.inferProperties();
}

void Packer::initialize(const BoundingBoxes* pBoxes) {
  m_pBoxes = pBoxes;
}

void Packer::printNodes() const {
  m_Nodes.print(m_Time, m_nCuSP);
}

void Packer::set(SearchControl* pControl) {
  m_pControl = pControl;
#ifdef VIZ
  m_State.set(pControl);
#endif // VIZ
}

void Packer::initAccumulators() {
  m_nCuSP = 0;
  m_YTime = 0;
  m_XTime = 0;
  m_Time = 0;
  m_Nodes.clear();
}

bool Packer::pack(const HeapBox& hb) {
  initAccumulators();
  initialize(&hb);
  m_bInitialized = true;
  m_Time.tick();

  bool bResult(false);
  if(m_pParams->m_sCra.empty())
    bResult = pack();
  else
    bResult = packcra(m_pParams->m_sCra);
  if(bResult) placeUnitRectangles();
  m_Time.tock();
  if(m_bYSmallerThanX) {
    m_YTime = m_Time;
    m_YTime -= m_XTime;
  }
  else {
    m_XTime = m_Time;
    m_XTime -= m_YTime;
  }
  return(bResult);
}

bool Packer::packcra(const std::string& sFile) {
  char pBuffer[1024];
  std::ifstream ifs(sFile.c_str());
  while(ifs.good()) {
    ifs.getline(pBuffer, 1024);
    std::string s(pBuffer);
    if(s.size() > m_vRects.size()) { // Simple sanity check.
      std::vector<int> v;
      m_vRects.str2vec(s, v);
      m_vRects.loady(v);
      if(this->packcra())
	return(true);
    }
  }
  return(false);
}

bool Packer::packcra() {
  return(false);
}

void Packer::print() const {
  std::cout << "ID Size    y yi.b yi.e yi.w" << std::endl;
  for(size_t i = 0; i < m_vRectPtrs.size(); ++i)
    std::cout << std::setw(2) << m_vRectPtrs[i]->m_nID << " "
	      << std::setw(2) << m_vRectPtrs[i]->m_nWidth << "x"
	      << std::setw(2) << m_vRectPtrs[i]->m_nHeight
	      << std::setw(4) << m_vRectPtrs[i]->y
	      << std::setw(5) << m_vRectPtrs[i]->yi.m_nBegin
	      << std::setw(5) << m_vRectPtrs[i]->yi.m_nEnd
	      << std::setw(5) << m_vRectPtrs[i]->yi.m_nWidth
	      << std::endl;
  std::cout << std::endl;
}

void Packer::firstUnitRectangle() {
  for(m_iFirstUnit = m_vRectPtrs.begin();
      m_iFirstUnit != m_vRectPtrs.end(); ++m_iFirstUnit)
    if((*m_iFirstUnit)->unit())
      break;
  m_nNonUnits = m_iFirstUnit - m_vRectPtrs.begin();
}
