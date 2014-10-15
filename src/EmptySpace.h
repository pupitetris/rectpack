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

#ifndef EMPTYSPACE_H
#define EMPTYSPACE_H

#include "Integer.h"
#include <set>

class Component;
class EmptyCorner;

class EmptySpace {
 public:
  EmptySpace();
  ~EmptySpace(); 
  
  /**
   * Returns a height restricted by either the max height of this
   * empty space template or the other instantiations (such that we
   * will not be adjacent to those other instantiations, which is the
   * same as having at least a single strip of buffer between our new
   * instantiation and the previous ones).
   */

  UInt maxNonAdjacentHeight(const EmptyCorner& ec) const;

  UInt m_nMaxHeight;
  UInt m_nHeightLeft;
  std::set<Component*> m_Instantiations;

  void print() const;
};

#endif // EMPTYSPACE_H
