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

#ifndef COMPONENTPTRS_H
#define COMPONENTPTRS_H

#include <deque>
#include <map>
#include "Integer.h"
#include "RectPtrArray.h"

class Component;

class ComponentPtrs : public std::deque<Component*> {
 public:
  ComponentPtrs();
  virtual ~ComponentPtrs();
  Component* contains(const Rectangle* r);
  void clear();

  /**
   * Creates a new empty component of unit width and height at
   * location nX.
   */

  Component* newEmptyUnit(const UInt& nX);

  /**
   * Creates a new empty space template.
   */

  Component* newEmptyTemplateX(const UInt& nX, const UInt& nWidth,
			       const UInt& nMaxHeight);

  /**
   * Creates a new empty component.
   */

  Component* newEmptyComponentX(const UInt& nX, const UInt& nWidth,
				const UInt& nHeight);

  /**
   * Forks off a new component from the given empty component
   * template, with a fixed x-coordinate, and the given height.
   */

  Component* forkEmptyX(Component* pEmpty, const UInt& nHeight);
  void unforkEmptyX(Component* pEmpty, Component* pForked);

  /**
   * Combines two components horizontally, and returns the combined
   * component. We also make the two input components non-toplevel.
   */

  Component* combineHorizontally(Component* p1, Component* p2);

  /**
   * Extends a currently existing component to the right, up to but
   * not including the given x-coordinate. That is, the new component
   * will have an x2 value equal to nX.
   */

  Component* extendRight(Component* c, const UInt& nX);
  
  /**
   * Returns the cumulative profile of the components in the array. We
   * assume that the x-coordinates are fixed, and only examine the
   * top-level elements. We also account for the proper multiplicity
   * settings in the Component class object. The returned data
   * structure maps an x-coordinate to the height. Consecutively equal
   * heights are not merged, so therefore there is one point per
   * rectangle (due to endpoint overlap, approximately).
   */

  void getCumulativeProfile(std::map<UInt, UInt>& m) const;
  void instantiate();
  void instantiateX();
  void instantiateY();
  void instantiate(const RectPtrArray::iterator& iBegin,
		   const RectPtrArray::const_iterator& iEnd);
  void instantiateX(const RectPtrArray::iterator& iBegin,
		    const RectPtrArray::const_iterator& iEnd);
  void instantiateY(const RectPtrArray::iterator& iBegin,
		    const RectPtrArray::const_iterator& iEnd);
  void print() const;
};

#endif // COMPONENTPTRS_H
