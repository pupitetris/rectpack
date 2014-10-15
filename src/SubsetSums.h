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

#ifndef SUBSETSUMS_H
#define SUBSETSUMS_H

#include "Integer.h"
#include "MutexMap.h"
#include "RectArray.h"
#include <map>

class DimsFunctor;
class Rectangle;
class SubsetSums;

class SubsetSums : public std::map<UInt, MutexMap> {
 public:
  typedef UInt key_type;
  typedef MutexMap data_type;
  typedef std::pair<key_type, data_type> value_type;

  SubsetSums();
  virtual ~SubsetSums();
  void initializeInts(UInt nMin, UInt nMax);
  void initializeW(RectArray::const_iterator iBegin,
		   RectArray::const_iterator iEnd);
  void initializeH(RectArray::const_iterator iBegin,
		   RectArray::const_iterator iEnd);
  UInt m_nEpsilon;

 private:
  void initialize(RectArray::const_iterator iBegin,
		  RectArray::const_iterator iEnd,
		  const DimsFunctor* pDims);
  void computeEpsilon();

  /**
   * Computes the set intersection between a and b, and store the
   * results in a. Also, if there are common rectangle entries in both
   * sets, we will take the union of their orientation specifications.
   */

  void intersect(data_type& a, const data_type& b);
  void print() const;
  void print(UInt n) const;
  void print(const const_iterator& i) const;
  void print(const iterator& i) const;
};

#endif // SUBSETSUMS_H
