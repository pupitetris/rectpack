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

#ifndef RATPACK_H
#define RATPACK_H

#include "Cumulative.h"
#include "IntPlacements.h"
#include "Packer.h"
#include "TimeSpec.h"

class HeapBox;
class Placements;
class SpaceFill;

class RatPack : public Packer {
 public:
  RatPack();
  virtual ~RatPack();
  virtual void initialize(Packer* pPacker);
  virtual void initialize(const Parameters* pParams);
  virtual void initialize(const HeapBox* pBox);
  virtual void initialize(const BoundingBoxes* pBoxes);
  virtual bool pack();
  virtual bool packX();
  virtual bool packY();
  virtual void get(Placements& v) const;
  virtual TimeSpec& timeDomination();

 protected:
  Cumulative m_vX;

  /**
   * A pointer to the packer we will use for deciding the
   * y-coordinates.
   */

  SpaceFill* m_pSpaceFill;

 protected:
  void placeUnitRectangles();
  IntPlacements m_vUnits;
  TimeSpec m_DominationTimeStub;
};

#endif // RATPACK_H
