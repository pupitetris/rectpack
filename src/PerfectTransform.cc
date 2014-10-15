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

#include "AlignedComponentPtrs.h"
#include "BoxDimensions.h"
#include "Component.h"
#include "Cumulative.h"
#include "Parameters.h"
#include "PerfectTransform.h"
#include "Rectangle.h"

PerfectTransform::PerfectTransform() :
  m_nTopLevel(0),
  m_nUnit(1, 1),
  m_pBox(NULL),
  m_pParams(NULL),
  m_bEmptyComposition(true),
  m_bImplicitRepresentation(true) {
}

PerfectTransform::~PerfectTransform() {
}

void PerfectTransform::initialize(const Cumulative& c) {
  m_nTopLevel = 0;
  m_vComponents.clear();

  /**
   * First add in each solid rectangle along with its x-coordinate.
   */

  for(std::vector<CompulsoryAssign>::const_iterator i =
	c.values().begin(); i != c.values().end(); ++i) {
    const Rectangle* r = i->m_nValue.m_pRect;
    Component* pc = new Component();
    pc->m_nID = r->m_nID;
    pc->m_Dims.initialize(r->m_nWidth, r->m_nHeight);
    pc->m_nX = i->m_nValue.left();
    pc->m_pRect = r;
    ++m_nTopLevel;
    m_vComponents.push_back(pc);
  }

  /**
   * Now create empty space components based on a quick scan of the
   * Cumulative class object. The algorithm works as follows. We'll
   * compare the differing height levels from each control
   * point. Between every two control point, we'll compute the width
   * difference. Then find the empty space area using this width and
   * the empty height. Dividing this area by the unit empty area we
   * would get the number of components for this one part.
   */

  Cumulative::const_iterator iPrev = c.begin();
  Cumulative::const_iterator iNext = iPrev; iNext++;
  while(iNext != c.end()) {

    /**
     * Compute the number of empty space increments between the two
     * control points.
     */

    UInt nWidthUnits =
      (iNext->first - iPrev->first) /
      m_nUnit.m_nWidth;
    UInt nHeightUnits =
      (m_pBox->m_nHeight - iPrev->second.m_nHeight) /
      m_nUnit.m_nHeight;
    std::list<std::pair<UInt, Dimensions> > lUnits;
    lUnits.push_back(std::pair<UInt, Dimensions>(iPrev->first, Dimensions(nWidthUnits, nHeightUnits)));

    /**
     * Add in the empty components.
     */

    while(!lUnits.empty()) {
      if(lUnits.back().second.m_nWidth && lUnits.back().second.m_nHeight)
	for(UInt w = 0; w < lUnits.back().second.m_nWidth; ++w)
	  for(UInt h = 0; h < lUnits.back().second.m_nHeight; ++h) {
	    Component* pc = new Component();
	    pc->m_nID = m_vComponents.size();
	    pc->m_Dims = m_nUnit;
	    pc->m_bEmpty = true;
	    pc->m_nX = iPrev->first + (w * m_nUnit.m_nWidth);
	    ++m_nTopLevel;
	    m_vComponents.push_back(pc);
	  }
      lUnits.pop_back();
    }
    
    /**
     * Advance the appropriate pointers;
     */

    iPrev = iNext;
    ++iNext;
  };
}

void PerfectTransform::initialize(const Cumulative& c,
				  AlignedComponentPtrs& lp,
				  AlignedComponentPtrs& rp) {
  m_nTopLevel = 0;
  m_vComponents.clear();

  /**
   * First add in each solid rectangle along with its x-coordinate.
   */

  for(std::vector<CompulsoryAssign>::const_iterator i =
	c.values().begin(); i != c.values().end(); ++i) {
    const Rectangle* r = i->m_nValue.m_pRect;
    Component* pc = new Component();
    pc->m_nID = r->m_nID;
    pc->m_Dims.initialize(r->m_nWidth, r->m_nHeight);
    pc->m_nX = i->m_nValue.left();
    pc->m_pRect = r;
    ++m_nTopLevel;
    m_vComponents.push_back(pc);
  }

  /**
   * Next insert the new components into our indexing structures, the
   * left and right indices. At this point we don't need the aligned
   * congruencies just yet.
   */

  lp.initializex1(m_vComponents);
  rp.initializex2(m_vComponents);

  /**
   * Next, loop until steady state, by applying our rectangle
   * extension rules.
   */

  bool bContinue(true);
  while(m_bEmptyComposition && bContinue) {
    bContinue = false;
    bContinue |= extendEmpty(lp, rp);
  }

  /**
   * Now build all of the required horizontal strips.
   */

  std::map<UInt, UInt> m;
  m_vComponents.getCumulativeProfile(m);
  if(m_bImplicitRepresentation)
    horizontalEmpty(m, lp, rp);
  else
    unitEmpty(m, lp, rp);
}

bool PerfectTransform::horizontalEmpty(const std::map<UInt, UInt>& m,
				       AlignedComponentPtrs& lp,
				       AlignedComponentPtrs& rp) {
  assert(m.size() >= 2);
  std::map<UInt, UInt>::const_iterator i(m.begin());
  std::map<UInt, UInt>::const_iterator j(m.begin());
  ++j;
  bool bAdded(false);
  for(; j != m.end(); ++i, ++j)
    if(i->second < m_pBox->m_nHeight) {
      Component* pEmpty =
	m_vComponents.newEmptyTemplateX(i->first, j->first - i->first,
					m_pBox->m_nHeight - i->second);
      ++m_nTopLevel;
      lp.addx1(pEmpty);
      rp.addx2(pEmpty);
      bAdded = true;
    }
  return(bAdded);
}

bool PerfectTransform::unitEmpty(const std::map<UInt, UInt>& m,
				 AlignedComponentPtrs& lp,
				 AlignedComponentPtrs& rp) {
  assert(m.size() >= 2);
  std::map<UInt, UInt>::const_iterator i(m.begin());
  std::map<UInt, UInt>::const_iterator j(m.begin());
  ++j;
  for(; j != m.end(); ++i, ++j)
    if(i->second < m_pBox->m_nHeight) {
      UInt nWidth = j->first - i->first;
      UInt nHeight = m_pBox->m_nHeight - i->second;
      for(UInt k = 0; k < nWidth; ++k)
	for(UInt l = 0; l < nHeight; ++l) {
	  Component* pEmpty = m_vComponents.newEmptyUnit(i->first + k);
	  ++m_nTopLevel;
	  lp.addx1(pEmpty);
	  rp.addx2(pEmpty);
	}
    }
  return(true);
}

bool PerfectTransform::extendEmpty(AlignedComponentPtrs& lp,
				   AlignedComponentPtrs& rp) {
  
  /**
   * Consider the empty space induced by extending from the left side
   * of the bounding box towards the right, to the first left borders.
   */

  bool bConverted(false);
  if(lp.empty()) {
    
    /**
     * The left side extends all the way to the right side of the
     * bounding box. Generate one giant empty component, and return.
     */

    Component* pc = 
      m_vComponents.newEmptyComponentX(0, m_pBox->m_nWidth,
				       m_pBox->m_nHeight);
    ++m_nTopLevel;
    lp.addx1(pc);
    rp.addx2(pc);
    return(true);
  }
  else if(lp.begin()->first > 0) {

    /**
     * The left side extends up to the first item.
     */

    Component* pc =
      m_vComponents.newEmptyComponentX(0, lp.begin()->first,
				       m_pBox->m_nHeight);
    ++m_nTopLevel;
    lp.addx1(pc);
    rp.addx2(pc);
    bConverted = true;
  }

  /**
   * Then, for every right ending point, attempt to extend to the
   * right, up to the first left border encountered. Iterator i
   * represents the right side from which we start the extension.
   */

  for(AlignedComponentPtrs::iterator i = rp.begin(); i != rp.end();) {

    /**
     * Iterator j represents the left side at which we end the
     * extension.
     */

    AlignedComponentPtrs::iterator j = lp.upper_bound(i->first);
    if(j == lp.end()) {

      /**
       * We can extend every single component within the set of items
       * whose right sides are included with iterator i. Note that
       * since we're extending the right side, the extension function
       * will tell us the next iterator to try. Otherwise we should
       * manually increment the iterator.
       */
      
      if(i->first + 1 < m_pBox->m_nWidth) {
	extendR(i, m_pBox->m_nWidth, lp, rp);
	bConverted = true;
      }
      else ++i;
    }
    else {
      if(i->first + 1 < j->first) {
	extendR(i, j->first, lp, rp);
	bConverted = true;
      }
      else ++i;
    }    
  }
  return(bConverted);
}

void PerfectTransform::initialize(const BoxDimensions* pBox) {
  m_pBox = pBox;
  m_nUnit.initialize(1, 1);
}

void PerfectTransform::initialize(const Parameters* pParams) {
  m_pParams = pParams;
  if(pParams->m_s2.find_first_of('e') < pParams->m_s2.size())
    m_bEmptyComposition = false;
  if(pParams->m_s2.find_first_of('i') < pParams->m_s2.size())
    m_bImplicitRepresentation = false;
}

void PerfectTransform::extendR(AlignedComponentPtrs::iterator& s,
			       const UInt& n,
			       AlignedComponentPtrs& lp,
			       AlignedComponentPtrs& rp) {
  for(std::set<Component*>::iterator i = s->second.begin();
      i != s->second.end(); ++i) {

    /**
     * First extend the given component to the right.
     */

    Component* pNew = m_vComponents.extendRight(*i, n);

    /**
     * Insert the new component that captures both the empty space as
     * well as the previous component.
     */

    lp.delx1(*i);
    lp.addx1(pNew);
    rp.addx2(pNew);
  }

  /**
   * Now remove the previous indexed data structure.
   */

  if(s == rp.begin()) {
    rp.erase(s);
    s = rp.begin();
  }
  else {
    AlignedComponentPtrs::iterator t(s);
    ++t;
    rp.erase(s);
    s = t;
  }
}
