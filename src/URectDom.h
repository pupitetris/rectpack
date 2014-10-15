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

#ifndef URECTDOM_H
#define URECTDOM_H

#include "Domination.h"
#include "Integer.h"

class URectDom;

class URectDom : public Domination {
 public:
  URectDom();
  URectDom(const URectDom& src);
  ~URectDom();

  bool dominatedw1(const Rectangle* r, UInt nGap) const {
    return(dominatedw(r, nGap));
  }

  bool dominatedw2(const Rectangle* r, UInt nGap) const {
    return(dominatedw(r, nGap));
  }

  bool dominatedh1(const Rectangle* r, UInt nGap) const {
    return(dominatedh(r, nGap));
  }

  bool dominatedh2(const Rectangle* r, UInt nGap) const {
    return(dominatedh(r, nGap));
  }

  UInt entriesw1(const Rectangle* r) const {
    return(entriesw(r));
  }

  UInt entriesw2(const Rectangle* r) const {
    return(entriesw(r));
  }

  UInt entriesh1(const Rectangle* r) const {
    return(entriesh(r));
  }

  UInt entriesh2(const Rectangle* r) const {
    return(entriesh(r));
  }

  UInt gapw1(const Rectangle* r) const {
    return(gapw(r));
  }

  UInt gapw2(const Rectangle* r) const {
    return(gapw(r));
  }

  UInt gaph1(const Rectangle* r) const {
    return(gaph(r));
  }

  UInt gaph2(const Rectangle* r) const {
    return(gaph(r));
  }

  /**
   * The width of the rectangle forms the length of the wall.
   */

  void buildTable() { };
  void buildTableStage1() { };
  void buildTableStage2() { };
  Domination* clone() const;

 private:
  bool dominatedw(const Rectangle* r, UInt nGap) const;
  bool dominatedh(const Rectangle* r, UInt nGap) const;
  UInt entriesw(const Rectangle* r) const;
  UInt entriesh(const Rectangle* r) const;
  UInt gapw(const Rectangle* r) const;
  UInt gaph(const Rectangle* r) const;
  static UInt m_pRectShort[28][8];
  static UInt m_pRectLong[28][8];
  static UInt m_pRectSizeShort[28];
  static UInt m_pRectSizeLong[28];
  static UInt m_pRectSkipShort[28];
  static UInt m_pRectSkipLong[28];
};

#endif // URECTDOM_H
