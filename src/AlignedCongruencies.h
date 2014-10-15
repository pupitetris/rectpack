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

#ifndef ALIGNEDCONGRUENCIES_H
#define ALIGNEDCONGRUENCIES_H

#include "Integer.h"
#include "SymmetryMap.h"
#include <map>
#include <set>
#include <vector>

class AlignedComponentPtrs;
class AlignedComponents;
class Component;
class ComponentPtrs;

class AlignedCongruencies : public std::map<UInt, SymmetryMap> {
 public:
  AlignedCongruencies();
  virtual ~AlignedCongruencies();
  void initialize(const AlignedComponentPtrs& aligned);
  void initialize(const AlignedComponents& aligned,
		  const ComponentPtrs& vComponents);
  void print() const;
};

#endif // ALIGNEDCONGRUENCIES_H
