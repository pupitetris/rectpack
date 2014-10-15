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

#ifndef VECTORSTACK_H
#define VECTORSTACK_H

#include <vector>
#include <deque>

/**
 * This class represents a vectore augmented with stack
 * capabilities. It differs from the basic stack data structure
 * provided by STL in that save and restore functions are used to
 * remember the current state. Essentially all operations that modify
 * the vector are noted through the use of the update interface, and
 * upon a restoration call, we will undo all of those operations to
 * their previous values. The memory will grow based on the number of
 * operations that have to be remembered. It's imperative that for
 * this to work you modify values with the update call instead of
 * writing directly, or else corruption will occur.
 */

template <typename T> class VectorStack;

template <typename T>
class VectorStack : public std::vector<T> {
 private:
  std::deque<std::pair<size_t, T> > m_vStack;
  std::deque<size_t> m_vStackSize;

 public:
  VectorStack() { }

  ~VectorStack() { }

  void clear() {
    std::vector<T>::clear();
    m_vStack.clear();
    m_vStackSize.clear();
  }
  
  void update(size_t i, const T& n) {
    m_vStack.push_back(std::pair<size_t, T>(i, std::vector<T>::operator[](i)));
    std::vector<T>::operator[](i) = n;
  }

  void save() {
    m_vStackSize.push_back(m_vStack.size());
  }

  void restore() {
    while(m_vStack.size() > m_vStackSize.back()) {
      std::vector<T>::operator[](m_vStack.back().first) = m_vStack.back().second;
      m_vStack.pop_back();
    }
    m_vStackSize.pop_back();
  }
};

#endif // VECTORSTACK_H
