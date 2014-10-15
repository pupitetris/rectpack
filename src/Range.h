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

#ifndef RANGE_H
#define RANGE_H

#include "Integer.h"
#include <deque>
#include <limits>

class Range {
 public:
  Range();
  Range(UInt nlb, UInt nub);
  ~Range();

  bool operator<(const Range& r) const {
    if(lb() != r.lb())
      return(lb() < r.lb());
    else
      return(ub() < r.ub());
  }

  const Range& operator=(const Range& r) {
    m_vLower = r.m_vLower;
    m_vUpper = r.m_vUpper;
    return(*this);
  }

  const Range& operator|=(const Range& r) {
    lb() = std::min(lb(), r.lb());
    ub() = std::max(ub(), r.ub());
    return(*this);
  }

  const Range& operator&=(const Range& r) {
    lb() = std::max(lb(), r.lb());
    ub() = std::min(lb(), r.ub());
    return(*this);
  }

  void set(UInt n) {
    set(n, n);
  }

  void set(const Range& r) {
    lb() = r.lb();
    ub() = r.ub();
  }

  void set(UInt nlb, UInt nub) {
    lb() = nlb;
    ub() = nub;
  }

  void setInvalid() {
    lb() = std::numeric_limits<UInt>::max();
    ub() = 0;
  }

  void push() {
    m_vUpper.push_back(m_vUpper.back());
    m_vLower.push_back(m_vLower.back());
  }

  void pop() {
    m_vUpper.pop_back();
    m_vLower.pop_back();
  }

  const UInt& ub() const {
    return(m_vUpper.back());
  }

  const UInt& lb() const {
    return(m_vLower.back());
  }

  UInt& ub() {
    return(m_vUpper.back());
  }

  UInt& lb() {
    return(m_vLower.back());
  }

  bool fixed() const {
    return(lb() == ub());
  }

  int size() const {
    return(ub() - lb() + 1);
  }

  bool nonempty() const {
    return(lb() <= ub());
  }

  bool empty() const {
    return(lb() > ub());
  }
  
  bool empty(UInt n) const {
    return(empty(n, n));
  }

  bool empty(UInt nlb, UInt nub) const {
    return(nlb > ub() || nub < lb());
  }

  void intersect(UInt nlb, UInt nub) {
    lb() = std::max(lb(), nlb);
    ub() = std::min(ub(), nub);
  }

  bool constrains(const Range& r) const {
    return(constrains(r.lb(), r.ub()));
  }

  bool constrains(UInt nlb, UInt nub) const {
    return(lb() > nlb || ub() < nub);
  }

  bool left() const {
    return(lb() == 0);
  }

  bool right() const {
    return(lb() != 0);
  }

  void printEmpty() const;
  void print() const;

 private:
  std::deque<UInt> m_vLower;
  std::deque<UInt> m_vUpper;
};

#endif // RANGE_H
