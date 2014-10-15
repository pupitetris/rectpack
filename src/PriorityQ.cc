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

#include "PriorityQ.h"
#include <algorithm>

PriorityQ::PriorityQ() {
}

PriorityQ::~PriorityQ() {
}

const HeapBox& PriorityQ::top() const {
  return(front());
}

void PriorityQ::pop() {
  std::pop_heap(begin(), end(), m_Greater);
  pop_back();
}

void PriorityQ::push(const HeapBox& hb) {
  push_back(hb);
  std::push_heap(begin(), end(), m_Greater);
}

void PriorityQ::clear() {
  std::vector<HeapBox>::clear();
}

void PriorityQ::print() const {
  for(const_iterator i = begin(); i != end(); ++i)
    std::cout << i->m_Box << " ";
  std::cout << std::endl;
}

size_t PriorityQ::size() const {
  return(std::vector<HeapBox>::size());
}

bool PriorityQ::empty() const {
  return(std::vector<HeapBox>::empty());
}
