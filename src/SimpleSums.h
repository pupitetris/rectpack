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

#ifndef SIMPLESUMS_H
#define SIMPLESUMS_H

#include "Integer.h"
#include "RectArray.h"
#include <set>

class Compulsory;
class DimsFunctor;
class Rectangle;
class SimpleSums;

class SimpleSums : public std::set<UInt> {
 public:
  SimpleSums();
  virtual ~SimpleSums();
  void initializeInts(UInt nMin, UInt nMax);
  void initializeW(RectArray::const_iterator iBegin,
		   RectArray::const_iterator iEnd);
  void initializeH(RectArray::const_iterator iBegin,
		   RectArray::const_iterator iEnd);
  virtual void add(const UInt& n);
  virtual void add(const UInt& n, const UInt& nMax);
  virtual void add(const UInt& n1, const UInt& n2, const UInt& nMax);
  virtual void add(const Compulsory& nValue, const UInt& nMax);

 private:
  void initialize(RectArray::const_iterator iBegin,
		  RectArray::const_iterator iEnd,
		  const DimsFunctor* pDims);
  void print() const;
  void print(const const_iterator& i) const;
};

#endif // SIMPLESUMS_H
