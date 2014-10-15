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

#ifndef COMPOSEDEMPTY_H
#define COMPOSEDEMPTY_H

#include "Bins.h"
#include "Integer.h"
#include "NodeCount.h"
#include "SpaceFill.h"
#include "WastedBins.h"
#include <string>
#include <vector>

class BoxDimensions;
class Cumulative;
class Packer;
class Parameters;
class TimeSpec;

/**
 * Represents the data structures, logic, and everything related to
 * the task of perfect packing.
 */

class ComposedEmpty : public SpaceFill {

 public:
  ComposedEmpty();
  ~ComposedEmpty();
  void initialize(Packer* pPacker);
  void initialize(const Parameters* pParams);
  void initialize(const BoxDimensions* pBox);
  virtual bool packY(Cumulative& c);
  bool packYAux(Int nDepth);
  void get(Placements& v) const;
  void get(IntPlacements& v) const;

  IntPlacements m_vValues;
  std::list<Intersect> m_lVariables;
  std::set<IntPlacement*> m_lUnassigned;
  std::set<IntPlacement*> m_lAssigned;
  size_t m_nDepth;

  /**
   * Some debugging functions.
   */

  void print() const;

  /**
   * Data structures that are given to us by the calling class that we
   * need to modify in order to monitor the search correctly.
   */

  UInt* m_pCuSP;
  const BoxDimensions* m_pBox;
  TimeSpec* m_pTime;
  NodeCount* m_pNodes;

};

#endif // COMPOSEDEMPTY_H
