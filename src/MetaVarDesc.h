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

#ifndef METAVARDESC_H
#define METAVARDESC_H

#include "Integer.h"
#include <iostream>

class Rectangle;

class MetaVarDesc {
  friend std::ostream& operator<<(std::ostream& os, const MetaVarDesc& rhs);

 public:
  MetaVarDesc();
  virtual ~MetaVarDesc();
  void initialize(const Rectangle* r1, const Rectangle* r2);
  void print() const;
  /**
   * The variable is defined by the two rectangle constraints that it
   * represents.
   */

  const Rectangle* m_pRect1;
  const Rectangle* m_pRect2;

  /**
   * Precomputed score for purposes of variable ordering. According to
   * Moffitt and Pollack's definition, this is simply the minimum area
   * of the two rectangles, or the maximum area of the two rectangles.
   */

  UInt m_nMinArea;
  UInt m_nMaxArea;
};

std::ostream& operator<<(std::ostream& os, const MetaVarDesc& rhs);

#endif // METAVARDESC_H
