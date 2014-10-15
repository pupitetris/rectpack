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

#ifndef BOUNDINGBOXES_H
#define BOUNDINGBOXES_H

#include "Attempt.h"
#include "HeapBox.h"
#include "NodeCount.h"
#include "PriorityQ.h"
#include "Rational.h"
#include "SubsetSums.h"
#include "SubsetSumsSet.h"
#include "TimeSpec.h"
#include <deque>
#include <set>
#include <string>
#include <vector>
#include <queue>

class BoxDimensions;
class Packer;
class Parameters;

class BoundingBoxes {
 public:
  BoundingBoxes();
  virtual ~BoundingBoxes();
  virtual void initialize(const Parameters* pParams);
  virtual void initialize(Packer* pPacker);
  virtual void initialize(const std::deque<Packer*>& vPackers);
  virtual bool run();
  URational m_nMinArea;
  UInt m_nMaxArea;
  Packer* m_pPacker;

  /**
   * Collect data about the packing attempts.
   */

  virtual std::string solutions() const;
  UInt attempts() const;
  std::set<Attempt> m_sAttempts;
  NodeCount m_Total;
  TimeSpec m_TotalTime;
  TimeSpec m_TotalXTime;
  TimeSpec m_TotalYTime;

  /**
   * Represents all possible widths created by subset sums of all the
   * rectangles.
   */

  SubsetSums m_Widths;

  /**
   * Represents all possible heights created by subset sums of all the
   * rectangles.
   */

  SubsetSums m_Heights;

  /**
   * These subset sums represent limited sets that are based on a
   * subset of the rectangles only. That is, for m_vWidths[i], we only
   * consider subset sums up to and including the rectangle of ID i.
   */

  SubsetSumsSet m_vWidths;
  SubsetSumsSet m_vHeights;

  bool m_bAllIntegralBoxes;
  bool m_bMutexChecking;
  bool m_bConflictLearning;
  bool m_bTopLevelWaste;

 protected:
  void pop(HeapBox& hb);

  /**
   * Performs a single packing attempt on a bounding box as specified
   * in the command line parameters.
   */

  virtual bool singleRun();

  /**
   * Gets the next bounding box we're supposed to try, and queues up
   * the next successive one to try as well. The contents of b prior
   * to this function call are irrelevant. Before doing so, however,
   * it checks to make sure that the queue is actually not empty.
   *
   * @return true if the box was successfully popped, and false
   * otherwise (i.e. the queue was empty).
   */

  virtual bool getNext(HeapBox& b);
  virtual bool pushNext(const HeapBox& hb);
  virtual bool pushNextInt(const HeapBox& hb);
  virtual void printAttempt(const BoxDimensions& b) const;

  /**
   * Enqueues new classes of bounding boxes by incrementing the width
   * and inserting bounding boxes whose heights tightly wrap the
   * current minimum area.
   *
   * @param nMinArea the current lower bound on the area (this value
   * should increase as we continue to test infeasible bounding
   * boxes).
   */

  void enqueueNewWidths(UInt nMinArea);
  void enqueueNewIntWidths(UInt nMinArea);

  PriorityQ m_vBoxes;
  Int m_nPopped;
  const Parameters* m_pParams;

  /**
   * Points to the next width that we should consider for purposes of
   * inserting a new class of boxes.
   */

  SubsetSums::const_iterator m_iWidth;

  /**
   * Functions similarly as m_iWidths except that we are actually
   * dealing with all possible integer values as opposed to subset
   * sums.
   */

  UInt m_nWidth;

 private:
  void lowerBoundSolution();
  void upperBoundSolution();
};

#endif // BOUNDINGBOXES_H
