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

#ifndef COMPULSORYASSIGN_H
#define COMPULSORYASSIGN_H

#include "Compulsory.h"
#include "Integer.h"
#include "RefHeight.h"
#include <map>
#include <ostream>

class CompulsoryAssign;

class CompulsoryAssign {
  friend std::ostream& operator<<(std::ostream& os,
				  const CompulsoryAssign& ca);

 public:
  CompulsoryAssign();
  virtual ~CompulsoryAssign();
  void print() const;
  const UInt& id() const;
  Compulsory m_nValue;

  /**
   * Points to the left side of the compulsory part. This point will
   * have to accommodate the area of the compulsory part.
   */

  std::map<UInt, RefHeight>::iterator m_iLeft;

  /**
   * Points to the right side of the compulsory part. This point does
   * NOT accommodate the area of the compulsory part, but instead
   * represents almost an open endpoint.
   */

  std::map<UInt, RefHeight>::iterator m_iRight;
};

std::ostream& operator<<(std::ostream& os,
			 const CompulsoryAssign& ca);

#endif // COMPULSORYASSIGN_H
