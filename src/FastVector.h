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

#ifndef FASTVECTOR_H
#define FASTVECTOR_H

#include <boost/dynamic_bitset.hpp>

/**
 * This class implements a backtrackable vector that keeps track of
 * what modifications were performed on it. In order to meet its speed
 * guarantees, it must be preallocated with the appropriate size prior
 * to the search.
 */

template <typename T> class FastVector;

template <typename T>
class FastVector : public std::vector<T> {
 private:

  typedef std::pair<size_t, boost::dynamic_bitset<> > StackEntry;

  /**
   * Saves the previous values before making the changes.
   */

  std::deque<std::pair<size_t, T> > m_vValues;

  /**
   * Remembers the size of our stack so that we know how many values
   * to pop off of it.
   */

  std::deque<StackEntry> m_vStack;

 public:

  FastVector() {}

  FastVector(size_t i, const T& t) :
    std::vector<T>(i, t) {
    push();
  }
    
  ~FastVector() {}

  /**
   * We provide only a single function for reserving space. This is
   * because when we allocate our data structures, the sizes between
   * these bins along with the maximum seach depth must be consistent
   * with one another.
   */

  void reserve(size_t n) {
    std::vector<T>::reserve(n);
  }

  size_t size() const {
    return(std::vector<T>::size());
  }

  void push() {
    m_vStack.push_back(StackEntry(m_vValues.size(),
				  boost::dynamic_bitset<>(size())));
  }

  void resize(size_t n) {
    std::vector<T>::resize(n);
    push();
  }

  void resize(size_t n, const T& t) {
    std::vector<T>::resize(n, t);
    push();
  }

  void clear() {
    std::vector<T>::clear();
    m_vValues.clear();
    m_vStack.clear();
  }

  void pop() {
    while(m_vValues.size() > m_vStack.back().first) {
      operator[](m_vValues.back().first) = m_vValues.back().second;
      m_vValues.pop_back();
    }
    m_vStack.pop_back();
  }

  const T& get(size_t n) const {
    return(std::vector<T>::operator[](n));
  }

  void set(size_t n, const T& t) {
    if(!m_vStack.back().second.test(n)) {
      m_vValues.push_back(std::pair<size_t, T>(n, std::vector<T>::operator[](n)));
      m_vStack.back().second.set(n);     
    }
    operator[](n) = t;
  }

  void setBack(const T& t) {
    if(!m_vStack.back().second.test(std::vector<T>::size() - 1)) {
      m_vValues.push_back(std::pair<size_t, T>(std::vector<T>::size() - 1,
					       std::vector<T>::back()));
      m_vStack.back().second.set(std::vector<T>::size() - 1);
    }
    m_vStack.back() = t;
  }

  bool operator==(const FastVector<T>& rhs) const {
    return((std::vector<T>) (*this) == (std::vector<T>) rhs &&
	   m_vValues == rhs.m_vValues &&
	   m_vStack == rhs.m_vStack);
  }
};

#endif // FASTVECTOR_H
