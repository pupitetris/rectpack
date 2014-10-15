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

#ifndef EMPTYDOM_H
#define EMPTYDOM_H

#include "Domination.h"
#include "Integer.h"

class Rectangle;

class EmptyDom : public Domination {
 public:
  EmptyDom();
  ~EmptyDom();
  bool dominated(const Rectangle* r, UInt nGap) const;
  bool dominatedw(const Rectangle* r, UInt nGap) const;
  bool dominatedh(const Rectangle* r, UInt nGap) const;
  UInt entries(const Rectangle* r) const;
  UInt entriesw1(const Rectangle* r) const;
  UInt entriesw2(const Rectangle* r) const;
  UInt entriesh1(const Rectangle* r) const;
  UInt entriesh2(const Rectangle* r) const;
  UInt gap(const Rectangle* r) const;
  UInt gapw(const Rectangle* r) const;
  UInt gaph(const Rectangle* r) const;
  void buildTable();
};

#endif // EMPTYDOM_H
