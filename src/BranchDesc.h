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

#ifndef BRANCHDESC_H
#define BRANCHDESC_H

#include "Integer.h"
#include "Interval.h"
#include <iostream>
#include <vector>

class BranchDesc;
class Rectangle;

class BranchDesc {
  friend std::ostream& operator<<(std::ostream& os, const BranchDesc& rhs);

 public:
  BranchDesc();
  BranchDesc(const BranchDesc& src);
  ~BranchDesc();
  UInt total() const;
  bool operator<(const BranchDesc& rhs) const;
  const BranchDesc& operator=(const BranchDesc& src);
  void print(const UInt pTotal[4], const UInt pBranches[4][3],
	     std::ostream& os) const;

  enum Config {
    TT,
    TF,
    FT,
    FF
  };

  UInt m_pTotal[4];
  UInt m_pBranches[4][3];

  UInt m_pRTotal[4];
  UInt m_pRBranches[4][3];
  Rectangle* m_pRect;
};

#endif // BRANCHDESC_H
