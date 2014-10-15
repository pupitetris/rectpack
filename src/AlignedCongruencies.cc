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

#include "AlignedComponentPtrs.h"
#include "AlignedComponents.h"
#include "AlignedCongruencies.h"
#include "Component.h"
#include "ComponentPtrs.h"

AlignedCongruencies::AlignedCongruencies() {
}

AlignedCongruencies::~AlignedCongruencies() {
}

void AlignedCongruencies::
initialize(const AlignedComponentPtrs& aligned) {
  clear();
  for(AlignedComponentPtrs::const_iterator i = aligned.begin();
      i != aligned.end(); ++i) {
    if(i->second.empty()) continue;
    iterator k = insert(std::pair<UInt, SymmetryMap>
			(i->first, SymmetryMap())).first;
    k->second.clear();
    for(std::set<Component*>::const_iterator j = i->second.begin();
	j != i->second.end(); ++j)
      k->second[(*j)->m_Dims].insert(*j);
  }
}

void AlignedCongruencies::
initialize(const AlignedComponents& aligned,
	   const ComponentPtrs& vComponents) {
  clear();
  for(AlignedComponents::const_iterator i = aligned.begin();
      i != aligned.end(); ++i) {
    if(i->second.empty()) continue;
    iterator k = insert(std::pair<UInt, SymmetryMap>
			(i->first, SymmetryMap())).first;
    k->second.clear();
    for(std::set<UInt>::const_iterator j = i->second.begin();
	j != i->second.end(); ++j)
      k->second[vComponents[*j]->m_Dims].insert(vComponents[*j]);
  }
}

void AlignedCongruencies::print() const {
  for(std::map<UInt, SymmetryMap>::const_iterator i = begin();
      i != end(); ++i) {
    std::cout << i->first << "=";
    i->second.print();
    std::cout << std::endl;
  }
}
