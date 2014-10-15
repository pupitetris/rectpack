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

#ifdef VIZ

#ifndef SEARCHCONTROL_H
#define SEARCHCONTROL_H

#include <glib.h>
#include <boost/thread.hpp>
#include "VizType.h"

class Component;
class LockableState;

class SearchControl {
 public:
  enum Directive {
    PLAY,
    PAUSE,
    PAUSE_AFTER_ADD,
    PAUSE_AFTER_DEL,
    PAUSE_NEXT_DEPTH,
    PAUSE_AFTER_CLEAR,
  };

  SearchControl();
  ~SearchControl();

  /**
   * This should be called after the given grid's pointer has been
   * SHARED LOCKED and the grid data structure also has been SHARED
   * LOCKED and after the square s has been already added to the
   * stack. These functions will then lock the current data structure,
   * perform its updates, and unlock everything prior to waiting on
   * the condition variable.
   */

  void postAdd(VizType nType);
  void postDel(VizType nType);
  void postClear(VizType nType);
  void postClear();
  void postCompress(VizType nType);
  void set(guint nKey, VizType nType);
  void set(LockableState* pState);
  void toggleCompression();

 private:

  /**
   * It is assumed that by calling this function, we currently have a
   * shared lock on the container pointer, a shared lock on the
   * container, and a unique lock on our current data structure. It
   * will block waiting until we get a signal.
   */

  void block(VizType nType, boost::unique_lock<boost::mutex>& lock);
  LockableState* m_pState;
  VizType m_nType;
  Directive m_nCondition;
  int m_nDepth;
  bool m_bBlockOnCompression;
  boost::mutex m_Mutex;

  bool m_bContinue;
  boost::condition_variable m_Continue;

 public:
  bool m_bRunAgain;
};

#endif // SEARCHCONTROL_H

#endif // VIZ
