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

#ifndef COMPULSORY_H
#define COMPULSORY_H

#include "Integer.h"
#include "RInterval.h"
#include <ostream>

class Compulsory;
class Rectangle;

class Compulsory {
 public:

  friend std::ostream& operator<<(std::ostream& os, const Compulsory& c);

  Compulsory();
  ~Compulsory();
  void initialize(const UInt& nInterval,
		  const Rectangle* pRectangle);
  void initialize(const UInt& nStart1, const UInt& nStart2,
		  const Rectangle* pRectangle);
  const Compulsory& operator+=(const UInt& ur);
  UInt size() const;
  UInt csize() const;
  UInt area() const;
  UInt carea() const;
  bool overlaps(UInt n) const;

  /**
   * Returns whether or not this compulsory part is actually the full
   * rectangle. This is deduced by checking to see if we're actually
   * describing only exactly one start point.
   */

  bool isFull() const;

  /**
   * The left coordinate endpoint of the compulsory part. Based on our
   * semantics, there is actually area drawn into this coordinate.
   */

  const UInt& left() const;
  UInt& left();

  /**
   * The right coordinate endpoint of the compulsory part. Based on
   * our semantics, there is actually NO area drawn into this
   * coordinate. The actual occupation of compulsory part cannot
   * include this point.
   */

  const UInt& right() const;
  UInt& right();
  void print() const;

  /**
   * A range describing the beginning coordinates of the
   * placement. Note that this interval is inclusive. Therefore, the
   * cumulative part's area will be drawn in starting from the
   * endpoint of m_nStart.
   */

  RInterval m_nStart;

  /**
   * A range describing the ending coordinates of the placement. Note
   * that based on our curren tsemantics, no area is drawn actually ON
   * these coordinates. Furthermore, the starting point of this
   * interval is the first point that will NOT contain any area
   * occupied by the compulsory part.
   */

  RInterval m_nEnd;
  const Rectangle* m_pRect;
};

std::ostream& operator<<(std::ostream& os, const Compulsory& c);

#endif // COMPULSORY_H
