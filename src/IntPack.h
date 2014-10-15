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

#ifndef INTPACK_H
#define INTPACK_H

#include "Bins.h"
#include "Integer.h"
#include "Packer.h"
#include "RectPtrArray.h"
#include <vector>

class BoundingBoxes;
class BoxDimensions;
class Domination;
class Grid;
class GridViz;
class Parameters;
class Perfect;
class Placements;
class TimeSpec;

class IntPack : public Packer {

 public:
  IntPack();
  virtual ~IntPack();
  virtual void initialize(const Parameters* pParams);
  virtual void initialize(const HeapBox* pBox);
  virtual void initialize(const BoundingBoxes* pBoxes);
  virtual void finalizeDomination();

  /**
   * After packing the first dimension, this call packs the second
   * dimension.
   */

  bool packX();

  /**
   * One-dimensional bitmap.
   */

  Bins m_vY;

#ifdef VIZ
  GridViz* m_pGrid;
#else
  Grid* m_pGrid;
#endif

  virtual void placeUnitRectangles();
  virtual void get(Placements& v) const;

  /**
   * Tells us when certain positions are dominated.
   */

  Domination* m_pDomination;
  Domination* m_pDomTemplate;
  virtual TimeSpec& timeDomination();

  /**
   * Packing mechanism for the second dimension.
   */

  Perfect* m_pPerfect;
};

#endif // INTPACK_H
