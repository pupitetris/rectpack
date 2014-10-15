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

#ifndef VALUESTACK_H
#define VALUESTACK_H

#include "Compulsory.h"
#include <boost/dynamic_bitset.hpp>
#include <vector>

class BoxDimensions;
class Parameters;

/**
 * This class represents a stack that only accepts pushes of values
 * that haven't already been pushed. We use rectangle ID to determine
 * uniqueness.
 */

class ValueStack {
 public:
  ValueStack();
  virtual ~ValueStack();

  /**
   * Initializes the data structure by preallocating the maximum
   * amount of memory that we expect to be using.
   *
   * @param pParams a pointer to the Parameters class object.
   */

  void initialize(const Parameters* pParams);
  void initialize(const BoxDimensions* pBox);

  /**
   * Signals to the class object that we are starting a new stack
   * frame.
   */

  void save();

  /**
   * Signals to the class object that we're reverting to the previous
   * stack frame. Note that this function must only be called when the
   * class object reports that it is "empty", and the caller must
   * never pop the stack beyond what was previously pushed.
   */

  void restore();

  /**
   * Returns the next value that should be restored.
   */

  const Compulsory& top() const;

  /**
   * Returns whether or not all values have been restored in the
   * current stack frame.
   */

  bool empty() const;

  /**
   * Attempts to save the given value. This will only occur if this
   * value had not previously been saved.
   */

  void push(const Compulsory& c);

  /**
   * Pops the top saved value off of the stack -- this should be
   * called during the restoration process.
   */

  void pop();

 private:
  void print() const;

  std::vector<Compulsory> m_vStack;
  std::vector<boost::dynamic_bitset<> > m_vInserted;
  std::vector<size_t> m_vSize;
  const Parameters* m_pParams;
};

#endif // VALUESTACK_H
