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

#include "EmptyDom.h"

EmptyDom::EmptyDom() {
}

EmptyDom::~EmptyDom() {
}

bool EmptyDom::dominated(const Rectangle* r, UInt nGap) const {
  return(false);
}

bool EmptyDom::dominatedw(const Rectangle* r, UInt nGap) const {
  return(false);
}

bool EmptyDom::dominatedh(const Rectangle* r, UInt nGap) const {
  return(false);
}

UInt EmptyDom::entries(const Rectangle* r) const {
  return(0);
}

UInt EmptyDom::entriesw1(const Rectangle* r) const {
  return(0);
}

UInt EmptyDom::entriesw2(const Rectangle* r) const {
  return(0);
}

UInt EmptyDom::entriesh1(const Rectangle* r) const {
  return(0);
}

UInt EmptyDom::entriesh2(const Rectangle* r) const {
  return(0);
}

UInt EmptyDom::gap(const Rectangle* r) const {
  return(0);
}

UInt EmptyDom::gapw(const Rectangle* r) const {
  return(0);
}

UInt EmptyDom::gaph(const Rectangle* r) const {
  return(0);
}

void EmptyDom::buildTable() {
}
