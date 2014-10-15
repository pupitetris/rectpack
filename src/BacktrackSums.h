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

#ifndef BACKTRACKSUMS_H
#define BACKTRACKSUMS_H

#include "Integer.h"
#include "RectPtrArray.h"
#include "SimpleSums.h"
#include <deque>

class Compulsory;

class BacktrackSums : public SimpleSums {
 public:
  BacktrackSums();
  ~BacktrackSums();
  void initialize(RectPtrArray::const_iterator iBegin,
		  RectPtrArray::const_iterator iEnd);
  void clear();
  void push();
  void push(const UInt& s);
  void pop();

  virtual void add(const UInt& n, const UInt& nMax);
  virtual void add(const Compulsory& nValue, const UInt& nMax);

  std::deque<iterator> m_vStack;
  std::deque<UInt> m_vStackSize;

  std::vector<SimpleSums> m_vUnplaced;
};

#endif // BACKTRACKSUMS_H
