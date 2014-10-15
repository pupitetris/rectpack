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
#include "MetaCSP.h"
#include "Parameters.h"
#include <stdlib.h>

MetaCSP::MetaCSP() {
}

MetaCSP::~MetaCSP() {
}

bool MetaCSP::pack() {
  initVariableDescriptions();
  m_vStack.clear();
  m_vStack.reserve(m_vVariableDescs.size());
  m_vStack.resize(1);
  m_vStack.back().initialize(m_vVariableDescs);
  m_Best.clear();
  m_nBestReplacements = 0;

  if(m_vRectPtrs.size() == 1) {
    m_Box.initialize(m_vRects[0].m_nWidth,
		     m_vRects[0].m_nHeight);
    m_Best.set(m_Box);
    printRecentBest();
    return(true);
  }
  if(m_pParams->m_vInstance.m_bDiagonalSymmetry)
    breakInstanceSymmetry();
  orientation();
  return(true);
}

void MetaCSP::packAux() {

  /**
   * If we found a valid solution, then see if we can update our
   * current solution.
   */

  if(allAssigned()) {

    /**
     * If the solution has been updated, then we should print out the
     * new findings.
     */

    if(updateSolution() && !m_pParams->m_bQuiet)
      printRecentBest();
    return;
  }

  /**
   * Select the next variable and branch on its values.
   */

  MetaFrame::VarIter i = nextVariable();
  branchOn(i);
}

void MetaCSP::printRecentBest() const {
  BoxDimensions b = m_Best.back();
  std::cout << "Solution " << m_pParams->unscale(b.m_nWidth)
	    << " X " << m_pParams->unscale(b.m_nHeight)
	    << " = " << m_pParams->unscale2(b.m_nArea)
	    << ", Empty = ";
  URational nEmpty(b.m_nArea);
  nEmpty -= m_pParams->m_vInstance.m_nTotalArea;
  nEmpty = m_pParams->unscale2(nEmpty);
  nEmpty.print();
  std::cout << std::endl;

  AsciiGrid ag;
  Placements v;
  get(v);
  if(m_bInstanceRotated)
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

void MetaCSP::branchOn(MetaFrame::VarIter& i) {

  /**
   * Dynamically break symmetry, if applicable.
   */

  dynamicSymmetry(i);

  /**
   * Perform value ordering.
   */

  std::vector<int> v;
  valueOrdering(i, v);
  
  /**
   * Iterate over each possible value in the domain.
   */

  for(std::vector<int>::const_iterator j = v.begin();
      j != v.end(); ++j) {

    /**
     * This is considered a generated node, beecause we do a lot of
     * work propagating constraints and value assignments.
     */

    m_Nodes.tick(XF);

    /**
     * Save the current stack frame because we're about to make
     * modifications.
     */

    m_vStack.push_back(MetaFrame(m_vStack.back()));
    MetaFrame* pCurrent = &m_vStack.back();
    pCurrent->assign(i->first, (MetaDomain::ValueT) *j);

    /**
     * Assert our constraint into our APSP and check it for negative
     * cycles.
     */

    if(*j == MetaDomain::ABOVE || *j == MetaDomain::BELOW)
      pCurrent->m_vYMatrix.assign(i->first, *j);

    /**
     * @TODO Optimizations may be performed here, by focusing on
     * only propagating the changes that we've made instead of
     * recomputing the entire matrix.
     */

    else
      pCurrent->m_vXMatrix.assign(i->first, *j);

    if(computeAPSP()) {

      /**
       * Subsume variables.
       */
      
      pCurrent->subsumeVariables();

      /**
       * Perform forward checking. This version simply removes domain
       * values that are inconsistent with the APSP.
       */

      if(pCurrent->forwardChecking()) 
	
	/**
	 * Perform clique detection in order to derive tighter bounds
	 * on our minimum height and width.
	 */
	
	if(pCurrent->updateMinimumBounds(cliqueWidth(),
					 cliqueHeight(), 
					 m_Best.m_nArea))
	  
	  /**
	   * Make sure that we don't exceed bounds on the bounding box
	   * in our APSP matrix. Also, eliminate any domain
	   * assignments that would exceed our bounding box
	   * restrictions as well.
	   */
	  
	  if(pCurrent->forwardCheckBounds(m_Best))
	    packAux();
    }
    
    /**
     * Restore the previous stack frame.
     */
    
    m_vStack.pop_back();
    semanticBranching(i, *j);
  }
}

void MetaCSP::semanticBranching(MetaFrame::VarIter& v, int n) {
  MetaFrame* pCurrent = &m_vStack.back();
  if(n == MetaDomain::LEFTOF || n == MetaDomain::RIGHTOF)
    pCurrent->m_vXMatrix.negate(v->first, n);
  else
    pCurrent->m_vYMatrix.negate(v->first, n);
}

bool MetaCSP::computeAPSP() {
  MetaFrame* pCurrent = &m_vStack.back();
  pCurrent->m_vXMatrix.floydWarshall();
  if(pCurrent->m_vXMatrix.negativeCycles())
    return(false);
  pCurrent->m_vYMatrix.floydWarshall();
  if(pCurrent->m_vYMatrix.negativeCycles())
    return(false);
  return(true);
}

bool MetaCSP::allAssigned() const {
  return(m_vStack.back().m_Unassigned.empty());
}

bool MetaCSP::updateSolution() {

  /**
   * @TODO Stopped here! We need to code up logic to dynamically infer
   * the correct bounding box width and height here and update the
   * best solution (or at least be able to conduct the appropriate
   * check).
   */

  m_Box.initialize(m_vStack.back().m_nMinWidth,
		   m_vStack.back().m_nMinHeight);
  if(m_Box.m_nArea < m_Best.m_nArea) {
    m_Best.set(m_Box);
    return(true);
  }
  else if(m_Box.m_nArea == m_Best.m_nArea &&
	  !m_Best.contains(m_Box)) {
    m_Best.push_back(m_Box);
    return(true);
  }
  else
    return(false);
}

MetaFrame::VarIter MetaCSP::nextVariable() {

  /**
   * Return any variables that have only one choice.
   */
  
  for(MetaFrame::VarIter i = m_vStack.back().m_Unassigned.begin();
      i != m_vStack.back().m_Unassigned.end(); ++i)
    if(i->second.m_Domain.size() == 1)
      return(i);

  /**
   * Select the meta-variables that maximize min(areai, areaj).
   */

  UInt nBestScore = 0;
  std::vector<MetaFrame::VarIter> vReturn;
  for(MetaFrame::VarIter i = m_vStack.back().m_Unassigned.begin();
      i != m_vStack.back().m_Unassigned.end(); ++i)
    if(i->first->m_nMinArea == nBestScore)
      vReturn.push_back(i);
    else if(i->first->m_nMinArea > nBestScore) {
      vReturn.clear();
      vReturn.push_back(i);
      nBestScore = i->first->m_nMinArea;
    }

  /**
   * Of these, select randomly from the set of meta-variables that
   * maximize max(areai, areaj).
   */

  nBestScore = 0;
  std::vector<MetaFrame::VarIter> vReturn2;
  for(size_t i = 0; i < vReturn.size(); ++i)
    if(vReturn[i]->first->m_nMaxArea == nBestScore)
      vReturn2.push_back(vReturn[i]);
    else if(vReturn[i]->first->m_nMaxArea > nBestScore) {
      vReturn2.clear();
      vReturn2.push_back(vReturn[i]);
      nBestScore = vReturn[i]->first->m_nMaxArea;
    }
  return(vReturn2[random() % vReturn2.size()]);  
}

void MetaCSP::initVariableDescriptions() {
  m_vVariableDescs.clear();
  m_vVariableDescs.resize(m_vRectPtrs.size());
  for(size_t i = 0; i < m_vRectPtrs.size(); ++i) {
    m_vVariableDescs[i].resize(i);
    for(size_t j = 0; j < i; ++j)
      m_vVariableDescs[i][j].initialize(m_vRectPtrs[i],
					m_vRectPtrs[j]);
  }
}

UInt MetaCSP::cliqueWidth() const {

  /**
   * @TODO We can precompute the sorted set of rectangles and store
   * this somewhere instead of doing it here at every node.
   */

  std::vector<WidthFirst> v;
  for(size_t i = 0; i < m_vRectPtrs.size(); ++i)
    v.push_back(WidthFirst(m_vRectPtrs[i]));
  std::sort(v.rbegin(), v.rend());
  UInt nWidth = cliqueWidthAux(v);

  /**
   * Now try this from the smallest width up to the largest and report
   * the maximum of the two.
   */

  std::reverse(v.begin(), v.end());
  UInt nWidth2 = cliqueWidthAux(v);
  return(std::max(nWidth, nWidth2));
}

UInt MetaCSP::cliqueWidthAux(const std::vector<WidthFirst>& v) const {

  /**
   * Start with the first rectangle, and then iterate over the
   * remaining rectangles in order. Add the rectangle into the set
   * only if it forms a clique with all the other rectangles in the
   * current set.
   */

  std::set<const Rectangle*> s;
  for(std::vector<WidthFirst>::const_iterator i = v.begin();
      i != v.end(); ++i) {    
    bool bClique(true);
    for(std::set<const Rectangle*>::const_iterator j = s.begin();
	j != s.end(); ++j) {

      /**
       * Check each rectangle in the set, and verify that the new
       * rectangle is forced to have a horizontal disjunct with this
       * rectangle.
       */

      if(!horizontal(i->m_pRect, *j)) {
	bClique = false;
	break;
      }
    }
    if(bClique)
      s.insert(i->m_pRect);
  }

  /**
   * Compute the width based on our clique.
   */

  UInt nWidth(0);
  for(std::set<const Rectangle*>::const_iterator i = s.begin();
      i != s.end(); ++i)
    nWidth += (*i)->m_nWidth;
  return(nWidth);
}

UInt MetaCSP::cliqueHeight() const {

  /**
   * @TODO We can precompute the sorted set of rectangles and store
   * this somewhere instead of doing it here at every node.
   */

  std::vector<HeightFirst> v;
  for(size_t i = 0; i < m_vRectPtrs.size(); ++i)
    v.push_back(HeightFirst(m_vRectPtrs[i]));
  std::sort(v.rbegin(), v.rend());
  UInt nHeight = cliqueHeightAux(v);

  /**
   * Then try the reverse sequence.
   */

  std::reverse(v.begin(), v.end());
  UInt nHeight2 = cliqueHeightAux(v);
  return(std::max(nHeight, nHeight2));
}

UInt MetaCSP::cliqueHeightAux(const std::vector<HeightFirst>& v) const {

  /**
   * Start with the first rectangle, and then iterate over the
   * remaining rectangles in order. Add the rectangle into the set
   * only if it forms a clique with all the other rectangles in the
   * current set.
   */

  std::set<const Rectangle*> s;
  for(std::vector<HeightFirst>::const_iterator i = v.begin();
      i != v.end(); ++i) {    
    bool bClique(true);
    for(std::set<const Rectangle*>::const_iterator j = s.begin();
	j != s.end(); ++j) {

      /**
       * Check each rectangle in the set, and verify that the new
       * rectangle is forced to have a horizontal disjunct with this
       * rectangle.
       */

      if(!vertical(i->m_pRect, *j)) {
	bClique = false;
	break;
      }
    }
    if(bClique)
      s.insert(i->m_pRect);
  }

  /**
   * Compute the height based on our clique.
   */

  UInt nHeight(0);
  for(std::set<const Rectangle*>::const_iterator i = s.begin();
      i != s.end(); ++i)
    nHeight += (*i)->m_nHeight;
  return(nHeight);
}

bool MetaCSP::horizontal(const Rectangle* r1,
			 const Rectangle* r2) const {
  if(r2->m_nID > r1->m_nID)
    std::swap(r1, r2);
  const MetaVarDesc* v(&m_vVariableDescs[r1->m_nID][r2->m_nID]);
  MetaFrame::ConstVarIter i, k;
  i = m_vStack.back().m_Assigned.find(v);
  if(i != m_vStack.back().m_Assigned.end())
    return(i->second.m_nValue == MetaDomain::LEFTOF ||
	   i->second.m_nValue == MetaDomain::RIGHTOF);
  else {
    k = m_vStack.back().m_Unassigned.find(v);
    return(!k->second.m_Domain.contains(MetaDomain::ABOVE) &&
	   !k->second.m_Domain.contains(MetaDomain::BELOW));
  }
}

bool MetaCSP::vertical(const Rectangle* r1,
		       const Rectangle* r2) const {
  if(r2->m_nID > r1->m_nID)
    std::swap(r1, r2);
  const MetaVarDesc* v(&m_vVariableDescs[r1->m_nID][r2->m_nID]);
  MetaFrame::ConstVarIter i, k;
  i = m_vStack.back().m_Assigned.find(v);
  if(i != m_vStack.back().m_Assigned.end())
    return(i->second.m_nValue == MetaDomain::ABOVE ||
	   i->second.m_nValue == MetaDomain::BELOW);
  else {
    k = m_vStack.back().m_Unassigned.find(v);
    return(!k->second.m_Domain.contains(MetaDomain::LEFTOF) &&
	   !k->second.m_Domain.contains(MetaDomain::RIGHTOF));
  }
}

void MetaCSP::dynamicSymmetry(MetaFrame::VarIter& v) {
  bool bHorizontalDisjuncts(false);
  bool bVerticalDisjuncts(false);
  MetaFrame* pCurrent = &m_vStack.back();
  for(MetaFrame::ConstVarIter i = pCurrent->m_Assigned.begin();
      i != pCurrent->m_Assigned.end(); ++i) {
    if(i->second.m_nValue == MetaDomain::LEFTOF ||
       i->second.m_nValue == MetaDomain::RIGHTOF)
      bHorizontalDisjuncts = true;
    else
      bVerticalDisjuncts = true;
    if(bHorizontalDisjuncts && bVerticalDisjuncts)
      return;
  }
  if(!bHorizontalDisjuncts)
    v->second.m_Domain.erase(MetaDomain::LEFTOF);
  if(!bVerticalDisjuncts)
    v->second.m_Domain.erase(MetaDomain::BELOW);
}

void MetaCSP::breakInstanceSymmetry() {
  MetaFrame::VarIter i = nextVariable();
  i->second.m_Domain.clear();
  i->second.m_Domain.insert(MetaDomain::RIGHTOF);
}

void MetaCSP::orientation() {
  orientationAux(0);
}

void MetaCSP::orientationAux(size_t nRectangle) {
  if(nRectangle == m_vRectPtrs.size()) {
    packAux();
    return;
  }

  if(m_vRectPtrs[nRectangle]->rotatable()) {
    m_vRectPtrs[nRectangle]->rotate();
    orientationAux(nRectangle + 1);
    m_vRectPtrs[nRectangle]->rotate();
  }
  orientationAux(nRectangle + 1);
}

void MetaCSP::valueOrdering(MetaFrame::VarIter& i,
			    std::vector<int>& v) const {  
  Comparator c;
  const Rectangle* r1(i->first->m_pRect1);
  const Rectangle* r2(i->first->m_pRect2);
  const MetaFrame* pCurrent(&m_vStack.back());

  /**
   * Determine if we would prefer vertical disjuncts to horizontal
   * disjuncts.
   */

  UInt nWidth = r1->m_nWidth + r2->m_nWidth;
  UInt nHeight = r1->m_nHeight + r2->m_nHeight;
  UInt dWidth =
    (pCurrent->m_nMinWidth < nWidth) ?
    (nWidth - pCurrent->m_nMinWidth) : 0;
  UInt dHeight =
    (pCurrent->m_nMinHeight < nHeight) ?
    (nHeight - pCurrent->m_nMinHeight) : 0;
  if(dHeight <= dWidth)
    c.m_bPreferVertical = true;
  else
    c.m_bPreferVertical = false;

  /**
   * Assess the slack factor.
   */

  std::vector<std::pair<int, Int> > v2;
  v2.reserve(i->second.m_Domain.size());
  for(std::set<int>::const_iterator j = i->second.m_Domain.begin();
      j != i->second.m_Domain.end(); ++j) {
    Int nSlack;
    switch(*j) {
    case MetaDomain::LEFTOF:
      nSlack = pCurrent->m_vXMatrix[r1->m_nID][r2->m_nID] -
	r1->m_nWidth;
      break;
    case MetaDomain::RIGHTOF:
      nSlack = pCurrent->m_vXMatrix[r2->m_nID][r1->m_nID] -
	r2->m_nWidth;
      break;
    case MetaDomain::BELOW:
      nSlack = pCurrent->m_vYMatrix[r1->m_nID][r2->m_nID] -
	r1->m_nHeight;
      break;
    case MetaDomain::ABOVE:
      nSlack = pCurrent->m_vYMatrix[r2->m_nID][r1->m_nID] -
	r2->m_nHeight;
      break;
    default:
      break;
    };
    v2.push_back(std::pair<int, Int>(*j, nSlack));
  }
  std::sort(v2.begin(), v2.end(), c);

  /**
   * Insert the results into our output vector.
   */

  v.clear();
  v.reserve(i->second.m_Domain.size());
  for(size_t i = 0; i < v2.size(); ++i)
    v.push_back(v2[i].first);
}

void MetaCSP::get2(Placements& v) const {

  /**
   * Find all rectangle that set the width of the bounding box, and
   * enqueue their partners into the queue. Then we scan for
   * rectangles whose width is equal to the width of the bounding
   * box. They immediately get assigned x=0.
   */

  const MetaFrame* pCurrent = &m_vStack.back();
  std::vector<bool> vAssigned(m_vRects.size(), false);
  std::vector<UInt> vX(m_vRects.size(), 0);
  std::deque<size_t> q;
  for(std::vector<std::vector<MetaVarDesc> >::const_iterator i =
	m_vVariableDescs.begin(); i != m_vVariableDescs.end(); ++i)
    for(std::vector<MetaVarDesc>::const_iterator j = i->begin();
	j != i->end(); ++j) {
      const MetaVarDesc* pDesc = &(*j);
      Int nWidth = pCurrent->m_vXMatrix.minWidth(pDesc);
      if(nWidth == (Int) pCurrent->m_nMinWidth) {
	const MetaVariable* pVar(NULL);
	MetaFrame::ConstVarIter k = pCurrent->m_Assigned.find(pDesc);
	if(k != pCurrent->m_Assigned.end())
	  pVar = &k->second;
	if(pVar->m_nValue == MetaDomain::ABOVE ||
	   pVar->m_nValue == MetaDomain::BELOW)
	  continue;
	vAssigned[pDesc->m_pRect1->m_nID] = true;
	vAssigned[pDesc->m_pRect2->m_nID] = true;
	q.push_back(pDesc->m_pRect1->m_nID);
	q.push_back(pDesc->m_pRect2->m_nID);
	if(pVar->m_nValue == MetaDomain::LEFTOF) {
	  vX[pDesc->m_pRect1->m_nID] = 0;
	  vX[pDesc->m_pRect2->m_nID] =
	    pCurrent->m_nMinWidth - pDesc->m_pRect2->m_nWidth;
	}
	else if(pVar->m_nValue == MetaDomain::RIGHTOF) {
	  vX[pDesc->m_pRect2->m_nID] = 0;
	  vX[pDesc->m_pRect1->m_nID] =
	    pCurrent->m_nMinWidth - pDesc->m_pRect1->m_nWidth;
	}
      }
    }
  for(RectArray::const_iterator i = m_vRects.begin();
      i != m_vRects.end(); ++i)
    if(i->m_nWidth == pCurrent->m_nMinWidth) {
      vX[i->m_nID] = 0;
      vAssigned[i->m_nID] = true;
    }
  while(!q.empty()) {
    size_t i = q.back();
    q.pop_back();
    for(size_t j = 0; j < i; ++j) {
      if(vAssigned[j]) continue;
      Int nWidth = pCurrent->m_vXMatrix.minWidth(&m_vVariableDescs[i][j]);
      if(nWidth > 0)
	vX[j] = (UInt) nWidth - m_vRectPtrs[j]->m_nWidth;
      vAssigned[j] = true;
      q.push_back(j);
    }
  }

  /**
   * Now assign the heights.
   */

  std::fill(vAssigned.begin(), vAssigned.end(), false);
  std::vector<UInt> vY(m_vRects.size(), 0);
  for(std::vector<std::vector<MetaVarDesc> >::const_iterator i =
	m_vVariableDescs.begin(); i != m_vVariableDescs.end(); ++i)
    for(std::vector<MetaVarDesc>::const_iterator j = i->begin();
	j != i->end(); ++j) {
      const MetaVarDesc* pDesc = &(*j);
      Int nHeight = pCurrent->m_vYMatrix.minHeight(pDesc);
      if(nHeight == (Int) pCurrent->m_nMinHeight) {
	const MetaVariable* pVar(NULL);
	MetaFrame::ConstVarIter k = pCurrent->m_Assigned.find(pDesc);
	if(k != pCurrent->m_Assigned.end())
	  pVar = &k->second;
	if(pVar->m_nValue == MetaDomain::LEFTOF ||
	   pVar->m_nValue == MetaDomain::RIGHTOF)
	  continue;
	vAssigned[pDesc->m_pRect1->m_nID] = true;
	vAssigned[pDesc->m_pRect2->m_nID] = true;
	q.push_back(pDesc->m_pRect1->m_nID);
	q.push_back(pDesc->m_pRect2->m_nID);
	if(pVar->m_nValue == MetaDomain::BELOW) {
	  vY[pDesc->m_pRect1->m_nID] = 0;
	  vY[pDesc->m_pRect2->m_nID] =
	    pCurrent->m_nMinHeight - pDesc->m_pRect2->m_nHeight;
	}
	else if(pVar->m_nValue == MetaDomain::ABOVE) {
	  vY[pDesc->m_pRect2->m_nID] = 0;
	  vY[pDesc->m_pRect1->m_nID] =
	    pCurrent->m_nMinHeight - pDesc->m_pRect1->m_nHeight;
	}
      }
    }
  for(RectArray::const_iterator i = m_vRects.begin();
      i != m_vRects.end(); ++i)
    if(i->m_nHeight == pCurrent->m_nMinHeight) {
      vY[i->m_nID] = 0;
      vAssigned[i->m_nID] = true;
    }
  while(!q.empty()) {
    size_t i = q.back();
    q.pop_back();
    for(size_t j = 0; j < i; ++j) {
      if(vAssigned[j]) continue;
      Int nHeight = pCurrent->m_vYMatrix.minHeight(&m_vVariableDescs[i][j]);
      if(nHeight > 0)
	vY[j] = (UInt) nHeight - m_vRectPtrs[j]->m_nHeight;
      vAssigned[j] = true;
      q.push_back(j);
    }
  }

  /**
   * Construct the placement return vector.
   */

  v.resize(m_vRectPtrs.size());
  v.m_Box = m_Box;
  for(size_t i = 0; i < v.size(); ++i) {
    v[i].m_Dims = m_vRects[i];
    v[i].m_nLocation.initialize(MpqWrapper(vX[i], (UInt) 1),
				MpqWrapper(vY[i], (UInt) 1));
  }
}

void MetaCSP::get(Placements& v) const {

  /**
   * Find all rectangle that set the width of the bounding box, and
   * enqueue their partners into the queue. Then we scan for
   * rectangles whose width is equal to the width of the bounding
   * box. They immediately get assigned x=0.
   */

  const MetaFrame* pCurrent = &m_vStack.back();
  std::vector<bool> vAssigned(m_vRects.size(), false);
  std::vector<Int> vX(m_vRects.size(), 0);
  std::deque<size_t> q;
  for(std::vector<std::vector<MetaVarDesc> >::const_iterator i =
	m_vVariableDescs.begin(); i != m_vVariableDescs.end(); ++i)
    for(std::vector<MetaVarDesc>::const_iterator j = i->begin();
	j != i->end(); ++j) {
      const MetaVarDesc* pDesc = &(*j);
      vX[pDesc->m_pRect1->m_nID] =
	std::min(vX[pDesc->m_pRect1->m_nID],
		 pCurrent->m_vXMatrix
		 [pDesc->m_pRect1->m_nID]
		 [pDesc->m_pRect2->m_nID]);
    }
  for(size_t i = 0; i < vX.size(); ++i)
    vX[i] = -vX[i];

  /**
   * Now assign the heights.
   */

  std::fill(vAssigned.begin(), vAssigned.end(), false);
  std::vector<Int> vY(m_vRects.size(), 0);
  for(std::vector<std::vector<MetaVarDesc> >::const_iterator i =
	m_vVariableDescs.begin(); i != m_vVariableDescs.end(); ++i)
    for(std::vector<MetaVarDesc>::const_iterator j = i->begin();
	j != i->end(); ++j) {
      const MetaVarDesc* pDesc = &(*j);
      vY[pDesc->m_pRect1->m_nID] =
	std::min(vY[pDesc->m_pRect1->m_nID],
		 pCurrent->m_vYMatrix
		 [pDesc->m_pRect1->m_nID]
		 [pDesc->m_pRect2->m_nID]);
    }
  for(size_t i = 0; i < vY.size(); ++i)
    vY[i] = -vY[i];

  /**
   * Construct the placement return vector.
   */

  v.resize(m_vRectPtrs.size());
  v.m_Box = m_Box;
  for(size_t i = 0; i < v.size(); ++i) {
    v[i].m_Dims = m_vRects[i];
    v[i].m_nLocation.initialize(MpqWrapper(vX[i], (Int) 1),
				MpqWrapper(vY[i], (Int) 1));
  }
}

void MetaCSP::placeUnitRectangles() {
}

TimeSpec& MetaCSP::timeDomination() {
  return(m_nTimeDomination);
}
