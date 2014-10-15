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

#ifndef NODEBASE_H
#define NODEBASE_H

#include "VizType.h"
#include <algorithm>

class NodeBase;
class TimeSpec;

class NodeBase {
 protected:
  const NodeBase& operator+=(const NodeBase& src);
  unsigned long long m_vNodes[4];

 public:
  NodeBase();
  NodeBase(const NodeBase& src);
  ~NodeBase();
  void clear();
  unsigned long long get(VizType n) const;
  void set(VizType n, unsigned long long m);
  const NodeBase& operator=(const NodeBase& src);
  bool operator<(const NodeBase& rhs) const;
  bool operator!=(const NodeBase& rhs) const;
  void tick(VizType n) { ++m_vNodes[n]; }
  void tick(VizType n, int m);
  void accumulate(NodeBase& src);
  void decumulate(NodeBase& src);
  unsigned long long total() const;
  void print() const;
  void print(const TimeSpec& t, size_t nCuSP) const;
};

#endif // NODEBASE_H
