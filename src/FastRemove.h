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

#ifndef FASTREMOVE_H
#define FASTREMOVE_H

#include <limits>
#include <vector>

/**
 * This class represents a stack that implements push and pop
 * functionality upon a vector. It allows you to make calls to remove
 * a single square by its size_t index, and it will remember all of
 * your operations for restoration upon the next pop. We enforce that
 * modifications to this class must take the schema of first calling
 * the push function, then adding any new data to the array, followed
 * by calls to the remove function. The pop function will restore
 * edits made by the remove function, and then remove additions to the
 * data. Note that this means you are not allowed to interleave
 * additions and removals.
 */

template <typename T> class FastRemove;

template <typename T>
class FastRemove : public std::vector<T> {

 public:
  std::vector<std::pair<size_t, T> > m_vStack;

  /**
   * First element is the data vector, and second element is the
   * actual stack array.
   */

  std::vector<std::pair<size_t, size_t> > m_vStackSize;

  FastRemove() {}
  ~FastRemove() {}

  void reserve(size_t n) {
    std::vector<T>::reserve(n);
    m_vStack.reserve(n);
    m_vStackSize.reserve(n);
  }

  void clear() {
    std::vector<T>::clear();
    m_vStack.clear();
    m_vStackSize.clear();
  }

  size_t size() const {
    return(std::vector<T>::size());
  }

  void pop_back() {
    std::vector<T>::pop_back();
  }

  T& operator[](size_t n) {
    return(std::vector<T>::operator[](n));
  }

  const T& operator[](size_t n) const {
    return(std::vector<T>::operator[](n));
  }

  T& back() {
    return(std::vector<T>::back());
  }

  const T& back() const {
    return(std::vector<T>::back());
  }

  void push() {
    m_vStackSize.push_back(std::pair<size_t, size_t>(size(), m_vStack.size()));
  }

  void pop() {

    /**
     * Erase any calls to the remove function.
     */

    while(m_vStack.size() > m_vStackSize.back().second) {

      /**
       * Move the element to the back.
       */

      push_back(operator[](m_vStack.back().first));

      /**
       * Write the previously-erased element into its original
       * location.
       */

      operator[](m_vStack.back().first) = m_vStack.back().second;
      m_vStack.pop_back();
    }

    /**
     * Remove additions to the data array.
     */

    while(size() > m_vStackSize.back().first)
      pop_back();

    m_vStackSize.pop_back();
  }

  void remove(size_t n) {

    /**
     * Save the previous value we're going to overwrite.
     */

    m_vStack.push_back(std::pair<size_t, T>(n, operator[](n)));

    /**
     * Move the back element into the erased spot.
     */

    operator[](n) = back();
    pop_back();
  }

  size_t find(const T& t) {
    for(size_t i = 0; i < size(); ++i)
      if(t == operator[](i))
	return(i);
    return(std::numeric_limits<size_t>::max());
  }

  void remove(const T& t) {
    remove(find(t));
  }

  bool operator==(const FastRemove<T>& rhs) const {
    return((std::vector<T>) (*this) == (std::vector<T>) rhs &&
	   m_vStack == rhs.m_vStack &&
	   m_vStackSize == rhs.m_vStackSize);
  }
};

#endif // FASTREMOVE_H
