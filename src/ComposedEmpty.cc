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
#include "Component.h"
#include "Globals.h"
#include "IntPlacements.h"
#include "NodeCount.h"
#include "Printer.h"
#include "Packer.h"
#include "Placements.h"
#include "TimeSpec.h"
#include "ComposedEmpty.h"
#include <iomanip>

ComposedEmpty::ComposedEmpty() :
  m_pCuSP(NULL),
  m_pBox(NULL),
  m_pTime(NULL),
  m_pNodes(NULL),
  m_nDepth(0),
  m_nCorners(0) {
}

ComposedEmpty::~ComposedEmpty() {
}

void ComposedEmpty::initialize(Packer* pPacker) {
  SpaceFill::initialize(pPacker);
  m_pCuSP = &pPacker->m_nCuSP;
  m_pTime = &pPacker->m_YTime;
  m_pNodes = &pPacker->m_Nodes;
  m_vValues.clear();
  for(RectPtrArray::const_iterator i = pPacker->m_vRectPtrs.begin();
      i != pPacker->m_iFirstUnit; ++i)
    m_vValues.push_back(IntPlacement(*i, 0, 0));
  m_nDepth = m_vValues.size();
}

void ComposedEmpty::initialize(const Parameters* pParams) {
  SpaceFill::initialize(pParams);
}

void ComposedEmpty::initialize(const BoxDimensions* pBox) {
  m_pBox = pBox;
}

bool ComposedEmpty::packY(Cumulative& c) {
  TimeSpec t;
  t.tick();
  ++(*m_pCuSP);

  /**
   * Copy over the x-coordinates.
   */

  m_lAssigned.clear();
  m_lUnassigned.clear();
  for(size_t i = 0; i < m_vValues.size(); ++i) {
    m_vValues[i].m_nX = c.values()[i].m_nValue.left();
    m_lUnassigned.insert(m_vValues[i].m_pRect);
  }

  m_lVariables.clear();
  Intersect j;
  j.m_nCorner.m_nX = 0;
  j.m_nCorner.m_nY = 0;
  j.m_pTop = NULL;
  j.m_pRight = NULL;
  m_lVariables.insert(j);
  
  bool bResult(packYAux(0));
  t.tock();
  (*m_pTime) += t;
  if(bResult) return(true);
  else {
    while(!m_vStack.empty())
      pop();
    return(false);
  }
}

bool ComposedEmpty::packYAux(Int nDepth) {
  if(bQuit) return(false);

  if(m_nDepth == nDepth) return(true);
  
  /**
   * Find the next variable to branch on.
   */

  std::set<Intersect>::iterator iVar;
  getNextVar(iVar);
  if(iVar == m_lVariables.end())
    return(false);
  
  /**
   * We have a variable. Now let's branch on the different rectangles
   * that can be placed here.
   */

  for(std::set<IntPlacement*>::iterator i = m_lUnassigned.begin();
      i != m_lUnassigned.end(); ++i) {
    IntPlacement* pCurrent = &m_vValues[(*i)->m_nID];
    pCurrent->m_nY = iVar->m_nCorner.m_nY;

    /**
     * Ignore values with different x-coordinates.
     */

    if(pCurrent->m_nX != iVar->m_nCorner.m_nX)
      continue;

    /**
     * Ignore values that exceed the bounding box height.
     */

    if(pCurrent->y2() > m_pBox->m_nHeight)
      continue;

    /**
     * Ignore values that can't be propped up by the given
     * rectangles. That is, the value must x-overlap the bottom, and
     * y-overlap the left side.
     */

    const Rectangle* pTop = iVar->m_nCorner.m_pTop;
    const Rectangle* pRight = iVar->m_nCorner.m_pRight;
    if((pTop == NULL || pCurrent->overlapsX(m_vValues[pTop->m_nID])) &&
       (pRight == NULL || pCurrent->overlapsY(m_vValues[pRight->m_nID]))) {
    }
    else
      continue;

    /**
     * Now test for non-overlap.
     */

    std::set<IntPlacement*>::const_iterator j;
    for(j = m_lAssigned.begin(); j != m_lAssigned.end(); ++j)
      if(pCurrent->overlaps(**j))
	break; // Found overlap.
    if(j != m_lAssigned.end())
      continue; // Found overlap. Try another assignment.

    /**
     * At this point there is no overlap. So we make the assignment
     * permanent and recurse!
     */

    m_lUnassigned.erase(i);
    m_lAssigned.insert(pCurrent);
    std::list<std::list<Intersect>::iterator> lVars;
    generateNewVariables(pCurrent, lVars);

    if(packYAux(nDepth + 1))
      return(true);

    while(!lVars.empty()) {
      m_lVariables.erase(lVars.back());
      lVars.pop_back();
    }
    m_lAssigned.erase(pCurrent);
    i = m_lUnassigned.insert(pCurrent);
  }
  return(false);
}

void ComposedEmpty::get(Placements& v) const {
  ComponentPtrs::const_iterator j = m_Transform.m_vComponents.begin();
  for(Placements::iterator i = v.begin(); i != v.end(); ++i, ++j)
    i->m_nLocation.y = (URational) (*j)->m_nY;
}

void ComposedEmpty::get(IntPlacements& v) const {
  ComponentPtrs::const_iterator j = m_Transform.m_vComponents.begin();
  for(IntPlacements::iterator i = v.begin(); i != v.end(); ++i, ++j)
    i->m_nY = (UInt) (*j)->m_nY;
}

void ComposedEmpty::() {
}

void ComposedEmpty::getNextVar(std::set<Intersect>::iterator& i) {
  size_t nMin(std::numeric_limits<size_t>::max());
  i = m_lVariables.end();
  for(std::set<Intersect::iterator j = m_lVariables.begin();
      j != m_lVariables.end(); ++j) {
    size_t nSize = valuesAt(i->m_nCorner.m_nX);
    if(nSize == 0) {

      /**
       * If there's a domain of 0, ultimately we should check to
       * ensure that empty space can fill this space up. But for now
       * we just ignore the 0's, assuming that variable is done.
       */

    }
    else {
      nMin = std::min(nMin, nSize);
      i = j;
    }
  }
}

size_t ComposedEmpty::valuesAt(UInt x) {
  size_t nCount(0);
  for(std::set<IntPlacement*>::const_iterator i = m_lUnassigned.begin();
      i != m_lUnassigned.end(); ++i)
    if(m_vValues[(*i)->m_nID].m_nX == x)
      ++nCount;
  return(nCount);
}

void ComposedEmpty::generateNewVariables(IntPlacement* pCurrent,
					 std::list<std::list<Intersect>::iterator>& lVars) {
  
  /**
   * We've made one assignment. Now let's see what new intersections
   * we've created. We can provide only a top and a right side.
   */

  /**
   * If we provided a top that is a viable intersection, then the
   * other rectangle is either to our left, diagonally up and to the
   * left, or above us.
   */

  std::list<Intersect> lNew;
  for(std::set<IntPlacement*>::iterator i = m_lAssigned.begin();
      i != m_lAssigned.end(); ++i) {
    if(pCurrent->y2() > (*i)->y1() &&
       pCurrent->y2() < (*i)->y2())
      lNew.push_back(Intersect(pCurrent, *i));
  }
}
