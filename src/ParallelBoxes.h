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

#ifndef PARALLELBOXES_H
#define PARALLELBOXES_H

#include "BoundingBoxes.h"
#include "Integer.h"
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>
#include <vector>

class Parameters;

/**
 * Note that this is very much broken because we did not update the
 * multi-threaded version of our rectangle packing program.
 */

class ParallelBoxes : public BoundingBoxes {
 public:
  ParallelBoxes();
  virtual ~ParallelBoxes();
  virtual void initialize(const Parameters* pParams);
  virtual bool run();
  virtual void set(const std::deque<Packer*>& vPackers);
  virtual void printStarting(const BoxDimensions& b) const;

 private:
  bool boxValid(const BoxDimensions& b) const;
  void runThread();
  void printid() const;
  Packer* getPacker();

  /**
   * Shared data (this class).
   */

  boost::mutex m_Mutex;

  /**
   * All of our worker threads.
   */

  boost::thread* m_pWorkers;

  /**
   * Our local copy of the packers.
   */

  std::deque<Packer*> m_vPackers;

  /**
   * Map from the worker to the packer to use.
   */

  std::map<boost::thread::id, Packer*> m_mAssignment;

  /**
   * A mutex for printing to the console (so that we don't clobber
   * each others' results).
   */

  boost::mutex m_Console;

  /**
   * The condition variable that our threads will use to notify us
   * that they've found a solution.
   */

  boost::condition_variable m_SolutionFound;
  bool m_bSolutionFound;
  UInt m_nMaxArea;
};

#endif // PARALLELBOXES_H
