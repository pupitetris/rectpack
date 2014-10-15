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

#ifndef SPACEFILL_H
#define SPACEFILL_H

#include <string>

class BoxDimensions;
class Cumulative;
class Dimensions;
class IntPlacements;
class Packer;
class Parameters;
class Placements;
class SubsetSums;

class SpaceFill {
 public:
  SpaceFill();
  virtual ~SpaceFill();
  virtual void initialize(Packer* pPacker);
  virtual void initialize(const Parameters* pParams);
  virtual void initialize(const BoxDimensions* pBox);
  virtual void get(IntPlacements& v) const = 0;
  virtual void get(Placements& v) const = 0;
  virtual bool packY(Cumulative& c, const SubsetSums* pValidY);
  const Parameters* m_pParams;
};

#endif // SPACEFILL_H
