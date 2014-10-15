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

#ifndef NODESTATUS_H
#define NODESTATUS_H

#include <boost/thread/mutex.hpp>
#include <gtkmm.h>

class NodeStatus : public Gtk::Statusbar {
 public:
  NodeStatus();
  ~NodeStatus();

  /**
   * Remembers the current node count as the reference point.
   */

  void setPrevious();

  /**
   * Sets the current node count to the given number. This should be
   * called by the drawing thread outside of the GDK lock.
   */

  void set(const unsigned long long& n);

  /**
   * This should be called by the drawing thread to actually update
   * stuff. Notice that we assume that the GDK thread lock has been
   * already called.
   */

  void redraw();

  char m_pBuffer[256];
  bool m_bDirty;
  unsigned long long m_nCurrent;
  unsigned long long m_nPrevious;
  boost::mutex m_Mutex;
};

#endif // NODESTATUS_H

#endif // VIZ
