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

#ifndef SUBSETSUMSSET_H
#define SUBSETSUMSSET_H

#include "Integer.h"
#include "SubsetSums.h"
#include <vector>

class RectArray;

class SubsetSumsSet : public std::vector<SubsetSums> {
 public:
  
  enum Orientation {
    WIDTH,
    HEIGHT
  };

  SubsetSumsSet();
  virtual ~SubsetSumsSet();
  void initialize(const RectArray* pRects, const SubsetSums* pCached,
		  Orientation nType);
  const SubsetSums& operator[](const UInt& i);

 private:
  void print(UInt n) const;

  const RectArray* m_pRects;
  const SubsetSums* m_pCached;
  Orientation m_nType;
};

#endif // SUBSETSUMSSET_H
