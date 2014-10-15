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

#ifndef CUMULATIVE_H
#define CUMULATIVE_H

#include "CompulsoryAssign.h"
#include "FastRemove.h"
#include "Integer.h"
#include "RefHeight.h"
#include "SingleStack.h"
#include "ValueStack.h"
#include <map>
#include <vector>

class BoxDimensions;
class Compulsory;
class Packer;
class Parameters;
class IntPlacements;
class Placements;
class Rectangle;

class Cumulative : public std::map<UInt, RefHeight> {
 public:
  Cumulative();
  virtual ~Cumulative();
  void initialize(const Parameters* pParams);
  void initialize(const Packer* pPacker);
  void initialize(const BoxDimensions* pBox);
  bool canFit(const Compulsory& c);

  /**
   * Places the given rectangle in the given location. The function
   * assume that a rectangle previously has its interval assigned.
   */

  void push(const Rectangle* r, const UInt& n);

  /**
   * Inserts the given Compulsory part into our data structures,
   * records this into our stack.
   *
   * @param c the compulsory part context that reflects the value
   * assignment.
   */

  void push(const Compulsory& c);

  /**
   * Inserts the given Compulsory part into our data structures,
   * records this into our stack. This differs from the previous push
   * function in that we expect hints to have already been learned
   * from an immediately preceding call to the canFit function.
   *
   * @param c the compulsory part context that reflects the value
   * assignment.
   */

  void pushHinted(const Compulsory& c);

  /**
   * Unrolls all of the modifications to our data structures since the
   * last call to the push command, including any results of
   * constraint propagation.
   */

  void pop();

  /**
   * Initializes the constraint propagation queue with the endpoints
   * of the compulsory part of the most recently placed rectangle, and
   * performs constraint propagation until we're done or we've run
   * into a constraint violation. Notice that this only happens when
   * the most recent assignment is an interval assignment. If it were
   * a singular value assignment (in which case we would have a NULL
   * pointer as the most recent), then we'll simply call the auxiliary
   * propagation function instead of enqueueing anything new.
   *
   * @return true if we successfully completed constraint propagation
   * without any violations, and false otherwise.
   */

  bool propagate();
  void largestAreaPlacement(UInt& n, const Compulsory*& c);

  /**
   * Returns the smallest difference between two control points along
   * the x-axis.
   */

  UInt epsilon() const;
  void get(Placements& v) const;
  void get(IntPlacements& v) const;
  const std::vector<CompulsoryAssign>& values() const;
  const FastRemove<CompulsoryAssign*>& intervals() const;
  const SingleStack& singles() const;

 private:

  /**
   * Auxiliary function to aid in the pushing call, after
   * hint-handling has been resolved.
   */

  void pushAux(CompulsoryAssign* p);

  /**
   * Removes the current Compulsory class object and rolls it back to
   * the previous one. We expect to actually modify the current data
   * structures so as to revert to the previous one.
   *
   * @param ca a reference to the current CompulsoryAssign class
   * object.
   *
   * @param c the previous Compulsory class object which we will
   * regress to.
   */

  void pop(CompulsoryAssign& ca, const Compulsory& c);

  /**
   * Processes elements from the queue that represents the left
   * missing compulsory parts.
   *
   * @return 0 if no updates were performed, 1 if at least one update
   * was performed, and -1 if there was a constraint violation.
   */

  int propagateLeft(CompulsoryAssign* p);

  /**
   * Processes elements from the queue that represents the right
   * missing compulsory parts.
   *
   * @return 0 if no updates were performed, 1 if at least one update
   * was performed, and -1 if there was a constraint violation.
   */

  int propagateRight(CompulsoryAssign* p);

  /**
   * After constraining the domain, the compulsory part may grow to
   * the left. This function updates the cumulative profile to reflect
   * this growth.
   *
   * @param iCurrent an iterator pointing to the current left endpoint
   * of the compulsory part.
   *
   * @param nDest the final value of the left endpoint that we should
   * grow the compulsory part to.
   *
   * @param iHint an iterator pointing to a location close to where
   * the eventual location of the left endpoint should be.
   *
   * @param nHeight the height of the rectangle that will be added to
   * the compulsory profile along the left of the current compulsory
   * part.
   */

  void extendL(iterator& iCurrent, const UInt& nDest,
	       iterator& iHint, const UInt& nHeight);

  /**
   * After constraining the domain, the compulsory part may grow to
   * the left. This function updates the cumulative profile to reflect
   * this growth.
   *
   * @param iCurrent an iterator pointing to the current left endpoint
   * of the compulsory part.
   *
   * @param nDest the final value of the left endpoint that we should
   * grow the compulsory part to.
   *
   * @param nHeight the height of the rectangle that will be added to
   * the compulsory profile along the left of the current compulsory
   * part.
   */

  void extendL(iterator& iCurrent, const UInt& nDest,
	       const UInt& nHeight);

  /**
   * After constraining the domain, the compulsory part may grow to
   * the right. This function updates the cumulative profile to
   * reflect this growth.
   *
   * @param iCurrent an iterator pointing to the current right
   * endpoint of the compulsory part.
   *
   * @param nDest the final value of the right endpoint that we should
   * grow the compulsory part to.
   *
   * @param iHint an iterator pointing to a location close to where
   * the eventual location of the right endpoint should be.
   *
   * @param nHeight the height of the rectangle that will be added to
   * the compulsory profile along the right of the current compulsory
   * part.
   */

  void extendR(iterator& iCurrent, const UInt& nDest,
	       iterator& iHint, const UInt& nHeight);

  /**
   * After constraining the domain, the compulsory part may grow to
   * the right. This function updates the cumulative profile to
   * reflect this growth.
   *
   * @param iCurrent an iterator pointing to the current right
   * endpoint of the compulsory part.
   *
   * @param nDest the final value of the right endpoint that we should
   * grow the compulsory part to.
   *
   * @param nHeight the height of the rectangle that will be added to
   * the compulsory profile along the right of the current compulsory
   * part.
   */

  void extendR(iterator& iCurrent, const UInt& nDest,
	       const UInt& nHeight);
  
  /**
   * Decrements the reference counter at the given iterator, and
   * removes the object if the reference count is 0.
   *
   * @param i an iterator pointing to the object to dereference.
   *
   * @return true if the reference count is 0 and the object is
   * removed, and false otherwise.
   */

  bool dereference(const iterator& i);

  /**
   * Given a starting iterator iHint, iterates over the control points
   * up to j, checking all intermediary points to ensure they are not
   * greater than the maximum allowable height. This function attempts
   * to check to see if we can extend the compulsory part to the
   * right.
   *
   * @param iHint the iterator from which we should start the search,
   * the current right endpoint of the compulsory part. This point is
   * assumed to be open. If it is possible to extend the compulsory
   * part, then this parameter also will return a hint to where the
   * new insertion should be. Otherwise, it will point to the control
   * point at which we cannot accommodate the given height.
   *
   * @param j the value up to which we expect to extend the right
   * endpoint of our compulsory part (exclusive).
   *
   * @param nMaxHeight the maximum allowable height we enforce as we
   * scan forward.
   *
   * @return true if it is possible to extend the compulsory part
   * forward, and false otherwise.
   */

  bool canFitR(iterator& iHint, const UInt& j,
	       const UInt& nMaxHeight);

  /**
   * Given a starting iterator iHint, iterates over the control points
   * down to j, checking all intermediary points to ensure they are
   * not greater than the maximum allowable height. This function
   * attempts to check to see if we can still extend the compulsory
   * part to the left.
   *
   * @param iHint the iterator from which we should start the search,
   * the current left endpoint of the compulsory part. This point is
   * assumed to be closed. If it is possible to extend the compulsory
   * part, then this parameter also will return a hint to where the
   * new insertion should come after (or at). Otherwise, it will point
   * to the control point at which we cannot accommodate the given
   * height.
   *
   * @param j the value down to which we expect to extend the left
   * endpoint of our compulsory part.
   *
   * @param nMaxHeight the maximum allowable height we enforce as we
   * scan backward.
   *
   * @return true if it is possible to extend the compulsory part
   * backward, and false otherwise.
   */

  bool canFitL(iterator& iHint, const UInt& j,
	       const UInt& nMaxHeight);

  /**
   * Given a point n, find all intervals that contain that point from
   * the set of open intervals, and enqueue them into the appropriate
   * queue.
   *
   * @param n a coordinate point.
   */

  void enqueueR(const UInt& n);

  /**
   * Given a point n, find all intervals that contain that point from
   * the set of open intervals, and enqueue them into the appropriate
   * queue.
   *
   * @param n a coordinate point.
   */

  void enqueueL(const UInt& n);

  /**
   * Both the UInt as well as the RefHeight are required to
   * insert a value pair, so we are simply assuming default
   * construction for RefHeight by providing these overloaded
   * functions.
   */

  std::pair<iterator, bool> insert(const key_type& x);
  iterator insert(iterator pos, const key_type& x);

  /**
   * Prints a graphical representation of the cumulative solution.
   */

 public:
  void print() const;

 private:
  void print(const_iterator j) const;
  void print(iterator j) const;
  void printAux(const_iterator j) const;
  bool valid() const;
  bool valid(const iterator& j) const;
  bool valid(const const_iterator& j) const;
  bool validAux(const const_iterator& j) const;

  /**
   * Stores the current, constraint-propagated values for each
   * rectangle.
   */

  std::vector<CompulsoryAssign> m_vValues;

  /**
   * A set that tracks the rectangles that need their compulsory parts
   * checked. That is, these rectangles still have missing compulsory
   * strips and need to be checked during constraint propagation.
   */

  FastRemove<CompulsoryAssign*> m_vCompulsory;


  /**
   * A stack that tracks the current ste of CompulsoryAssign class
   * objects representing compulsory parts that have actually either
   * been placed or propagated to singular values.
   */

  SingleStack m_vSingular;

  /**
   * The amount of height that cannot be exceeded in order for the
   * given rectangle to fit. This is precomputed for efficiency.
   */

  const Packer* m_pPacker;

  /**
   * Precomputed values that track the maximum cumulative height
   * allowed given a rectangle has not been rotated, and in order for
   * it to still fit within the current bounding box.
   */

  std::vector<UInt> m_vMaxHeight;

  /**
   * Precomputed values that track the maximum cumulative height
   * allowed given a rectangle has been rotated, and in order for it
   * to still fit within the current bounding box.
   */

  std::vector<UInt> m_vRMaxHeight;

  /**
   * Our undo stack. This one in particular is used to store, for a
   * given stack frame, values prior to changes enforced by constraint
   * propagation. It makes value unrolling easier.
   */

  ValueStack m_vStack;

  /**
   * This particular stack tracks assignments that we've made when
   * placing compulsory parts. Saving this not only aids in debugging
   * but also allows us to recall the necessary information for
   * removing the compulsory part when popping the value assignment.
   */

  std::vector<CompulsoryAssign*> m_vCStack;
};

#endif // CUMULATIVE_H
