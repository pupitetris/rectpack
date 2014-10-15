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

#ifndef SQUAREPACKER_H
#define SQUAREPACKER_H

#include "IntPack.h"
#include "RectSetMinDim.h"
#include "WastedBins.h"
#include <vector>

class SquarePacker : public IntPack {

 public:
  SquarePacker();
  virtual ~SquarePacker();
  virtual void initialize(const Parameters* pParams);
  virtual void initialize(const BoundingBoxes* pBoxes);

 protected:
  virtual bool pack();
  virtual bool packFirst(RectPtrArray::iterator iCurrent,
			 RectPtrArray::const_iterator iEnd,
			 const WastedBins& v);
  virtual bool packAux(RectPtrArray::iterator iCurrent,
		       RectPtrArray::const_iterator iEnd,
		       const WastedBins& v);
  virtual bool packRest(RectPtrArray::iterator iCurrent,
			RectPtrArray::const_iterator iEnd,
			const WastedBins& v);
  virtual void placeUnitRectangles();

  /**
   * Represents the wasted space histogram.
   */

  std::vector<WastedBins> m_vWastedXF;

 private:

  /**
   * Ports over Rich's code for determining when positions are
   * dominated because one square were too close to another
   * square. Note that the comments in this code have the vertical
   * sense of direction reversed. That is, references in the comments
   * to "above" really mean "below", or "positions of lower y-value",
   * etc.
   */

  bool tooClose(const Rectangle* r);

  /**
   * Checks whether or not the placement of square n at location (x,y)
   * creates a perfect rectangle for a square adjacent to it (such
   * that the adjacent square could have slid up or to the left).
   */

  bool createRect(const Rectangle* r);
  bool rectAbove(const Rectangle* r) const;
  bool rectLeft(const Rectangle* r) const;

  /**
   * Initializer and data structure for maintaining a set of rectangle
   * pointers sorted by decreasing width.
   */

  virtual void initRMinDims();
  RectSetMinDim m_sRMinDims;
  bool m_bTooCloseEnable;
};

#endif // SQUAREPACKER_H
