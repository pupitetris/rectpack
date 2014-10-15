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

#ifndef ALIGNEDCOMPONENTS_H
#define ALIGNEDCOMPONENTS_H

#include "Integer.h"
#include <map>
#include <set>
#include <vector>

class Component;
class ComponentPtrs;

class AlignedComponents : public std::map<UInt, std::set<UInt> > {
 public:
  AlignedComponents();
  virtual ~AlignedComponents();

  UInt size() const;
  UInt size(UInt i) const;
  bool empty() const;
  bool empty(UInt i) const;

  void initializey1(const ComponentPtrs& v);
  void initializey2(const ComponentPtrs& v);
  void initializex1(const ComponentPtrs& v);
  void initializex2(const ComponentPtrs& v);

  void addy1(const Component* c);
  void addy2(const Component* c);
  void addx1(const Component* c);
  void addx2(const Component* c);
};

#endif // ALIGNEDCOMPONENTS_H
