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

#ifndef COMPRESSIONSTACK_H
#define COMPRESSIONSTACK_H

#include <deque>
#include "CompressionEntry.h"

class CompressionStack;

class CompressionStack {
  friend std::ostream& operator<<(std::ostream& os, const CompressionStack& rhs);

 public:
  CompressionStack() {};
  ~CompressionStack() {};

  void clear() { m_vStack.clear(); };

  void push() { m_vStack.push_back(CompressionEntry()); };

  void pop() { m_vStack.pop_back(); };

  CompressionEntry& top() { return(m_vStack.back()); };

  void overwriting(const Rectangle* s, const Interval& i) {
    if(!m_vStack.back().has(s))
      top()[s] = i;
  };

  void print() const;

  std::deque<CompressionEntry> m_vStack;
};

std::ostream& operator<<(std::ostream& os, const CompressionStack& rhs);

#endif // COMPRESSIONSTACK_H
