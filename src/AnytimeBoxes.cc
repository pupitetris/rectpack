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
#include "AnytimeBoxes.h"
#include "BoxDimensions.h"
#include "Globals.h"
#include "Grid.h"
#include "GridViz.h"
#include "LessHeight.h"
#include "LessMinDim.h"
#include "MetaCSP.h"
#include "Packer.h"
#include "Parameters.h"
#include "Placements.h"
#include "Rational.h"
#include "Solution.h"
#include "WidthHeight.h"
#include <algorithm>
#include <assert.h>
#include <functional>
#include <sstream>

AnytimeBoxes::AnytimeBoxes() {
}

AnytimeBoxes::~AnytimeBoxes() {
}

void AnytimeBoxes::initialize(const Parameters* pParams) {
  m_pParams = pParams;
  if(pParams->m_s0.find_first_of('c') < pParams->m_s0.size())
    m_bConflictLearning = false;
  if(pParams->m_s0.find_first_of('i') < pParams->m_s0.size())
    m_bAllIntegralBoxes = true;
  if(pParams->m_s0.find_first_of('m') < pParams->m_s0.size())
    m_bMutexChecking = false;
  if(pParams->m_s0.find_first_of('w') < pParams->m_s0.size())
    m_bTopLevelWaste = false;
  m_nMinArea = pParams->m_vInstance.m_nTotalArea;
  m_nMaxArea = std::min(pParams->m_vInstance.m_nMax.m_nHeight.get_ui() *
			pParams->m_vInstance.m_nStacked.m_nWidth.get_ui(),
			pParams->m_vInstance.m_nMax.m_nWidth.get_ui() *
			pParams->m_vInstance.m_nStacked.m_nHeight.get_ui());

  if(!pParams->m_bSingleRun && pParams->m_nWeakening != 6) {

    /**
     * Initialize the set of subset sums in both dimensions of width
     * and height.
     */
  
    if(m_bAllIntegralBoxes)
      m_nWidth = m_pParams->m_vInstance.m_nMaxMin.m_nWidth.get_ui();
    else {
      m_Heights.initializeH(m_pPacker->m_vOriginalRects.begin(),
			    m_pPacker->m_vOriginalRects.end());
      m_Widths.initializeW(m_pPacker->m_vOriginalRects.begin(),
			   m_pPacker->m_vOriginalRects.end());
    }
    m_iWidth = m_Widths.lower_bound(m_pParams->m_vInstance.m_nMaxMin.m_nWidth.get_ui());

    /**
     * If conflict learning is enabled, then we will build the subset
     * sums for each subset of rectangles in accordance with our
     * variable ordering. These subsets are essentially sequences from
     * the start of our rectangles up to some point in the middle of
     * the array, so technically there are only N of these.
     */

    if(m_bConflictLearning && !m_bAllIntegralBoxes) {
      m_vWidths.initialize(&m_pPacker->m_vOriginalRects, &m_Widths,
			   SubsetSumsSet::WIDTH);
      m_vHeights.initialize(&m_pPacker->m_vOriginalRects, &m_Heights,
			    SubsetSumsSet::HEIGHT);
    }
  }
  if(!m_pParams->m_bQuiet && m_pParams->m_Log.empty())
    std::cout << "Command line: " << m_pParams->m_sCommand << std::endl
	      << "Minimum area: " << m_pParams->m_vUnscaled.m_nTotalArea
	      << std::endl << std::endl;
}

bool AnytimeBoxes::run() {
  m_Total.clear();
  m_TotalTime.clear();
  m_TotalXTime.clear();
  m_TotalYTime.clear();
  m_pPacker->pack();
  m_TotalTime += m_pPacker->m_Time;
  m_TotalYTime += m_pPacker->m_YTime;
  m_TotalXTime += m_pPacker->m_XTime;
  m_Total.accumulate(m_pPacker->m_Nodes);
  return(true);
}

void AnytimeBoxes::initialize(const std::deque<Packer*>& vPackers) {
  m_pPacker = vPackers.front();
}

void AnytimeBoxes::initialize(Packer* pPacker) {
  m_pPacker = pPacker;
}

bool AnytimeBoxes::singleRun() {
  return(run());
}

std::string AnytimeBoxes::solutions() const {
  std::ostringstream oss;
  const MetaCSP* pMeta = (const MetaCSP*) m_pPacker;
  for(Solution::const_iterator i = pMeta->m_Best.begin();
      i != pMeta->m_Best.end(); ++i) {
    RDimensions box(*i);
    box *= m_pParams->m_vInstance.m_nScale;
    oss << box << " ";
  }
  return(oss.str());
}
