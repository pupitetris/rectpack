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

#ifndef PLACEMENTS_H
#define PLACEMENTS_H

#include "Integer.h"
#include "Placement.h"
#include "RatDims.h"
#include "Rational.h"
#include <vector>

class AsciiGrid;
class Parameters;
class PlacementDec;

class Placements : public std::vector<Placement> {
 public:
  Placements();
  virtual ~Placements();
  void assignX();
  void assignY();
  Placements& operator*=(const URational& ur);
  Placements& operator/=(const URational& ur);
  URational minLabelScale() const;
  void print() const;
  bool verify() const;
  void rotate();
  RatDims m_Box;

 private:
  void assign(std::vector<PlacementDec*>& v,
	      bool bSort = false) const;
  UInt widthColumn(UInt nColumn, UInt nColumns,
		   const std::vector<std::string>& v) const;
  UInt widthLine(UInt nColumns,
		 const std::vector<std::string>& v) const;
  UInt widthSpace(UInt nColumns) const;
};

#endif // PLACEMENTS_H
