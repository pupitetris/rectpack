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
#include "Cumulative.h"
#include "EmptySpace.h"
#include "Globals.h"
#include "IntPlacements.h"
#include "NodeCount.h"
#include "Parameters.h"
#include "Printer.h"
#include "Packer.h"
#include "Placements.h"
#include "SubsetSums.h"
#include "TimeSpec.h"
#include "DynamicEmpty.h"
#include <iomanip>

DynamicEmpty::DynamicEmpty() :
  m_pCuSP(NULL),
  m_pBox(NULL),
  m_pTime(NULL),
  m_pNodes(NULL),
  m_nDepth(0),
  m_nCorners(0),
  m_pValidY(NULL),
  m_bDominated(true),
  m_pTopSymmetry(NULL),
  m_nNewY(0) {
}

DynamicEmpty::~DynamicEmpty() {
}

void DynamicEmpty::initialize(Packer* pPacker) {
  SpaceFill::initialize(pPacker);
  m_pCuSP = &pPacker->m_nCuSP;
  m_pTime = &pPacker->m_YTime;
  m_pNodes = &pPacker->m_Nodes;
}

void DynamicEmpty::initialize(const Parameters* pParams) {
  m_Transform.initialize(pParams);
  SpaceFill::initialize(pParams);
  if(pParams->m_s2.find_first_of('D') < pParams->m_s2.size())
    m_bDominated = false;
}

void DynamicEmpty::initialize(const BoxDimensions* pBox) {
  m_pBox = pBox;
  m_Transform.initialize(m_pBox);
}

bool DynamicEmpty::packY(Cumulative& c, const SubsetSums* pValidY) {
  TimeSpec t;
  t.tick();
  ++(*m_pCuSP);

  /**
   * Calculate the number of components we have to combinatorially
   * explore.
   */

  m_Transform.initialize(c, m_Left, m_Right);
  m_LeftCongruencies.initialize(m_Left);
  m_RightCongruencies.initialize(m_Right);

  m_nDepth = m_Transform.m_nTopLevel;
  m_vStackSize.clear(); m_vStackSize.reserve(m_nDepth);
  m_vStack.clear(); m_vStack.reserve(m_nDepth);

  /**
   * Size our data arrays and set up the initial bounding box corner.
   */

  m_vVariables.initialize(*m_pBox);
  m_vValues.initialize(m_RightCongruencies, m_LeftCongruencies);

  m_NewBox = *m_pBox;
  m_nNewY = 0;
  Int nStartDepth(0);
  placeDominated(nStartDepth);
  findTopSymmetry();
  computeValidY();

  /**
   * Run the search.
   */

  bool bResult(packYAux(nStartDepth));
  t.tock();
  (*m_pTime) += t;
  if(bResult) return(true);
  else {
    while(!m_vStack.empty())
      pop();
    return(false);
  }
}

bool DynamicEmpty::packYAux(Int nDepth) {
  if(bQuit) return(false);

  /**
   * Base case is when we've got
   */

  if(nDepth == m_nDepth) {
    m_Transform.m_vComponents.instantiateY();
    return(true);
  }
  
  /**
   * Compute the domain sizes in the model of empty corners.
   */

  UInt nSize1;
  EmptyCorner ec;
  smallestDomain(nSize1, ec);
  if(nSize1 == 0) return(false);
  return(packYCorner(ec, nDepth));
}

void DynamicEmpty::smallestDomain(UInt& nSmallest, EmptyCorner& c) {
  nSmallest = std::numeric_limits<UInt>::max();
  UInt nComponents(std::numeric_limits<UInt>::max());
  smallestDomain(nSmallest, nComponents,
		 m_vVariables[NewCornerVariables::LowerLeft],
		 m_vValues[NewCornerValues::Left], c, EmptyCorner::LowerLeft);
  if(nSmallest == 0) return;
  smallestDomain(nSmallest, nComponents,
		 m_vVariables[NewCornerVariables::UpperLeft],
		 m_vValues[NewCornerValues::Left], c, EmptyCorner::UpperLeft);
  if(nSmallest == 0) return;
  smallestDomain(nSmallest, nComponents,
		 m_vVariables[NewCornerVariables::LowerRight],
		 m_vValues[NewCornerValues::Right], c, EmptyCorner::LowerRight);
  if(nSmallest == 0) return;
  smallestDomain(nSmallest, nComponents,
		 m_vVariables[NewCornerVariables::UpperRight],
		 m_vValues[NewCornerValues::Right], c, EmptyCorner::UpperRight);
}

void DynamicEmpty::smallestDomain(UInt& nSize, UInt& nComponents,
			       std::map<UInt, std::set<Int> >& vars,
			       AlignedCongruencies& vals,
			       EmptyCorner& c,
			       EmptyCorner::Corner nType) {
  for(std::map<UInt, std::set<Int> >::iterator x = vars.begin();
      x != vars.end(); ++x)
    if(!x->second.empty()) {
      UInt p = vals[x->first].symPermutations2(m_BCache);
      if(p == 0) {
	nSize = 0;
	return;
      }
      if(p < nSize) {
	nSize = p;
	nComponents = vals[x->first].components();
	c.m_Location.x = x->first;
	c.m_Location.y = *x->second.begin();
	c.m_nType = nType;
      }
      else if(p == nSize) {
	UInt nc = vals[x->first].components();
	if(nc < nComponents) {
	  nComponents = nc;
	  c.m_Location.x = x->first;
	  c.m_Location.y = *x->second.begin();
	  c.m_nType = nType;
	}
      }
    }
}

SymmetryMap* DynamicEmpty::getValues(const EmptyCorner& ec) {
  switch(ec.m_nType) {
  case EmptyCorner::UpperLeft:
    return(&m_vValues[NewCornerValues::Left][ec.m_Location.x]);
  case EmptyCorner::LowerLeft:
    return(&m_vValues[NewCornerValues::Left][ec.m_Location.x]);
  case EmptyCorner::UpperRight:
    return(&m_vValues[NewCornerValues::Right][ec.m_Location.x]);
  case EmptyCorner::LowerRight:
    return(&m_vValues[NewCornerValues::Right][ec.m_Location.x]);
  default:
    return(NULL);
  }
}

void DynamicEmpty::pushAux(Component* c) {
  
  /**
   * Remove all empty corners that have been filled by this component
   * (carefully putting them into our undo stack).
   */

  m_vStack.push_back(Assignment(c));
  m_vVariables.erase(c, m_vStack.back().m_vErased);

  /**
   * Remove the component from our domain values so that we don't
   * consider placing it again.
   */

  m_vValues.erase(c);

  /**
   * Detect any new empty corners that we created by the placement of
   * this component, inserting them into our set of variables as well
   * as inserting them into our undo stack.
   */

  newCorners(c, m_vStack.back().m_vInserted);
  m_vVariables.insert(m_vStack.back().m_vInserted);
}

void DynamicEmpty::pushAux(Component* c, Component* pNew) {
  
  /**
   * Remove all empty corners that have been filled by this component
   * (carefully putting them into our undo stack).
   */

  m_vStack.push_back(Assignment(pNew));
  m_vVariables.erase(pNew, m_vStack.back().m_vErased);

  /**
   * Remove the component from our domain values so that we don't
   * consider placing it again.
   */

  if(c->m_pEmpty->m_nHeightLeft == 0)
    m_vValues.erase(c);

  /**
   * Detect any new empty corners that we created by the placement of
   * this component, inserting them into our set of variables as well
   * as inserting them into our undo stack.
   */

  newCorners(pNew, m_vStack.back().m_vInserted);
  m_vVariables.insert(m_vStack.back().m_vInserted);
}

void DynamicEmpty::popAux() {

  /**
   * Erase the variables that we inserted.
   */

  for(std::vector<EmptyCorner>::const_iterator i =
	m_vStack.back().m_vInserted.begin();
      i != m_vStack.back().m_vInserted.end(); ++i)
    m_vVariables.erase(*i);

  /**
   * Insert the variables that we erased.
   */

  m_vVariables.insert(m_vStack.back().m_vErased);

  /**
   * Insert the values representing the four corners of our assigned
   * component. We don't have to erase anything because we know for
   * sure we haven't erased any values from the domain other than our
   * own.
   */

  m_vValues.insert(m_vStack.back().m_pComponent);
  m_vStack.pop_back();
}

void DynamicEmpty::popAux(Component* c) {

  /**
   * Erase the variables that we inserted.
   */

  for(std::vector<EmptyCorner>::const_iterator i =
	m_vStack.back().m_vInserted.begin();
      i != m_vStack.back().m_vInserted.end(); ++i)
    m_vVariables.erase(*i);

  /**
   * Insert the variables that we erased.
   */

  m_vVariables.insert(m_vStack.back().m_vErased);

  /**
   * Insert the values representing the four corners of our assigned
   * component. If the height left is currently zero, then that means
   * we previously removed it, and therefore we must reinsert the
   * template. Otherwise, no insertion/deletion is necessary because
   * the forked instantiations were never inserted in the first place.
   */

  if(c->m_pEmpty->m_nHeightLeft == 0)
    m_vValues.insert(c);
  m_vStack.pop_back();
}

void DynamicEmpty::newCorners(const Component* c,
			   std::vector<EmptyCorner>& v) const {
  v.clear();
  std::vector<Assignment>::const_iterator iEnd(m_vStack.end());
  --iEnd;
  UInt x2(c->x2());
  UInt y2(c->y2());
  for(std::vector<Assignment>::const_iterator i = m_vStack.begin();
      i != iEnd; ++i) {
    
    const Component* c2(i->m_pComponent);
    UInt c2x2(c2->x2());
    UInt c2y2(c2->y2());

    /**
     * We are immediately beneath a previously placed component c2.
     */

    if((UInt) c2->m_nY == y2) {
      if(c2->m_nX < c->m_nX && (UInt) c->m_nX <= c2x2)
	v.push_back(EmptyCorner(c->m_nX - 1, y2 - 1, EmptyCorner::UpperRight));
      if((UInt) c2->m_nX <= x2 && x2 < c2x2)
	v.push_back(EmptyCorner(x2, y2 - 1, EmptyCorner::UpperLeft));
      if((UInt) c->m_nX < c2x2 && c2x2 <= x2)
	v.push_back(EmptyCorner(c2x2 - 1, c2->m_nY, EmptyCorner::LowerRight));
      if((UInt) c->m_nX <= c2x2 && c2x2 < x2)
	v.push_back(EmptyCorner(c2x2, c2->m_nY, EmptyCorner::LowerLeft));
    }

    /**
     * We are immediately above a previously placed component c2. All
     * empty corners have y coordinates of c->m_nY.
     */

    if((UInt) c->m_nY == c2y2) {
      if(c2->m_nX < c->m_nX && (UInt) c->m_nX <= c2x2)
	v.push_back(EmptyCorner(c->m_nX - 1, c->m_nY, EmptyCorner::LowerRight));
      if((UInt) c2->m_nX <= x2 && x2 < c2x2)
	v.push_back(EmptyCorner(x2, c->m_nY, EmptyCorner::LowerLeft));
      if((UInt) c->m_nX < c2x2 && c2x2 <= x2)
	v.push_back(EmptyCorner(c2x2 - 1, c2y2 - 1, EmptyCorner::UpperRight));
      if((UInt) c->m_nX <= c2x2 && c2x2 < x2)
	v.push_back(EmptyCorner(c2x2, c2y2 - 1, EmptyCorner::UpperLeft));
    }

    /**
     * We are immediately left of a previously placed component c2.
     */

    if((UInt) c2->m_nX == x2) {
      if(c2->m_nY < c->m_nY && (UInt) c->m_nY <= c2y2)
	v.push_back(EmptyCorner(x2 - 1, c->m_nY - 1, EmptyCorner::UpperRight));
      if((UInt) c2->m_nY <= y2 && y2 < c2y2)
	v.push_back(EmptyCorner(x2 - 1, y2, EmptyCorner::LowerRight));
      if(c->m_nY < c2->m_nY && (UInt) c2->m_nY <= y2)
	v.push_back(EmptyCorner(c2->m_nX, c2->m_nY - 1, EmptyCorner::UpperLeft));
      if((UInt) c->m_nY <= c2y2 && c2y2 < y2)
	v.push_back(EmptyCorner(c2->m_nX, c2y2, EmptyCorner::LowerLeft));
    }

    /**
     * We are immediately right of a previously placed component c2.
     */

    if((UInt) c->m_nX == c2x2) {
      if(c2->m_nY < c->m_nY && (UInt) c->m_nY <= c2y2)
	v.push_back(EmptyCorner(c->m_nX, c->m_nY - 1, EmptyCorner::UpperLeft));
      if((UInt) c2->m_nY <= y2 && y2 < c2y2)
	v.push_back(EmptyCorner(c->m_nX, y2, EmptyCorner::LowerLeft));
      if(c->m_nY < c2->m_nY && (UInt) c2->m_nY <= y2)
	v.push_back(EmptyCorner(c2x2 - 1, c2->m_nY - 1, EmptyCorner::UpperRight));
      if((UInt) c->m_nY <= c2y2 && c2y2 < y2)
	v.push_back(EmptyCorner(c2x2 - 1, c2y2, EmptyCorner::LowerRight));
    }
  }

  /**
   * Generate empty corners created by one side of the bounding box
   * along with the most recently placed rectangle.
   */

  if(c->m_nX == 0) {
    v.push_back(EmptyCorner(c->m_nX, y2, EmptyCorner::LowerLeft));
    v.push_back(EmptyCorner(c->m_nX, c->m_nY - 1, EmptyCorner::UpperLeft));
  }
  if(x2 == m_pBox->m_nWidth) {
    v.push_back(EmptyCorner(x2 - 1, y2, EmptyCorner::LowerRight));
    v.push_back(EmptyCorner(x2 - 1, c->m_nY - 1, EmptyCorner::UpperRight));
  }
  if(c->m_nY == 0) {
    v.push_back(EmptyCorner(c->m_nX - 1, c->m_nY, EmptyCorner::LowerRight));
    v.push_back(EmptyCorner(x2, c->m_nY, EmptyCorner::LowerLeft));
  }
  if(y2 == m_pBox->m_nHeight) {
    v.push_back(EmptyCorner(c->m_nX - 1, y2 - 1, EmptyCorner::UpperRight));
    v.push_back(EmptyCorner(x2, y2 - 1, EmptyCorner::UpperLeft));
  }
  
  /**
   * Now remove all corners that are either outside of the bounding
   * box or are actually filled by some previously placed rectangle.
   */

  for(Int j = 0; j < (Int) v.size(); ++j) {

    /**
     * Is this inside of the bounding box?
     */

    if(v[j].m_Location.x < 0 || v[j].m_Location.x >= (Int) m_pBox->m_nWidth ||
       v[j].m_Location.y < 0 || v[j].m_Location.y >= (Int) m_pBox->m_nHeight) {
      v[j] = v.back();
      v.pop_back();
      --j;
    }
    else {

      /**
       * The empty corner is in the bounding box. Now make sure no
       * other rectangle has already filled in the empty corner.
       */

      for(std::vector<Assignment>::const_iterator i = m_vStack.begin();
	  i != iEnd; ++i)
	if(i->m_pComponent->fills(v[j])) {
	  v[j] = v.back();
	  v.pop_back();
	  --j;
	  break;
	}
    }
  }
}

bool DynamicEmpty::insideBox(const Dimensions& d,
			  const EmptyCorner& c) const {
  if(c.m_nType == EmptyCorner::LowerLeft ||
     c.m_nType == EmptyCorner::LowerRight)
    return((UInt) c.m_Location.y + d.m_nHeight <= m_pBox->m_nHeight);
  else
    return((UInt) c.m_Location.y >= d.m_nHeight);
}

bool DynamicEmpty::insideBox(const Dimensions& d,
			  const ICoords& c) const {
  return((c.y >= 0) && ((UInt) c.y + d.m_nHeight <= m_pBox->m_nHeight));
}

void DynamicEmpty::push() {
  m_vStackSize.push_back(m_vStack.size());
}

void DynamicEmpty::pop() {
  while(m_vStack.size() > m_vStackSize.back())
    popAux();
  m_vStackSize.pop_back();
}

void DynamicEmpty::pop(Component* c) {
  while(m_vStack.size() > m_vStackSize.back())
    popAux(c);
  m_vStackSize.pop_back();
}

bool DynamicEmpty::placed(Component* c) const {
  for(std::vector<Assignment>::const_iterator i = m_vStack.begin();
      i != m_vStack.end(); ++i)
    if(i->m_pComponent == c) return(true);
  return(false);
}

bool DynamicEmpty::packYCorner(EmptyCorner& ec, Int nDepth) {
  SymmetryMap* pValues = getValues(ec);

  /**
   * See if we can immediately assign all of the items in this
   * corner. This happens when we find an upper left corner, all items
   * are of equal width, and whose height equals the height distance
   * between these corners.
   */

  bool bResult(false);
  if(stackCorner(ec, pValues, nDepth, bResult))
    return(bResult);

  /**
   * We found an empty corner that we can assign a square to. Now look
   * for a value to assign the corner. We iterate over every class of
   * item that can fit here, from largest to smallest. This typically
   * means any empty space components are last, using our compressed
   * representation.
   */

  for(SymmetryMap::reverse_iterator k = pValues->rbegin();
      k != pValues->rend(); ++k) {

    /**
     * No items of this dimension. Let's move on to the next one.
     */

    if(k->second.empty()) continue;

    /**
     * Get the first component of this class.
     */

    Component* c = *(k->second.begin());

    /**
     * If the item is an empty space template, we have to handle
     * branching differently.
     */

    if(c->m_pEmpty) {

      /**
       * Check if the empty corner touches another instantiation of
       * the current empty space template (if so it doesn't matter
       * what empty space height we instantiate the template with --
       * it will always be next to a copy of itself).
       */

      UInt nHeight = c->m_pEmpty->maxNonAdjacentHeight(ec);
      if(nHeight == 0)
	continue;

      /**
       * Get the maximum height subject to the maximum amount of empty
       * space, bounding box height, and non-overlapping constraints
       * with other previously placed rectangles. In the event a
       * maximum height of 0 is allowed, we can move on to the next
       * value to try.
       */

      nHeight = std::min(nHeight, maxHeight(ec, c->m_Dims.m_nWidth));
      if(nHeight == 0)
	continue;

      /**
       * Find the range of subset sums we must explore, to get the
       * possible heights of our template. We use a call to
       * map::upper_bound because it returns the first element that is
       * strictly larger than the input. We are going to iterate over
       * possible y2 values or possible y1 values.
       */

      SimpleSums::const_iterator yBegin, yEnd;
      if(ec.m_nType == EmptyCorner::LowerLeft ||
	 ec.m_nType == EmptyCorner::LowerRight) {
	yBegin = m_ValidY.upper_bound(ec.m_Location.y);
	yEnd = m_ValidY.upper_bound(ec.m_Location.y + nHeight);
      }
      else {
	yBegin = m_ValidY.lower_bound(ec.m_Location.y + 1 - nHeight);
	yEnd = m_ValidY.lower_bound(ec.m_Location.y + 1);
      }

      /**
       * Now that we have the range of valid non-overlapping subset
       * sum values, we simply iterate over the different empty space
       * instantiations.
       */

      for(; yBegin != yEnd; ++yBegin) {
	
	/**
	 * Instantiate the component.
	 */

	Component* pNew(NULL);
	if(ec.m_nType == EmptyCorner::LowerLeft ||
	   ec.m_nType == EmptyCorner::LowerRight)
	  pNew = m_Transform.m_vComponents.forkEmptyX
	    (c, *yBegin - ec.m_Location.y);
	else
	  pNew = m_Transform.m_vComponents.forkEmptyX
	    (c, ec.m_Location.y + 1 - *yBegin);

	pNew->assignY(ec);
	m_pNodes->tick(YF);
	++m_nCorners;
	push();
	pushAux(c, pNew);

	/**
	 * For the recursive call, if the empty template has not been
	 * fully exhausted, we do not increment the depth counter,
	 * because technically we haven't finished using up the
	 * template's entire allowance.
	 */

	if(c->m_pEmpty->m_nHeightLeft == 0) {
	  if(packYAux(nDepth + 1))
	    return(true);
	}
	else {
	  if(packYAux(nDepth))
	    return(true);
	}
	pop(c);
	m_Transform.m_vComponents.unforkEmptyX(c, pNew);
      }
    }
    else {

#if DEBUG

      /**
       * Check to see if this component is actually fixed for a
       * particular rectangle. If so, then we can only place this in
       * the corner only if the corner's x- and y-coordinates is
       * consistent with the rectangle's fixed coordinates.
       */

      if(!m_pParams->m_vFixed.empty()) {
	const Rectangle* r = c->getFirstRect();
	if(r && r->m_bFixed) {
	  if(ec.m_nType == EmptyCorner::LowerLeft ||
	     ec.m_nType == EmptyCorner::LowerRight) {
	    if((Int) r->m_nFixY != ec.m_Location.y)
	      continue;
	  }
	  else {
	    if((Int) (r->m_nFixY + r->m_nHeight) !=
	       ec.m_Location.y)
	      continue;
	  }
	}
      }

#endif // DEBUG

      /**
       * There are some items that must be placed. If this dimension
       * does not fit, we'll have to try the next one. Notice that we
       * can't backtrack because there might be another empty corner
       * that we can put this non-fitting dimension into.
       */

      if(!insideBox(k->first, ec))
	continue;
      c->assignY(ec);

#if 0

      /**
       * Note that we can't safely break top-level symmetry for
       * several reasons. First, if there is only one class of items
       * at x=0, and we pruned here, then we never get to explore our
       * other friends in the same class! Second, due to component
       * gluing, ID 0 might not even be in the top level. It might be
       * a part of some larger component. More complex code handling
       * could possibly fix this but as of right now it's not
       * necessary, as this is not a bottleneck for us.
       */

      if((c->m_nID == 0) &&
	 (c->m_nY > (Int) ((m_pBox->m_nHeight - c->m_Dims.m_nHeight) / 2)))
	continue;
#endif

      /**
       * However, we can break the top-level symmetry with our new
       * code, if we've activated it.
       */

      if(m_pTopSymmetry) {
	if(m_pTopSymmetry == c) {
	  if((UInt) c->m_nY - m_nNewY >
	     (m_NewBox.m_nHeight - c->m_Dims.m_nHeight) / 2)
	    continue;
	}
      }

      m_pNodes->tick(YF);
      ++m_nCorners;

      /**
       * Check overlap constraints.
       */

      if(!canPlace(c))
	continue;

      /**
       * We can place the rectangle here, so let's just make the
       * assignment. We save the value that we're assigning and remove
       * it from the domain, and add this component as assigned.
       */

      push();
      pushAux(c);

      /**
       * Make the recursive call.
       */

      if(packYAux(nDepth + 1))
	return(true);

      pop();
    }
  }
  return(false);
}

void DynamicEmpty::printStack() const {
  std::cout << "Assignment Stack:" << std::endl;
  UInt nMax(0), nStart(0);
  for(std::vector<Assignment>::const_iterator i = m_vStack.begin();
      i != m_vStack.end(); ++i) {
    std::ostringstream oss;
    Printer::print(i->m_vErased, oss);
    nMax = std::max(nMax, (UInt) oss.str().size());
  }
  nStart = nMax + 12;
  for(UInt i = 0; i < m_vStack.size(); ++i) {
    std::ostringstream oss;
    oss << std::setw(2) << i << ": c="
	<< std::setw(2) << m_vStack[i].m_pComponent->m_nID
	<< " e=";
    Printer::print(m_vStack[i].m_vErased, oss);
    while(oss.str().size() < nStart) oss << ' ';
    oss << " i=";
    Printer::print(m_vStack[i].m_vInserted, oss);
    std::cout << oss.str() << std::endl;
  }
  std::cout << std::endl;
}

void DynamicEmpty::print() const {

  /**
   * Assignment stack.
   */

  printStack();

  /**
   * Variables.
   */

  std::cout << "Variables:" << std::endl;
  m_vVariables.print();
  std::cout << std::endl;

  /**
   * Values.
   */

  std::cout << "Values:" << std::endl;
  m_vValues.print();
}

bool DynamicEmpty::canPlace(const Component* c) const {
  for(std::vector<Assignment>::const_iterator i = m_vStack.begin();
      i != m_vStack.end(); ++i)
    if(i->m_pComponent->yOverlaps(*c) &&
       i->m_pComponent->xOverlaps(*c)) return(false);
  return(true);
}

void DynamicEmpty::get(Placements& v) const {
  ComponentPtrs::const_iterator j = m_Transform.m_vComponents.begin();
  for(Placements::iterator i = v.begin(); i != v.end(); ++i, ++j)
    i->m_nLocation.y = (URational) (*j)->m_nY;
}

void DynamicEmpty::get(IntPlacements& v) const {
  ComponentPtrs::const_iterator j = m_Transform.m_vComponents.begin();
  for(IntPlacements::iterator i = v.begin(); i != v.end(); ++i, ++j)
    i->m_nY = (UInt) (*j)->m_nY;
}

UInt DynamicEmpty::maxHeight(const EmptyCorner& ec,
			  const UInt& nWidth) const {
  UInt nHeight(0);
  if(ec.m_nType == EmptyCorner::LowerLeft ||
     ec.m_nType == EmptyCorner::LowerRight)
    nHeight = m_pBox->m_nHeight - ec.m_Location.y;
  else
    nHeight = m_pBox->m_nHeight;

  /**
   * Iterate over every iteme in our stack, constraining the height.
   */

  for(std::vector<Assignment>::const_iterator i = m_vStack.begin();
      i != m_vStack.end(); ++i)
    ec.constrainHeight(nWidth, nHeight, i->m_pComponent);
  return(nHeight);
}

void DynamicEmpty::placeDominated(Int& nStartDepth) {
  EmptyCorner ec(0, 0, EmptyCorner::LowerLeft);
  SymmetryMap* pValues = getValues(ec);
  m_vDominated.clear();
  for(SymmetryMap::reverse_iterator k = pValues->rbegin();
      k != pValues->rend(); ++k) {
    if(k->second.empty() ||
       k->first.m_nWidth != m_pBox->m_nWidth) continue;
    for(std::set<Component*>::iterator i = k->second.begin();
	i != k->second.end(); ++i) {
      if((*i)->m_pEmpty)
	continue;
      m_vDominated.push_back(*i);
    }
  }
  
  for(std::deque<Component*>::iterator i = m_vDominated.begin();
      i != m_vDominated.end(); ++i) {
    Component* c = *i;
    c->assignY(ec);
    m_pNodes->tick(YF);
    ++m_nCorners;
    push();
    pushAux(c);
    ++nStartDepth;
    ec.m_Location.y += c->m_Dims.m_nHeight;
  }

  m_nNewY = ec.m_Location.y;
  m_NewBox.m_nHeight = m_pBox->m_nHeight - m_nNewY;
}

void DynamicEmpty::findTopSymmetry() {
  m_pTopSymmetry = NULL;
  EmptyCorner ec(0, 0, EmptyCorner::LowerLeft);
  SymmetryMap* pValues = getValues(ec);
  if(pValues->empty())
    return;

  /**
   * If there are multiple items of this class, then the code needs
   * to be more complicated. For now we simply can break top-level
   * symmetry only if there is exactly one element of this class,
   * and it's not empty.
     */
  
  SymmetryMap::reverse_iterator i = pValues->rbegin();
  while(i->second.empty() && i != pValues->rend())
    ++i;
  if(i == pValues->rend() || i->second.size() != 1)
    return;
  Component* c = *i->second.begin();
  if(c->m_pEmpty == NULL)
    m_pTopSymmetry = c;
}

void DynamicEmpty::computeValidY() {

  /**
   * Start with our computed y-offset, which is the result of placing
   * rectangles that span the width of the bounding box.
   */

  m_ValidY.clear();
  m_ValidY.insert(m_nNewY);

  /**
   * Next add in the subset sums due to all of the rectangles we're
   * supposed to be packing.
   */

  for(ComponentPtrs::const_iterator i = m_Transform.m_vComponents.begin();
      i != m_Transform.m_vComponents.end(); ++i)
    if((*i)->m_bTopLevel &&
       !(*i)->m_bEmpty &&
       (*i)->m_pEmpty == NULL &&
       (*i)->m_Dims.m_nWidth != m_pBox->m_nWidth)
      m_ValidY.add((*i)->m_Dims.m_nHeight, m_pBox->m_nHeight);
      
  /**
   * Now add in the dominated positions of placing a rectangle against
   * the top wall. This is necessary so that we can place rectangles
   * in all four corners and prune horizontal empty strips due to not
   * having enough to match exactly a subset number.
   */

  for(ComponentPtrs::const_iterator i = m_Transform.m_vComponents.begin();
      i != m_Transform.m_vComponents.end(); ++i)
    if((*i)->m_bTopLevel &&
       !(*i)->m_bEmpty &&
       (*i)->m_pEmpty == NULL &&
       (*i)->m_Dims.m_nWidth != m_pBox->m_nWidth)
      m_ValidY.insert(m_pBox->m_nHeight - (*i)->m_Dims.m_nHeight);
}

bool DynamicEmpty::stackCorner(EmptyCorner& ec, SymmetryMap* pValues,
			       Int nDepth, bool& bResult) {
  
  /**
   * Are all eligible widths equal, and does the combined component
   * fit?
   */

  EmptyCorner other;
  switch(ec.m_nType) {
  case EmptyCorner::LowerLeft:
    break;
  case EmptyCorner::UpperLeft:
    break;
  case EmptyCorner::LowerRight:
    break;
  case EmptyCorner::UpperRight:
    break;
  };
  return(false);
}
