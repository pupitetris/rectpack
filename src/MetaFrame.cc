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

#include "MetaFrame.h"
#include "Rectangle.h"
#include <limits>

MetaFrame::MetaFrame() {
}

MetaFrame::MetaFrame(const MetaFrame& src) :
  m_Assigned(src.m_Assigned),
  m_Unassigned(src.m_Unassigned),
  m_vXMatrix(src.m_vXMatrix),
  m_vYMatrix(src.m_vYMatrix),
  m_nMinWidth(0),
  m_nMinHeight(0),
  m_nMaxWidth(std::numeric_limits<UInt>::max()),
  m_nMaxHeight(std::numeric_limits<UInt>::max()) {
}

MetaFrame::~MetaFrame() {
}

void MetaFrame::initialize(const std::vector<std::vector<MetaVarDesc> >& vVars) {
  m_vXMatrix.initialize(vVars.size());
  m_vYMatrix.initialize(vVars.size());
  for(size_t i = 0; i < vVars.size(); ++i)
    for(size_t j = 0; j < i; ++j)
      m_Unassigned[&vVars[i][j]].initialize(&vVars[i][j]);
}

MetaVariable* MetaFrame::assign(const MetaVarDesc* pDesc,
				MetaDomain::ValueT nValue) {
  m_Assigned[pDesc] = m_Unassigned[pDesc];
  m_Assigned[pDesc].assign(nValue);
  m_Unassigned.erase(pDesc);
  return(&m_Assigned[pDesc]);
}

bool MetaFrame::forwardChecking() {
  for(std::map<const MetaVarDesc*, MetaVariable>::iterator i =
	m_Unassigned.begin(); i != m_Unassigned.end(); ++i) {
    const Rectangle* r1(i->first->m_pRect1);
    const Rectangle* r2(i->first->m_pRect2);
    if(m_vXMatrix[r1->m_nID][r2->m_nID] < (Int) r1->m_nWidth)
      i->second.m_Domain.erase(MetaDomain::LEFTOF);
    if(m_vYMatrix[r1->m_nID][r2->m_nID] < (Int) r1->m_nHeight)
      i->second.m_Domain.erase(MetaDomain::BELOW);
    if(m_vXMatrix[r2->m_nID][r1->m_nID] < (Int) r2->m_nWidth)
      i->second.m_Domain.erase(MetaDomain::RIGHTOF);
    if(m_vYMatrix[r2->m_nID][r1->m_nID] < (Int) r2->m_nHeight)
      i->second.m_Domain.erase(MetaDomain::ABOVE);
    if(i->second.m_Domain.empty())
      return(false);
  }
  return(true);
}

bool MetaFrame::forwardCheckBounds(const Solution& s) {
  if(s.empty())
    return(true);

  /**
   * We have a solution to compare against. Compute the induced width
   * and height of the bounding box.
   */

  BoxDimensions b;
  b.initialize(0);
  for(ConstVarIter i = m_Assigned.begin(); i != m_Assigned.end();
      ++i) {
    const Rectangle* r1(i->first->m_pRect1);
    const Rectangle* r2(i->first->m_pRect2);
    switch(i->second.m_nValue) {
    case MetaDomain::LEFTOF:
      b.m_nWidth =
	std::max(b.m_nWidth,
		 (UInt) (- m_vXMatrix[r1->m_nID][r2->m_nID] +
			 r2->m_nWidth));
      break;
    case MetaDomain::RIGHTOF:
      b.m_nWidth =
	std::max(b.m_nWidth,
		 (UInt) (- m_vXMatrix[r2->m_nID][r1->m_nID] +
			 r1->m_nWidth));
      break;
    case MetaDomain::BELOW:
      b.m_nHeight =
	std::max(b.m_nHeight,
		 (UInt) (- m_vYMatrix[r1->m_nID][r2->m_nID] +
			 r2->m_nHeight));
      break;
    case MetaDomain::ABOVE:
      b.m_nHeight =
	std::max(b.m_nHeight,
		 (UInt) (- m_vYMatrix[r2->m_nID][r1->m_nID] +
			 r1->m_nHeight));
      break;
    default:
      break;
    };
  }
  b.m_nArea = b.m_nWidth * b.m_nHeight;
  if(s.m_nArea < b.m_nArea)
    return(false);
  else if(s.m_nArea == b.m_nArea &&
	  s.contains(b))
    return(false);

  /**
   * Perform forward checking by removing any domain values that may
   * cause us to violate constraints.
   */

  for(VarIter i = m_Unassigned.begin(); i != m_Unassigned.end();
      ++i) {
    if(i->first->m_pRect1->m_nWidth + i->first->m_pRect2->m_nWidth >
       m_nMaxWidth) {
      i->second.m_Domain.erase(MetaDomain::LEFTOF);
      i->second.m_Domain.erase(MetaDomain::RIGHTOF);
    }
    if(i->first->m_pRect1->m_nHeight + i->first->m_pRect2->m_nHeight >
       m_nMaxHeight) {
      i->second.m_Domain.erase(MetaDomain::ABOVE);
      i->second.m_Domain.erase(MetaDomain::BELOW);
    }
    if(i->second.m_Domain.empty())
      return(false);
  }
  return(true);
}

bool MetaFrame::updateMinimumBounds(UInt nWidth, UInt nHeight,
				    UInt nArea) {
  m_nMinWidth = std::max(m_nMinWidth, nWidth);
  m_nMinHeight = std::max(m_nMinHeight, nHeight);
  m_nMaxWidth = std::min(m_nMaxWidth, nArea / m_nMinHeight);
  m_nMaxHeight = std::min(m_nMaxHeight, nArea / m_nMinWidth);
  return(m_nMinWidth <= m_nMaxWidth &&
	 m_nMinHeight <= m_nMaxHeight);
}

void MetaFrame::subsumeVariables() {

  /**
   * Iterate over all unassigned variables, and check if they are
   * already satisfied in the APSP.
   */

  std::list<std::map<const MetaVarDesc*, MetaVariable>::iterator> lErase;
  for(std::map<const MetaVarDesc*, MetaVariable>::iterator i =
	m_Unassigned.begin();
      i != m_Unassigned.end(); ++i) {

    /**
     * Is the LEFTOF relation implied?
     */
    
    const Rectangle* r1(i->first->m_pRect1);
    const Rectangle* r2(i->first->m_pRect2);
    if(m_vXMatrix[r2->m_nID][r1->m_nID] <= (Int) r1->m_nWidth) {
      lErase.push_back(i);
      i->second.assign(MetaDomain::LEFTOF);
      continue;
    }

    /**
     * Is the RIGHTOF relation implied?
     */

    if(m_vXMatrix[r1->m_nID][r2->m_nID] <= - (Int) r2->m_nWidth) {
      lErase.push_back(i);
      i->second.assign(MetaDomain::RIGHTOF);
      continue;
    }

    /**
     * Is the BELOW relation implied?
     */

    if(m_vYMatrix[r2->m_nID][r1->m_nID] <= - (Int) r1->m_nHeight) {
      lErase.push_back(i);
      i->second.assign(MetaDomain::BELOW);
      continue;
    }

    /**
     * Is the ABOVE relation implied?
     */

    if(m_vXMatrix[r1->m_nID][r2->m_nID] <= - (Int) r2->m_nHeight) {
      lErase.push_back(i);
      i->second.assign(MetaDomain::ABOVE);
      continue;
    }
  }

  /**
   * Now insert all of these variables into our assigned set.
   */

  while(!lErase.empty()) {
    m_Assigned[lErase.back()->first] = lErase.back()->second;
    m_Unassigned.erase(lErase.back());
    lErase.pop_back();
  }
}

void MetaFrame::print() const {
  
  /**
   * Print out the assigned variables.
   */

  std::cout << "Assigned:" << std::endl;
  for(ConstVarIter i = m_Assigned.begin(); i != m_Assigned.end(); ++i)
    std::cout << "    " << i->second << std::endl;
  std::cout << std::endl;
  std::cout << "Unassigned:" << std::endl;
  for(ConstVarIter i = m_Unassigned.begin(); i != m_Unassigned.end();
      ++i)
    std::cout << "    " << i->second << std::endl;
  std::cout << std::endl << std::endl
	    << "Horizontal APSP matrix:" << std::endl
	    << m_vXMatrix << std::endl
	    << "Vertical APSP matrix:" << std::endl
	    << m_vYMatrix << std::endl
	    << "Min=" << m_nMinWidth << 'x' << m_nMinHeight << ", "
	    << "Max=" << m_nMaxWidth << 'x' << m_nMaxHeight
	    << std::endl;
}
