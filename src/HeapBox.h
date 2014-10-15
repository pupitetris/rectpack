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

#ifndef HEAPBOX_H
#define HEAPBOX_H

#include "BoxDimensions.h"
#include "Integer.h"
#include "SubsetSums.h"

class HeapBox;

class HeapBox {
 public:
  HeapBox();
  virtual ~HeapBox();
  void initialize(SubsetSums::const_iterator& w,
		  SubsetSums::const_iterator& h);
  void initializeH(SubsetSums::const_iterator& h);
  bool operator==(const HeapBox& hb) const;
  bool operator>(const HeapBox& hb) const;
  bool operator<(const HeapBox& hb) const;

  /**
   * @return true if there is no intersection or there is only one
   * element of intersection and the orientation isn't contradictory,
   * otherwise false if an intersection exist and it's more than one
   * element.
   */

  bool intersect(SubsetSums::const_iterator& w);
  void rotate();
  void print() const;

  BoxDimensions m_Box;
  SubsetSums::const_iterator m_iHeight;

  /**
   * For the given bounding box, this represents any rotational
   * constraints on the rectangles due to the particular combination
   * of widths and heights.
   */

  SubsetSums::data_type m_Rects;
  bool m_bRotated;
  UInt m_nConflictLearningIndex;
};

#endif // HEAPBOX_H
