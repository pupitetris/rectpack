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

#ifndef CONFLICTBT_H
#define CONFLICTBT_H

#include "BacktrackSums.h"
#include "Domains.h"
#include "Integer.h"
#include "RatPack.h"
#include "SubsetSums.h"

class BoundingBoxes;
class Compulsory;
class HeapBox;

class ConflictBT : public RatPack {
 public:
  ConflictBT();
  virtual ~ConflictBT();
  void initialize(const Parameters* pParams);
  void initialize(const HeapBox* pBox);
  void initialize(const BoundingBoxes* pBoxes);
  virtual bool packX();
  virtual bool packY();

 private:
  bool packInterval(RectPtrArray::iterator i, UInt& nDeepest);
  bool packSingle(const Compulsory* c, RectPtrArray::iterator i,
		  UInt& nDeepest);
  bool nextVariable(RectPtrArray::iterator i, UInt& nDeepest);
  bool orientation(RectPtrArray::iterator i, UInt& nDeepest);

  /**
   * Loops over all interval assignments that have been made,
   * computing the area that would be now added to the cumulative
   * profile once we made an assignment of a single coordinate value.
   */

  void singleArea(UInt& n, Rectangle*& r);

  Domains m_vDomains;

  /**
   * Our own local copy of the subset sums is generated and usedd if,
   * during initialization, the bounding boxes doesn't provide one for
   * us.
   */

  const SubsetSums* m_pWidths;
  const SubsetSums* m_pHeights;
  SubsetSums m_Widths;
  SubsetSums m_Heights;

  BacktrackSums m_vSums;
};

#endif // CONFLICTBT_H
