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

#ifndef PERFECTTRANSFORM_H
#define PERFECTTRANSFORM_H

#include "ComponentPtrs.h"
#include "Dimensions.h"
#include "RectPtrArray.h"

class AlignedComponentPtrs;
class BoxDimensions;
class Cumulative;
class Parameters;

class PerfectTransform {
 public:
  PerfectTransform();
  virtual ~PerfectTransform();
  void initialize(const Parameters* pParams);
  void initialize(const BoxDimensions* pBox);
  void initialize(const Cumulative& c);

  /**
   * This is an alternative initialization function for the previous
   * one. It will also populate the given AlignedComponentPtrs (as
   * well as use the data structures to aid in our inference
   * mechanism). This version instantiates a compact representation
   * for the empty space as opposed to instantiated every single unit.
   */

  void initialize(const Cumulative& c,
		  AlignedComponentPtrs& lp,
		  AlignedComponentPtrs& rp);
  ComponentPtrs m_vComponents;

  /**
   * The number of top-level components there are in the array.
   */

  UInt m_nTopLevel;

 private:

  /**
   * Generates a compressed representation for a horizontal strip of
   * empty space with fixed width and variable height.
   */

  bool horizontalEmpty(const std::map<UInt, UInt>& m,
		       AlignedComponentPtrs& lp,
		       AlignedComponentPtrs& rp);

  /**
   * Generates all of the remaining empty space units in terms of 1x1
   * components.
   */

  bool unitEmpty(const std::map<UInt, UInt>& m,
		 AlignedComponentPtrs& lp,
		 AlignedComponentPtrs& rp);

  bool extendEmpty(AlignedComponentPtrs& lp,
		   AlignedComponentPtrs& rp);

  /**
   * Given a pointer to an entire set, and an ending point to extend,
   * we will extend all items in our set up to the given point. Then
   * we'll insert the new component into our data structure. We'll
   * assume that the extension is a valid request (that is, it would
   * result in a larger component that contains those in the given
   * set). Since we will be erasing the iterator s, we will return
   * another iterator which may be used to reset the iteration
   * variable through the same value s.
   */

  void extendR(AlignedComponentPtrs::iterator& s, const UInt& n,
	       AlignedComponentPtrs& lp, AlignedComponentPtrs& rp);
  bool consolidateEmpty(std::list<std::pair<UInt, Dimensions> >& lUnits);

  Dimensions m_nUnit;
  const BoxDimensions* m_pBox;
  const Parameters* m_pParams;

  bool m_bEmptyComposition;
  bool m_bImplicitRepresentation;
};

#endif // PERFECTTRANSFORM_H
