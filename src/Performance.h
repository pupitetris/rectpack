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

#ifndef PERFORMANCE_H
#define PERFORMANCE_H

#include "Dimensions.h"
#include "Instance.h"
#include "NodeCount.h"
#include "TimeSpec.h"
#include <deque>

class BoundingBoxes;

class Performance {

  friend std::ostream& operator<<(std::ostream& os, const Performance& rhs);

 public:
  Performance();
  Performance(const BoundingBoxes& b, const Instance& i);
  ~Performance();
  bool operator<(const Performance& rhs) const;
  void print() const;
  void printHeader() const;
  NodeCount m_Total;
  Instance m_vInstance;
};

std::ostream& operator<<(std::ostream& os, const Performance& rhs);

#endif // PERFORMANCE_H
