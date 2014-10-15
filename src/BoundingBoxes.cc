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
#include "BoundingBoxes.h"
#include "BoxDimensions.h"
#include "Globals.h"
#include "Grid.h"
#include "GridViz.h"
#include "LessHeight.h"
#include "LessMinDim.h"
#include "Packer.h"
#include "Parameters.h"
#include "Placements.h"
#include "Rational.h"
#include "WidthHeight.h"
#include <algorithm>
#include <assert.h>
#include <functional>
#include <sstream>

BoundingBoxes::BoundingBoxes() :
  m_nMinArea(0),
  m_pPacker(NULL),
  m_bAllIntegralBoxes(false),
  m_bMutexChecking(true),
  m_bConflictLearning(true),
  m_bTopLevelWaste(true),
  m_nPopped(0),
  m_pParams(NULL) {
}

BoundingBoxes::~BoundingBoxes() {
}

void BoundingBoxes::initialize(const Parameters* pParams) {
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
  if(pParams->m_s0.find_first_of('l') < pParams->m_s0.size())
    lowerBoundSolution();
  m_nMaxArea = std::min(pParams->m_vInstance.m_nMax.m_nHeight.get_ui() *
			pParams->m_vInstance.m_nStacked.m_nWidth.get_ui(),
			pParams->m_vInstance.m_nMax.m_nWidth.get_ui() *
			pParams->m_vInstance.m_nStacked.m_nHeight.get_ui());

  if(!pParams->m_bSingleRun) {

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
    
    /**
     * We insert into our heap a representative small bounding box for
     * every eligible width. We initialize our pointer to the maximum
     * height, and decrease this, as we increase the width. We stop
     * when the width is so high such that we break any requested
     * diagonal symmetry constraints or when we violate the minimum
     * height requirement (additional widths would only further
     * exacerbate the problem so upon the first violation of these
     * constraints we can forgo continued iteration).
     */

    m_vBoxes.clear();
    if(m_bAllIntegralBoxes)
      enqueueNewIntWidths(m_nMinArea.get_ui());
    else
      enqueueNewWidths(m_nMinArea.get_ui());
  }

  /**
   * Initialize the heap.
   */

  m_nPopped = 0;
  if(!m_pParams->m_bQuiet && m_pParams->m_Log.empty())
    std::cout << "Command line: " << m_pParams->m_sCommand << std::endl
	      << "Minimum area: " << m_pParams->m_vUnscaled.m_nTotalArea
	      << std::endl << std::endl;
}

bool BoundingBoxes::run() {
  m_sAttempts.clear();
  m_Total.clear();
  m_TotalTime.clear();
  m_TotalXTime.clear();
  m_TotalYTime.clear();

  if(m_pParams->m_bSingleRun)
    return(singleRun());

  size_t nNthBox = m_pParams->m_nNthBox;
  if(!m_pParams->m_Log.empty()) {
    m_pParams->m_Log.print();
    nNthBox = (size_t) m_pParams->m_Log.m_nBoxes + 1;
  }

  HeapBox b;
  size_t nIthBox(1), nSolutions(0);
  while(getNext(b) &&
	(b.m_Box.m_nArea <= m_nMaxArea) &&
	(nIthBox <= m_pParams->m_nMthBox)) {

    /**
     * Keep track of which box we've attempted.
     */

    if(nIthBox < nNthBox) {
      if(!m_pParams->m_bQuiet && m_pParams->m_Log.empty())
	std::cout << "Skipping " << m_pParams->unscale(b.m_Box.m_nWidth)
		  << " X " << m_pParams->unscale(b.m_Box.m_nHeight)
		  << " = " << m_pParams->unscale2(b.m_Box.m_nArea)
		  << std::endl;
      ++nIthBox;
      continue;
    } else ++nIthBox;

    /**
     * Attempt packing.
     */

    printAttempt(b.m_Box);
    bool bResult = m_pPacker->pack(b);
    if(m_bAllIntegralBoxes)
      pushNextInt(b);
    else
      pushNext(b);

    /**
     * If a signal was caught and we're told to quit, we won't add in
     * the work we've done on this last box, so that this won't
     * pollute our totals.
     */

    if(bQuit) {
      if(!m_pParams->m_bQuiet) {
	m_pPacker->printNodes();
	std::cout << "  Interrupted! Statistics from the last attempt "
		  << "are not included in the totals below, except for"
		  << " wall-clock time." << std::endl;
      }
      return(false);
    }

    /**
     * Print the nodes generated.
     */

    if(!m_pParams->m_bQuiet)
      m_pPacker->printNodes();

    m_TotalTime += m_pPacker->m_Time;
    m_TotalYTime += m_pPacker->m_YTime;
    m_TotalXTime += m_pPacker->m_XTime;
    
    /**
     * Record this result.
     */

    m_sAttempts.insert(Attempt(b.m_Box, bResult));
    m_Total.accumulate(m_pPacker->m_Nodes);

    /**
     * Print the resulting feasible solution (if any).
     */
    
    if(bResult) {
      if(!m_pParams->m_bQuiet) {
	AsciiGrid ag;
	Placements v;
	m_pPacker->get(v);
	if(m_pPacker->m_bInstanceRotated)
	  v.rotate();
	if(!v.verify())
	  std::cout << "  Error: Constraints not satisfied." << std::endl;
	ag.draw(m_pParams, v);
	ag.print();
	std::cout << "  Rectangles were placed in these (x,y) locations:"
		  << std::endl;
	v *= m_pParams->m_vInstance.m_nScale;
	v.print();
	std::cout << std::endl << std::endl;
      }
      ++nSolutions;
      m_nMaxArea = std::min(m_nMaxArea, b.m_Box.m_nArea);
    }
  }
  return(nSolutions > 0);
}

void BoundingBoxes::pop(HeapBox& hb) {
  hb = m_vBoxes.top();
  while(!m_vBoxes.empty() && hb == m_vBoxes.top())
    m_vBoxes.pop();
  ++m_nPopped;
}

void BoundingBoxes::initialize(const std::deque<Packer*>& vPackers) {
  m_pPacker = vPackers.front();
}

void BoundingBoxes::initialize(Packer* pPacker) {
  m_pPacker = pPacker;
}

void BoundingBoxes::printAttempt(const BoxDimensions& b) const {
  if(!m_pParams->m_bQuiet) {
    std::cout << "Trying " << m_pParams->unscale(b.m_nWidth)
	      << " X " << m_pParams->unscale(b.m_nHeight)
	      << " = " << m_pParams->unscale2(b.m_nArea)
	      << ", Empty = ";
    URational nEmpty(b.m_nArea);
    nEmpty -= m_pParams->m_vInstance.m_nTotalArea;
    nEmpty = m_pParams->unscale2(nEmpty);
    nEmpty.print();
    std::cout << std::endl;
  }
}

bool BoundingBoxes::singleRun() {
  HeapBox hb;
  hb.m_Box.initialize(m_pParams->m_Box.m_nWidth.get_ui(),
		      m_pParams->m_Box.m_nHeight.get_ui());
  printAttempt(hb.m_Box);
  bool bResult = m_pPacker->pack(hb);
  if(bQuit) return(false);
  m_TotalTime += m_pPacker->m_Time;
  m_TotalYTime += m_pPacker->m_YTime;
  m_TotalXTime += m_pPacker->m_XTime;
  m_Total.accumulate(m_pPacker->m_Nodes);
  if(bResult) {
    Placements v;
    AsciiGrid ag;
    m_pPacker->get(v);
    if(m_pPacker->m_bInstanceRotated)
      v.rotate();
    if(!v.verify())
      std::cout << "  Error: Constraints not satisfied." << std::endl;
    ag.draw(m_pParams, v);
    ag.print();
    std::cout << "  Rectangles were placed in these (x,y) locations:"
	      << std::endl;
    v *= m_pParams->m_vInstance.m_nScale;
    v.print();
    std::cout << std::endl << std::endl;
  }
  return(bResult);
}

bool BoundingBoxes::getNext(HeapBox& b) {
  if(m_vBoxes.empty()) return(false);
  HeapBox hb;
  pop(hb);
  b = hb;
  return(true);
}

bool BoundingBoxes::pushNext(const HeapBox& hb) {

  HeapBox b(hb);

  /**
   * HACK: Replace the current height iterator with the one that is
   * most appropriate based on our conflict learning.
   */

  if(m_bConflictLearning) {
    const SubsetSums* ss = &m_vHeights[m_pPacker->m_nDeepestConflict];
    b.m_iHeight = ss->upper_bound(b.m_iHeight->first);
    --b.m_iHeight;
    b.m_nConflictLearningIndex = m_pPacker->m_nDeepestConflict;
  }

  /**
   * Generate a box where the height has been increased.
   */

  ++b.m_iHeight;
  if(m_bConflictLearning) {
    while(b.m_iHeight != m_vHeights[b.m_nConflictLearningIndex].end()) {
      SubsetSums::const_iterator i = m_Widths.find(b.m_Box.m_nWidth);
      if(m_bMutexChecking && !b.intersect(i)) {
	++b.m_iHeight;
      }
      else
	break;
    }
    if(b.m_iHeight != m_vHeights[b.m_nConflictLearningIndex].end()) {
      b.initializeH(b.m_iHeight);
      m_vBoxes.push(b);
    }
  }
  else {
    while(b.m_iHeight != m_Heights.end()) {
      SubsetSums::const_iterator i = m_Widths.find(b.m_Box.m_nWidth);
      if(m_bMutexChecking && !b.intersect(i)) {
	++b.m_iHeight;
      }
      else
	break;
    }
    if(b.m_iHeight != m_Heights.end()) {
      b.initializeH(b.m_iHeight);
      m_vBoxes.push(b);
    }
  }

  /**
   * Generate a new class of bounding boxes relating to an increased
   * width.
   */

  enqueueNewWidths(b.m_Box.m_nArea);
  return(true);
}

bool BoundingBoxes::pushNextInt(const HeapBox& hb) {

  HeapBox b(hb);

  /**
   * Generate a box where the height has been increased.
   */

  ++b.m_Box.m_nHeight;
  if(b.m_Box.m_nHeight <= m_pParams->m_vInstance.m_nStacked.m_nHeight.get_ui()) {
    b.m_Box.initializeH(b.m_Box.m_nHeight);
    m_vBoxes.push(b);
  }
  
  /**
   * Generate a new class of bounding boxes relating to an increased
   * width.
   */

  enqueueNewIntWidths(b.m_Box.m_nArea);
  return(true);
}

std::string BoundingBoxes::solutions() const {
  std::ostringstream oss;
  for(std::set<Attempt>::const_iterator i = m_sAttempts.begin();
      i != m_sAttempts.end(); ++i)
    if(i->m_bResult) {
      RDimensions box(i->m_Box);
      box *= m_pParams->m_vInstance.m_nScale;
      oss << box << " ";
    }
  return(oss.str());
}

UInt BoundingBoxes::attempts() const {
  return((UInt) m_sAttempts.size());
}

void BoundingBoxes::enqueueNewWidths(UInt nMinArea) {

  /**
   * The max area will tells us when to stop considering additional
   * widths. The minimum area as passed in, tells us what we know
   * about the lower bound on the area. These two bounds both help us
   * decide what to insert into our heap.
   */

  UInt nMaxArea =
    m_vBoxes.empty() ? m_nMaxArea :
    m_vBoxes.top().m_Box.m_nArea;
  
  HeapBox hb;
  hb.m_nConflictLearningIndex = m_pParams->m_vInstance.size() - 1;
  WidthHeight wh;
  for(; m_iWidth != m_Widths.end(); ++m_iWidth) {

    /**
     * Is this box, and all subsequent boxes of larger width outside
     * of the area bound that we're considering? To compute this we
     * use the maximum area to decide when we should stop.
     */

    UInt nHeight(m_pParams->m_vInstance.m_nMaxMin.m_nHeight.get_ui());
    if(m_pParams->m_vInstance.m_bDiagonalSymmetry)
      nHeight = std::max(nHeight, m_iWidth->first);
    if(nHeight * m_iWidth->first > nMaxArea) break;

    /**
     * We're not outside of the maximum area allowed. Therefore, we'll
     * use the minimum area and derive all bounding boxes possible for
     * this width. Augment the height as necessary based on our
     * constraints.
     */

    nHeight = nMinArea / m_iWidth->first;
    if(nMinArea % m_iWidth->first) ++nHeight;
    nHeight = std::max(nHeight, m_pParams->m_vInstance.m_nMaxMin.m_nHeight.roundUp());
    nHeight = std::max(nHeight, m_pParams->m_vInstance.minDimPairs(m_iWidth->first, &wh).roundUp());
    nHeight = std::max(nHeight, m_pParams->m_vInstance.minDimStacked(m_iWidth->first, &wh).roundUp());
    if(m_pParams->m_vInstance.m_bDiagonalSymmetry)
      nHeight = std::max(nHeight, m_iWidth->first);
    hb.m_iHeight = m_Heights.lower_bound(nHeight);

    /**
     * There is one last problem. It's possible that if we ran
     * Instance::minDimStacked in the other dimension, we would be
     * required to increase the bound on the height. So we perform the
     * following test and look up a new iterator if applicable.
     */

    bool bOpenInterval(false);
    URational nHeight2 =
      m_pParams->m_vInstance.minDimStacked2(m_iWidth->first, &wh, bOpenInterval);
    if(bOpenInterval) 
      while(nHeight2 >= (URational) hb.m_iHeight->first) ++hb.m_iHeight;
    else
      while(nHeight2 > (URational) hb.m_iHeight->first) ++hb.m_iHeight;

    /**
     * Now we must increase the height to the point that the
     * rectangles comprising the width do not overlap with those
     * comprising the height (overlap must not exceed 1
     * rectangle). Furthermore, the result tells us if particular
     * rectangles can be rotated or not in the actual packing attempt.
     */

    while(hb.m_iHeight != m_Heights.end()) {
      if(m_bMutexChecking && !hb.intersect(m_iWidth)) {
	++hb.m_iHeight;
      }
      else
	break;
    }

    /**
     * Another bound is the empty space required given that perhaps a
     * largest rectangle leaves a large empty strip to its right. This
     * specifically occurs with our unoriented high-precision
     * benchmark. It's unlikely that this will happen for any of the
     * other benchmarks.
     */

    if(m_bTopLevelWaste && m_pParams->m_vInstance.m_nBenchmark == 5) {
      UInt nRectArea = m_nMinArea.get_ui();
      UInt nRectWidth = m_pPacker->m_vRects.front().m_nMinDim;
      UInt nWasteWidth = m_iWidth->first - nRectWidth;
      UInt nWasteArea = nWasteWidth * m_pPacker->m_vRects.front().m_nMaxDim;
      UInt nConsumedWaste(0);
      for(RectArray::const_reverse_iterator i = m_pPacker->m_vRects.rbegin();
	  i != m_pPacker->m_vRects.rend(); ++i)
	if(nWasteWidth < i->m_nMinDim)
	  break;
	else
	  nConsumedWaste += i->m_nArea;
      if(nConsumedWaste < nWasteArea)
	nWasteArea -= nConsumedWaste;
      else
	nWasteArea = 0;
      UInt nBoxAreaRequired = nRectArea + nWasteArea;
      UInt nHeightRequired = nBoxAreaRequired / m_iWidth->first;
      if(nBoxAreaRequired % m_iWidth->first)
	++nHeightRequired;
      
      while(hb.m_iHeight != m_Heights.end() &&
	    hb.m_iHeight->first < nHeightRequired)
	++hb.m_iHeight;
      while(hb.m_iHeight != m_Heights.end()) {
	if(m_bMutexChecking && !hb.intersect(m_iWidth)) {
	  ++hb.m_iHeight;
	}
	else
	  break;
      }
    }

    /**
     * Are the current height and subsequent heights valid for the
     * class of boxes with this width? If not, we're done with this
     * class of boxes and don't need to test any.
     */

    if(hb.m_iHeight == m_Heights.end()) continue;
    nHeight = hb.m_iHeight->first;

    /**
     * This box is important, so we insert it into our priority queue.
     */

    hb.initialize(m_iWidth, hb.m_iHeight);
    m_vBoxes.push(hb);
  }
}

void BoundingBoxes::enqueueNewIntWidths(UInt nMinArea) {

  /**
   * The max area will tells us when to stop considering additional
   * widths. The minimum area as passed in, tells us what we know
   * about the lower bound on the area. These two bounds both help us
   * decide what to insert into our heap.
   */

  UInt nMaxArea =
    m_vBoxes.empty() ? m_nMaxArea :
    m_vBoxes.top().m_Box.m_nArea;
  
  HeapBox hb;
  hb.m_nConflictLearningIndex = m_pParams->m_vInstance.size() - 1;
  WidthHeight wh;
  for(; m_nWidth <= m_pParams->m_vInstance.m_nStacked.m_nWidth.get_ui();
      ++m_nWidth) {

    /**
     * Is this box, and all subsequent boxes of larger width outside
     * of the area bound that we're considering? To compute this we
     * use the maximum area to decide when we should stop.
     */

    UInt nHeight(m_pParams->m_vInstance.m_nMaxMin.m_nHeight.get_ui());
    if(m_pParams->m_vInstance.m_bDiagonalSymmetry)
      nHeight = std::max(nHeight, m_nWidth);
    if(nHeight * m_nWidth > nMaxArea) break;

    /**
     * We're not outside of the maximum area allowed. Therefore, we'll
     * use the minimum area and derive all bounding boxes possible for
     * this width. Augment the height as necessary based on our
     * constraints.
     */

    nHeight = nMinArea / m_nWidth;
    if(nMinArea % m_nWidth) ++nHeight;
    nHeight = std::max(nHeight, m_pParams->m_vInstance.m_nMaxMin.m_nHeight.roundUp());
    nHeight = std::max(nHeight, m_pParams->m_vInstance.minDimPairs(m_nWidth, &wh).roundUp());
    nHeight = std::max(nHeight, m_pParams->m_vInstance.minDimStacked(m_nWidth, &wh).roundUp());
    if(m_pParams->m_vInstance.m_bDiagonalSymmetry)
      nHeight = std::max(nHeight, m_nWidth);
    hb.m_Box.m_nHeight = nHeight;

    /**
     * There is one last problem. It's possible that if we ran
     * Instance::minDimStacked in the other dimension, we would be
     * required to increase the bound on the height. So we perform the
     * following test and look up a new iterator if applicable.
     */

    bool bOpenInterval(false);
    URational nHeight2 =
      m_pParams->m_vInstance.minDimStacked2(m_nWidth, &wh, bOpenInterval);
    if(bOpenInterval) 
      hb.m_Box.m_nHeight = std::max(hb.m_Box.m_nHeight, nHeight2.get_ui() + 1);
    else
      hb.m_Box.m_nHeight = std::max(hb.m_Box.m_nHeight, nHeight2.roundUp());

    /**
     * Are the current height and subsequent heights valid for the
     * class of boxes with this width? If not, we're done with this
     * class of boxes and don't need to test any.
     */

    if(hb.m_Box.m_nHeight > m_pParams->m_vInstance.m_nStacked.m_nHeight.get_ui())
      continue;

    /**
     * This box is important, so we insert it into our priority queue.
     */

    hb.m_Box.initialize(m_nWidth, hb.m_Box.m_nHeight);
    m_vBoxes.push(hb);
  }
}

void BoundingBoxes::lowerBoundSolution() {

  /**
   * Scale the instance to a size to make the last rectangle a 1x1,
   * then round it down to the nearest integer for a relaxed version.
   */

  Instance v = m_pParams->m_vInstance;
  m_nMaxArea = v.m_nStacked.m_nWidth.get_ui() * v.m_nStacked.m_nHeight.get_ui();
  v[v.size() - 2] = v[v.size() - 1];
  v[v.size() - 3] = v[v.size() - 1];

  /**
   * Construct the necessary command line parameters.
   */

  std::vector<std::string> vs;
  vs.push_back(m_pParams->m_pArgv[0]);
  if(v.m_bUnoriented)
    vs.push_back("-u");
  vs.push_back("-v");
  vs.push_back("-0");
  vs.push_back("l");
  vs.push_back("-w");
  vs.push_back("5");
  vs.push_back("-1");
  vs.push_back("d");
  vs.push_back("-b");
  vs.push_back("0");
  vs.push_back("-i");
  std::ostringstream oss;
  oss << v;
  vs.push_back(oss.str());

  /**
   * Parse the parameters and run the solver.
   */

  Parameters params;
  params.parseCommand(vs);

  std::cout << "Scaling from " << m_pParams->m_vInstance << std::endl;
  std::cout << "to " << params.m_vInstance << std::endl;

  Packer* pPacker = Packer::create(&params);
  BoundingBoxes* pBoxes = new BoundingBoxes();
  pBoxes->initialize(pPacker);
  pPacker->initialize(pBoxes);
  pPacker->initialize(&params);
  pBoxes->initialize(&params);
  pBoxes->run();

  /**
   * Retrieve the minimum area.
   */

  URational nMinArea(0);
  for(std::set<Attempt>::const_iterator i = pBoxes->m_sAttempts.begin();
      i != pBoxes->m_sAttempts.end(); ++i) {
    if(i->m_bResult) {
      RDimensions rd(i->m_Box);
      rd *= params.m_vInstance.m_nScale;
      nMinArea = rd.m_nArea;
      std::cout << "Lower bound area is " << nMinArea << std::endl;
      break;
    }
  }
  delete pPacker;
  delete pBoxes;

  m_nMinArea = std::max(m_nMinArea, nMinArea);
}

void BoundingBoxes::upperBoundSolution() {
}
