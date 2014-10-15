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

#include "Globals.h"
#include "Perfect.h"
#include "Printer.h"
#include "BoxDimensions.h"
#include "TimeSpec.h"
#include "Grid.h"
#include "NodeCount.h"
#include <iomanip>

Perfect::Perfect() :
  m_pCuSP(NULL),
  m_pBox(NULL),
  m_pTime(NULL),
  m_pGrid(NULL),
  m_pNodes(NULL),
  m_nDepth(0),
  m_nAdjacency(0),
  m_nCorners(0),
  m_bTopLevelDomination(true),
  m_bFilterDomainWithGrid(true),
  m_bAllFourCorners(true) {
}

Perfect::~Perfect() {
}

void Perfect::initialize(const std::string& sParams) {
  if(sParams.find_first_of('1') < sParams.size())
    m_bAllFourCorners = false;
  if(sParams.find_first_of('a') < sParams.size())
    m_Inferences.m_bAdjacency = false;
  if(sParams.find_first_of('d') < sParams.size())
    m_Inferences.m_bDistance = false;
  if(sParams.find_first_of('c') < sParams.size())
    m_Inferences.m_bComposition = false;
  if(sParams.find_first_of('e') < sParams.size())
    m_Inferences.m_bEmptyComposition = false;
  if(sParams.find_first_of('s') < sParams.size())
    m_Inferences.m_bSubsetSums = false;
}

void Perfect::initialize(UInt* pCuSP, BoxDimensions* pBox,
			 TimeSpec* pTime, Grid* pGrid,
			 NodeCount* pNodes) {
  m_pCuSP = pCuSP;
  m_pBox = pBox;
  m_pTime = pTime;
  m_pGrid = pGrid;
  m_pNodes = pNodes;
  m_Inferences.clear();
}

void Perfect::initWastedSpace() {

  /**
   * Set up the wasted space histograms and place the first item for
   * free.
   */

  m_vWastedXF.clear();
  m_vWastedXF.resize(std::min(m_pBox->m_nWidth, m_pBox->m_nHeight) + 1, 0);
  m_vWastedXF.back() = m_pBox->m_nArea;
}

bool Perfect::packX(Bins* pBins, RectPtrArray::iterator iBegin,
		    const RectPtrArray::const_iterator& iEnd) {
  TimeSpec t;
  t.tick();
  ++(*m_pCuSP);

  /**
   * Assign any missing y values
   */
  
  for(RectPtrArray::iterator i = iBegin; i != iEnd; ++i)
    (*i)->y = pBins->m_vDomain[(*i)->m_nID].m_nBegin;

  /**
   * Apply bottom-up computation to derive constraints.
   */

  //m_Inferences.m_bSubsetSums = true;
  //m_Inferences.m_bComposition = true;
  m_Inferences.initialize(*pBins, iBegin, iEnd);
  m_Inferences.myb = m_pBox;
  m_Inferences.compute();
  m_Inferences.emptyToComponents();
  while(m_Inferences.computeNonEmpty());
  m_Inferences.findSymmetries();
  m_Inferences.updateComponentAreas();
  m_Inferences.updateAdjacency(*m_pBox);
  m_Inferences.updateDomination(*m_pBox);

  /**
   * Calculate the number of components we have to combinatorially
   * explore.
   */

  m_nDepth = 0;
  for(UInt i = 0; i < m_Inferences.m_vComponents.size(); ++i)
    if(m_Inferences.m_vComponents[i]->m_bTopLevel) ++m_nDepth;
  m_vStackSize.clear(); m_vStackSize.reserve(m_nDepth);
  m_vStack.clear(); m_vStack.reserve(m_nDepth);

  /**
   * Size our data arrays and set up the initial bounding box corner.
   */

  m_vVariables.initialize(*m_pBox);
  m_vValues.initialize(m_Inferences.m_TopSymmetries,
		       m_Inferences.m_BottomSymmetries);

  /**
   * Allocate more wasted spaces arrays if necessary.
   */

  m_vWastedX.resize(m_Inferences.m_vComponents.size());
  m_vPlaced.clear();
  m_vPlaced.resize(m_Inferences.m_vComponents.size(), false);

  /**
   * Enforce any top-level domination conditions.
   */

  WastedBins* pFirstWasted(&m_vWastedXF);
  if(m_bTopLevelDomination = !m_Inferences.m_lDomination.empty()) {
    UInt nBestArea(0);
    std::pair<UInt, UInt> best;
    for(std::list<std::pair<UInt, UInt> >::iterator i =
	  m_Inferences.m_lDomination.begin();
	i != m_Inferences.m_lDomination.end(); ++i) {
      if(i->first == i->second) {
	if(m_Inferences.m_vComponents[i->first]->m_Dims.m_nArea > nBestArea) {
	  nBestArea = m_Inferences.m_vComponents[i->first]->m_Dims.m_nArea;
	  best = *i;
	}
      }
      else {
	UInt nArea = 
	  m_Inferences.m_vComponents[i->first]->m_Dims.m_nArea +
	  m_Inferences.m_vComponents[i->second]->m_Dims.m_nArea;
	if(nArea > nBestArea) {
	  nBestArea = nArea;
	  best = *i;
	}
      }
    }
    
    if(best.first == best.second) {
      forceDominationLeft(best.first, m_vWastedXF);
      pFirstWasted = &m_vWastedX[best.first];
    }
    else {
      forceDominationLeft(best.first, best.second, m_vWastedXF);
      pFirstWasted = &m_vWastedX[best.second];
    }
  }

  /**
   * Run the search.
   */

  bool bResult(packXAux(0, *pFirstWasted));
  t.tock();
  (*m_pTime) += t;
  if(bResult) return(true);
  else {
    while(!m_vStack.empty())
      pop();
    return(false);
  }
}

bool Perfect::packXAux(Int nDepth, const WastedBins& v) {

  if(bQuit) return(false);

  /**
   * Base case is when we've got
   */

  if(nDepth == m_nDepth) return(true);
  
  /**
   * Compute the domain sizes in the model of empty corners.
   */

  UInt nSize1;
  EmptyCorner ec;
  if(m_bFilterDomainWithGrid)
    smallestDomain3(nSize1, ec, m_pGrid);
  else
    smallestDomain2(nSize1, ec);
  if(nSize1 == 0) return(false);

  if(m_Inferences.m_bAdjacency && m_bAllFourCorners) {

    /**
     * Also compute the domain sizes in the model of contiguous
     * placements.
     */

    UInt nSize2(nSize1 + 1);
    std::list<ICoords> l;
    Component* pForced(NULL);
    adjacencySize(nSize2, l, pForced);
    if(nSize2 == 0) return(false);
    if(nSize1 <= nSize2)
      return(packXCorner(ec, nDepth, v));
    else
      return(packXAdjacency(pForced, l, nDepth, v));
  }
  else
    return(packXCorner(ec, nDepth, v));
  return(false); // Control reaches non-void function warning?
}

bool Perfect::packXAdjacency(Component* c,
			     const std::list<ICoords>& l,
			     Int nDepth, const WastedBins& v) {
  for(std::list<ICoords>::const_iterator i = l.begin();
      i != l.end(); ++i) {
    c->m_nX = i->x;
    if(!m_bTopLevelDomination && (c->m_nID == 0) &&
       (c->m_nX > (Int) ((m_pBox->m_nWidth - c->m_Dims.m_nWidth) / 2))) continue;
    m_pNodes->tick(XF);
    ++m_nAdjacency;
    push();
    pushAux(c, v);
    if(m_vWastedX[c->m_nID].canFit(m_Inferences.m_vComponents, m_vPlaced))
      if(packXAux(nDepth + 1, m_vWastedX[c->m_nID]))
	return(true);
    pop();
  }
  return(false);
}

void Perfect::smallestDomain2(UInt& nSmallest, EmptyCorner& c) {
  nSmallest = std::numeric_limits<UInt>::max();
  UInt nComponents(std::numeric_limits<UInt>::max());
  smallestDomain6(nSmallest, nComponents,
		  m_vVariables[CornerVariables::UpperLeft],
		  m_vValues[CornerValues::Above], c, EmptyCorner::UpperLeft);
  if(m_bAllFourCorners) {
    if(nSmallest == 0) return;
    smallestDomain6(nSmallest, nComponents,
		    m_vVariables[CornerVariables::LowerLeft],
		    m_vValues[CornerValues::Below], c, EmptyCorner::LowerLeft);
    if(nSmallest == 0) return;
    smallestDomain6(nSmallest, nComponents,
		    m_vVariables[CornerVariables::LowerRight],
		    m_vValues[CornerValues::Below], c, EmptyCorner::LowerRight);
    if(nSmallest == 0) return;
    smallestDomain6(nSmallest, nComponents,
		    m_vVariables[CornerVariables::UpperRight],
		    m_vValues[CornerValues::Above], c, EmptyCorner::UpperRight);
  }
}

void Perfect::smallestDomain6(UInt& nSize, UInt& nComponents,
			      std::map<UInt, std::set<Int> >& vars,
			      AlignedCongruencies& vals,
			      EmptyCorner& c,
			      EmptyCorner::Corner nType) {
  for(std::map<UInt, std::set<Int> >::iterator y = vars.begin();
      y != vars.end(); ++y)
    if(!y->second.empty()) {
      UInt p = vals[y->first].symPermutations(m_BCache);
      if(p == 0) {
	nSize = 0;
	return;
      }
      if(p < nSize) {
	nSize = p;
	nComponents = vals[y->first].components();
	c.m_Location.x = *y->second.begin();
	c.m_Location.y = y->first;
	c.m_nType = nType;
      }
      else if(p == nSize) {
	UInt nc = vals[y->first].components();
	if(nc < nComponents) {
	  nComponents = nc;
	  c.m_Location.x = *y->second.begin();
	  c.m_Location.y = y->first;
	  c.m_nType = nType;
	}
      }
    }
}

void Perfect::smallestDomain3(UInt& nSmallest, EmptyCorner& c,
			      const Grid* pGrid) {
  nSmallest = std::numeric_limits<UInt>::max();
  UInt nComponents(std::numeric_limits<UInt>::max());
  smallestDomain7(nSmallest, nComponents,
		  m_vVariables[CornerVariables::UpperLeft],
		  m_vValues[CornerValues::Above], c,
		  EmptyCorner::UpperLeft, pGrid);
  if(m_bAllFourCorners) {
    if(nSmallest == 0) return;
    smallestDomain7(nSmallest, nComponents,
		    m_vVariables[CornerVariables::LowerLeft],
		    m_vValues[CornerValues::Below], c,
		    EmptyCorner::LowerLeft, pGrid);
    if(nSmallest == 0) return;
    smallestDomain7(nSmallest, nComponents,
		    m_vVariables[CornerVariables::LowerRight],
		    m_vValues[CornerValues::Below], c,
		    EmptyCorner::LowerRight, pGrid);
    if(nSmallest == 0) return;
    smallestDomain7(nSmallest, nComponents,
		    m_vVariables[CornerVariables::UpperRight],
		    m_vValues[CornerValues::Above], c,
		    EmptyCorner::UpperRight, pGrid);
  }
}

void Perfect::smallestDomain7(UInt& nSize, UInt& nComponents,
			      std::map<UInt, std::set<Int> >& vars,
			      AlignedCongruencies& vals,
			      EmptyCorner& c,
			      EmptyCorner::Corner nType,
			      const Grid* pGrid) {
  for(std::map<UInt, std::set<Int> >::iterator y = vars.begin();
      y != vars.end(); ++y)
    for(std::set<Int>::const_iterator i = y->second.begin();
	i != y->second.end(); ++i) {
      UInt p = vals[y->first].symPermutations(m_BCache,
					      pGrid->m_vHor[*i][y->first],
					      pGrid->m_vVer[*i][y->first]);
      if(p == 0) {
	nSize = 0;
	return;
      }
      if(p < nSize) {
	nSize = p;
	nComponents = vals[y->first].components(pGrid->m_vHor[*i][y->first],
						pGrid->m_vVer[*i][y->first]);
	c.m_Location.x = *i;
	c.m_Location.y = y->first;
	c.m_nType = nType;
      }
      else if(p == nSize) {
	UInt nc = vals[y->first].components(pGrid->m_vHor[*i][y->first],
					    pGrid->m_vVer[*i][y->first]);
	if(nc < nComponents) {
	  nComponents = nc;
	  c.m_Location.x = *i;
	  c.m_Location.y = y->first;
	  c.m_nType = nType;
	}
      }
    }
}

SymmetryMap* Perfect::getValues(const EmptyCorner& ec) {
  switch(ec.m_nType) {
  case EmptyCorner::UpperLeft:
    return(&m_vValues[CornerValues::Above][ec.m_Location.y]);
  case EmptyCorner::LowerLeft:
    return(&m_vValues[CornerValues::Below][ec.m_Location.y]);
  case EmptyCorner::UpperRight:
    return(&m_vValues[CornerValues::Above][ec.m_Location.y]);
  case EmptyCorner::LowerRight:
    return(&m_vValues[CornerValues::Below][ec.m_Location.y]);
  default:
    return(NULL);
  }
}

void Perfect::pushAux(Component* c, const WastedBins& v) {
  
  /**
   * Remove all empty corners that have been filled by this component
   * (carefully putting them into our undo stack).
   */

  m_vWastedX[c->m_nID] = v;
  m_pGrid->add(c, m_vWastedX[c->m_nID]);
  m_vPlaced[c->m_nID] = true;
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

void Perfect::popAux() {

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
  m_pGrid->del(m_vStack.back().m_pComponent);
  m_vPlaced[m_vStack.back().m_pComponent->m_nID] = false;
  m_vStack.pop_back();
}

void Perfect::newCorners(const Component* c,
			 std::vector<EmptyCorner>& v) const {
  if(c->m_nY > 0)
    newCornersA(c, v);
  if(c->m_nY + c->m_Dims.m_nHeight < (UInt) m_pBox->m_nHeight)
    newCornersB(c, v);
  if(c->m_nX > 0)
    newCornersL(c, v);
  if(c->m_nX + c->m_Dims.m_nWidth < (UInt) m_pBox->m_nWidth)
    newCornersR(c, v);
}

void Perfect::newCornersA(const Component* c, std::vector<EmptyCorner>& v) const {
  Int x1 = c->m_nX - 1;
  Int y1 = c->m_nY - 1;
  bool bPreviousEmpty(false), bCurrentEmpty;

  /**
   * Check the upper left cell.
   */
  
  UInt x(c->m_nX);
  if(c->m_nX > 0) {
    bPreviousEmpty = m_pGrid->empty(x1, y1);
    bCurrentEmpty = m_pGrid->empty(x, y1);
    if(bPreviousEmpty != bCurrentEmpty && bCurrentEmpty)
      v.push_back(EmptyCorner(x, y1, EmptyCorner::LowerLeft));
    bPreviousEmpty = bCurrentEmpty;
    ++x;
  }

  /**
   * Scan across to the right.
   */

  for(; x < c->m_nX + c->m_Dims.m_nWidth; ++x) {
    bCurrentEmpty = m_pGrid->empty(x, y1);
    if(bPreviousEmpty == bCurrentEmpty) continue;
    
    /**
     * Check which type of corner we've got: either lower left or
     * lower right.
     */
    
    if(bCurrentEmpty)
      v.push_back(EmptyCorner(x, y1,
			      EmptyCorner::LowerLeft));
    else
      v.push_back(EmptyCorner(x - 1, y1,
			      EmptyCorner::LowerRight));
    bPreviousEmpty = bCurrentEmpty;
  }
  
  /**
   * Boundary condition: check the upper right cell.
   */
  
  bCurrentEmpty = (x < (UInt) m_pBox->m_nWidth ? m_pGrid->empty(x, y1) : false);
  if(bPreviousEmpty != bCurrentEmpty && !bCurrentEmpty)
    v.push_back(EmptyCorner(x - 1, y1,
			    EmptyCorner::LowerRight));
}

void Perfect::newCornersB(const Component* c, std::vector<EmptyCorner>& v) const {
  Int x1 = c->m_nX - 1;
  Int yn = c->m_nY + c->m_Dims.m_nHeight;
  bool bPreviousEmpty(false), bCurrentEmpty;

  /**
   * Check the lower left cell.
   */
  
  Int x(c->m_nX);
  if(c->m_nX > 0) {
    bPreviousEmpty = m_pGrid->empty(x1, yn);
    bCurrentEmpty = m_pGrid->empty(x, yn);
    if(bPreviousEmpty != bCurrentEmpty && bCurrentEmpty)
      v.push_back(EmptyCorner(x, yn, EmptyCorner::UpperLeft));
    bPreviousEmpty = bCurrentEmpty;
    ++x;
  }

  /**
   * Scan across to the right.
   */

  for(; x < (Int) (c->m_nX + c->m_Dims.m_nWidth); ++x) {
    bCurrentEmpty = m_pGrid->empty(x, yn);
    if(bPreviousEmpty == bCurrentEmpty) continue;
    
    /**
     * Check which type of corner we've got: either lower left or
     * lower right.
     */
    
    if(bCurrentEmpty)
      v.push_back(EmptyCorner(x, yn, EmptyCorner::UpperLeft));
    else
      v.push_back(EmptyCorner(x - 1, yn, EmptyCorner::UpperRight));
    bPreviousEmpty = bCurrentEmpty;
  }
  
  /**
   * Boundary condition: check the lower right cell. It's either going
   * to be an empty border, or whatever is located there.
   */
  
  bCurrentEmpty = (x < (Int) m_pBox->m_nWidth ? m_pGrid->empty(x, yn) : false);
  if(bPreviousEmpty != bCurrentEmpty && !bCurrentEmpty)
    v.push_back(EmptyCorner(x - 1, yn, EmptyCorner::UpperRight));
}

void Perfect::newCornersL(const Component* c, std::vector<EmptyCorner>& v) const {
  Int y1 = c->m_nY - 1;
  Int x1 = c->m_nX - 1;
  bool bPreviousEmpty(false), bCurrentEmpty;

  /**
   * Check the upper left cell.
   */

  Int y(c->m_nY);
  if(c->m_nY > 0) {
    bPreviousEmpty = m_pGrid->empty(x1, y1);
    bCurrentEmpty = m_pGrid->empty(x1, y);
    if(bPreviousEmpty != bCurrentEmpty && bCurrentEmpty)
      v.push_back(EmptyCorner(x1, y, EmptyCorner::UpperRight));
    bPreviousEmpty = bCurrentEmpty;
    ++y;
  }

  /**
   * Scan down.
   */

  for(; y < (Int) (c->m_nY + c->m_Dims.m_nHeight); ++y) {
    bCurrentEmpty = m_pGrid->empty(x1, y);
    if(bPreviousEmpty == bCurrentEmpty) continue;
    
    /**
     * Check which type of corner we've got: either upper right or
     * lower right.
     */
    
    if(bCurrentEmpty)
      v.push_back(EmptyCorner(x1, y, EmptyCorner::UpperRight));
    else
      v.push_back(EmptyCorner(x1, y - 1, EmptyCorner::LowerRight));
    bPreviousEmpty = bCurrentEmpty;
  }

  /**
   * Boundary condition: check the lower left cell.
   */
    
  bCurrentEmpty = (y < (Int) m_pBox->m_nHeight ? m_pGrid->empty(x1, y) : false);
  if(bPreviousEmpty != bCurrentEmpty && !bCurrentEmpty)
    v.push_back(EmptyCorner(x1, y - 1, EmptyCorner::LowerRight));
}

void Perfect::newCornersR(const Component* c, std::vector<EmptyCorner>& v) const {
  Int y1 = c->m_nY - 1;
  Int xn = c->m_nX + c->m_Dims.m_nWidth;
  bool bPreviousEmpty(false), bCurrentEmpty;

  /**
   * Check the upper right cell.
   */

  Int y(c->m_nY);
  if(c->m_nY > 0) {
    bPreviousEmpty = m_pGrid->empty(xn, y1);
    bCurrentEmpty = m_pGrid->empty(xn, y);
    if(bPreviousEmpty != bCurrentEmpty && bCurrentEmpty)
      v.push_back(EmptyCorner(xn, y, EmptyCorner::UpperLeft));
    bPreviousEmpty = bCurrentEmpty;
    ++y;
  }

  /**
   * Scan down.
   */

  for(; y < (Int) (c->m_nY + c->m_Dims.m_nHeight); ++y) {
    bCurrentEmpty = m_pGrid->empty(xn, y);
    if(bPreviousEmpty == bCurrentEmpty) continue;

    /**
     * Check which type of corner we've got: either upper left or
     * lower left.
     */

    if(bCurrentEmpty)
      v.push_back(EmptyCorner(xn, y, EmptyCorner::UpperLeft));
    else
      v.push_back(EmptyCorner(xn, y - 1, EmptyCorner::LowerLeft));
    bPreviousEmpty = bCurrentEmpty;
  }

  /**
   * Boundary condition: check the lower right cell.
   */
    
  bCurrentEmpty = (y < (Int) m_pBox->m_nHeight ? m_pGrid->empty(xn, y) : false);
  if(bPreviousEmpty != bCurrentEmpty && !bCurrentEmpty)
    v.push_back(EmptyCorner(xn, y - 1, EmptyCorner::LowerLeft));
}

bool Perfect::insideBox(const Dimensions& d,
			const EmptyCorner& c) const {
  if(c.m_nType == EmptyCorner::UpperLeft ||
     c.m_nType == EmptyCorner::LowerLeft)
    return((UInt) c.m_Location.x + d.m_nWidth <= m_pBox->m_nWidth);
  else
    return((UInt) c.m_Location.x >= d.m_nWidth);
}

bool Perfect::insideBox(const Dimensions& d,
			const ICoords& c) const {
  return((c.x >= 0) && ((UInt) c.x + d.m_nWidth <= m_pBox->m_nWidth));
}

void Perfect::push() {
  m_vStackSize.push_back(m_vStack.size());
}

void Perfect::pop() {
  while(m_vStack.size() > m_vStackSize.back())
    popAux();
  m_vStackSize.pop_back();
}

void Perfect::adjacencySize(UInt& nSize,
			    std::list<ICoords>& l,
			    Component*& c) {
  nSize = std::numeric_limits<UInt>::max();
  Inferences::AdjacencyTraits::edge_iterator ei, ei_end;
  for(boost::tie(ei, ei_end) = boost::edges(m_Inferences.m_X);
      ei != ei_end; ++ei) {
    Component* c1(m_Inferences.m_vComponents[boost::source(*ei, m_Inferences.m_X)]);
    Component* c2(m_Inferences.m_vComponents[boost::target(*ei, m_Inferences.m_X)]);
    bool bPlaced1(placed(c1));
    bool bPlaced2(placed(c2));

    /**
     * If they're both unplaced, we should simply continue.
     */

    if(!bPlaced1 && !bPlaced2) continue;

    /**
     * If they're both placed, and they're not adjacent, then we
     * should prune so we return a size 0 domain.
     */

    if(bPlaced1 && bPlaced2) {
      if(((Int) (c1->m_nX + c1->m_Dims.m_nWidth) == c2->m_nX) ||
	 ((Int) (c2->m_nX + c2->m_Dims.m_nWidth) == c1->m_nX))
	continue;
      else {
	nSize = 0;
	return;
      }
    }

    /**
     * One is placed and the other is unplaced.
     */

    std::list<ICoords> l2;
    if(bPlaced1) adjacencySize(l2, c1, c2);
    else adjacencySize(l2, c2, c1);
    if(l2.empty()) {
      l.clear();
      nSize = 0;
      return;
    }
    if(l2.size() < nSize) {
      nSize = l2.size();
      l = l2;
      c = bPlaced1 ? c2 : c1;
    }
  }
}

void Perfect::adjacencySize(std::list<ICoords>& ls,
			    const Component* pPlaced,
			    const Component* pUnplaced) const {
  ICoords c;
  c.y = pUnplaced->m_nY;
  c.x = pPlaced->m_nX - pUnplaced->m_Dims.m_nWidth;
  if(insideBox(pUnplaced->m_Dims, c) &&
     !m_pGrid->occupied(c, pUnplaced->m_Dims))
    ls.push_back(c);
  c.x = pPlaced->m_nX + pPlaced->m_Dims.m_nWidth;
  if(insideBox(pUnplaced->m_Dims, c) &&
     !m_pGrid->occupied(pPlaced->m_nX + pPlaced->m_Dims.m_nWidth,
			c.y, pUnplaced->m_Dims))
    ls.push_back(c);
}

bool Perfect::placed(Component* c) const {
  for(std::vector<Assignment>::const_iterator i = m_vStack.begin();
      i != m_vStack.end(); ++i)
    if(i->m_pComponent == c) return(true);
  return(false);
}

bool Perfect::packXCorner(EmptyCorner& ec, Int nDepth,
			  const WastedBins& v) {
  SymmetryMap* pValues = getValues(ec);

  /**
   * We found an empty corner that we can assign a square to. Now look
   * for a value to assign the corner.
   */

  for(SymmetryMap::reverse_iterator k = pValues->rbegin();
      k != pValues->rend(); ++k) {
    if(k->second.empty()) continue;
    Component* c = *(k->second.begin());
    if(!insideBox(c->m_Dims, ec)) continue;
    c->assignX(ec);
    if(!m_bTopLevelDomination && (c->m_nID == 0) &&
       (c->m_nX > (Int) ((m_pBox->m_nWidth - c->m_Dims.m_nWidth) / 2))) continue;

    m_pNodes->tick(XF);
    ++m_nCorners;
    if(m_pGrid->occupied(c)) continue;

    /**
     * We can place the square here, so let's just make the
     * assignment. We save the value that we're assigning and
     * remove it from the domain, and add this component as
     * assigned.
     */

    push();
    pushAux(c, v);

    /**
     * Make the recursive call.
     */

    if(m_vWastedX[c->m_nID].canFit(m_Inferences.m_vComponents, m_vPlaced))
      if(packXAux(nDepth + 1, m_vWastedX[c->m_nID]))
	return(true);

    /**
     * Undo our modifications.
     */

    pop();
  }
  return(false);
}

void Perfect::forceDominationLeft(UInt c1, UInt c2,
				  const WastedBins& v) {
  push();
  m_Inferences.m_vComponents[c1]->m_nX = 0;
  m_Inferences.m_vComponents[c2]->m_nX = 0;
  pushAux(m_Inferences.m_vComponents[c1], v);
  pushAux(m_Inferences.m_vComponents[c2], m_vWastedX[c1]);
  m_pNodes->tick(XF, 2);
  m_nDepth -= 2;
}

void Perfect::forceDominationLeft(UInt c,
				  const WastedBins& v) {
  push();
  m_Inferences.m_vComponents[c]->m_nX = 0;
  m_pNodes->tick(XF);
  pushAux(m_Inferences.m_vComponents[c], v);
  --m_nDepth;
}

void Perfect::forceDominationRight(UInt c1, UInt c2,
				   const WastedBins& v) {
  push();
  m_Inferences.m_vComponents[c1]->m_nX =
    m_pBox->m_nWidth - m_Inferences.m_vComponents[c1]->m_Dims.m_nWidth;
  m_Inferences.m_vComponents[c2]->m_nX =
    m_pBox->m_nWidth - m_Inferences.m_vComponents[c2]->m_Dims.m_nWidth;
  pushAux(m_Inferences.m_vComponents[c1], v);
  pushAux(m_Inferences.m_vComponents[c2], m_vWastedX[c1]);
  m_pNodes->tick(XF, 2);
  m_nDepth -= 2;
}

void Perfect::print() const {

  /**
   * Assignment stack.
   */

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
