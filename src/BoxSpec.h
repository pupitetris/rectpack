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

#ifndef BOXSPEC_H
#define BOXSPEC_H

#include "Rational.h"
#include <iostream>
#include <string>

class BoxSpec;

/**
 * This class represents the logic and data that tracks the minimum
 * and maximum box dimensions. It also remembers certain area
 * information to facilitate this computation. It should start off the
 * most unconstrained (allowing the widest amount of boxes), and
 * slowly be constrained as the search deepens in the tree, and
 * perhaps as the search progresses as well.
 */

class BoxSpec {
  friend std::ostream& operator<<(std::ostream& os, const BoxSpec& b);

 public:
  BoxSpec();
  BoxSpec(const BoxSpec& b);
  ~BoxSpec();

  /**
   * We'll accept specifications for the box bounds such as
   * [w,W]x[h,H]. You can indicate unboundedness by a *. For example,
   * [*,*]x[h,H] or *x[h,H] means unbounded width.
   */

  bool load(const std::string& s);
  const BoxSpec& operator=(const BoxSpec& b);
  void rescale(const URational& nScale);
  void clear();
  URational m_nWidth;
  URational m_nHeight;
  void print() const;
};

std::ostream& operator<<(std::ostream& os, const BoxSpec& b);

#endif // BOXSPEC_H
