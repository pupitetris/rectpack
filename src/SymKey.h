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

#ifndef SYMKEY_H
#define SYMKEY_H

#include "Integer.h"
#include <cstddef>
#include <vector>

class SymKey;

class SymKey {
 public:
  SymKey();
  SymKey(const SymKey& rhs);
  SymKey(UInt nMinCoordinate, UInt nID,
	 std::vector<std::vector<UInt> >* pTable, UInt nGap);
  ~SymKey();
  const SymKey& operator=(const SymKey& rhs);
  bool operator==(const SymKey& rhs) const;

  /**
   * The major sort is by the minimum coordinate, ID, width, and
   * finally the gap.
   */

  bool operator<(const SymKey& rhs) const;

  /**
   * This is the minimum coordinate that can be forced upon the
   * top-level rectangle in the dominating position.
   */

  UInt m_nMinCoordinate;

  /**
   * This is the ID of the rectangle who provides the wall.
   */

  UInt m_nID;

  /**
   * Specifies which table to use (this matters a lot for the
   * unoriented case when the wall may be formed by either the width
   * or the height of the original unrotated rectangle).
   */

  std::vector<std::vector<UInt> >* m_pTable;

  /**
   * Specifies the gap width, allowing us to index into the tables.
   */

  UInt m_nGap;
};

#endif // SYMKEY_H
