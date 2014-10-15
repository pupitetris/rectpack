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

#ifndef SQUAREDOM_H
#define SQUAREDOM_H

#include "Domination.h"
#include "Integer.h"

class SquareDom;

class SquareDom : public Domination {
 public:
  SquareDom();
  SquareDom(const SquareDom& src);
  ~SquareDom();

  bool dominatedw1(const Rectangle* r, UInt nGap) const {
    return(_dominated(r, nGap));
  }

  bool dominatedw2(const Rectangle* r, UInt nGap) const {
    return(_dominated(r, nGap));
  }

  bool dominatedh1(const Rectangle* r, UInt nGap) const {
    return(_dominated(r, nGap));
  }

  bool dominatedh2(const Rectangle* r, UInt nGap) const {
    return(_dominated(r, nGap));
  }

  UInt entriesw1(const Rectangle* r) const {
    return(_entries(r));
  }

  UInt entriesw2(const Rectangle* r) const {
    return(_entries(r));
  }

  UInt entriesh1(const Rectangle* r) const {
    return(_entries(r));
  }

  UInt entriesh2(const Rectangle* r) const {
    return(_entries(r));
  }

  UInt gapw1(const Rectangle* r) const {
    return(_gap(r));
  }

  UInt gapw2(const Rectangle* r) const {
    return(_gap(r));
  }

  UInt gaph1(const Rectangle* r) const {
    return(_gap(r));
  }

  UInt gaph2(const Rectangle* r) const {
    return(_gap(r));
  }

  void buildTable() { };
  void buildTableStage1() { };
  void buildTableStage2() { };
  Domination* clone() const;

 private:
  bool _dominated(const Rectangle* r, UInt nGap) const;
  UInt _entries(const Rectangle* r) const;
  UInt _gap(const Rectangle* r) const;
  static UInt m_pSquare[36][10];
  static UInt m_pSquareSize[36];
  static UInt m_pSquareSkip[36];
};

#endif // SQUAREDOM_H
