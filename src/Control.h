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

#ifndef CONTROL_H
#define CONTROL_H

#include "NodeCount.h"
#include <boost/thread.hpp>
#include <gtkmm.h>
#include <vector>

class Packer;

class Control : public Gtk::Window {
 public:
  Control();
  ~Control();
  bool on_key_press_event(GdkEventKey* event);
  void finished();
  void initialize(const Packer* pPacker);
  void startNodePolling();
  void stopNodePolling();
  void show();

 private:
  boost::thread m_tNodeUpdater;
  void nodePollingThread();
  const NodeCount* m_pCurrent;

  bool m_bQuit;
  Gtk::VBox m_VBox;
  Gtk::Frame m_CommandF;
  Gtk::Label m_CommandL;
  Gtk::Frame m_NodeF;
  Gtk::Table m_NodeT;
  Gtk::Label m_vData[3][2];
};

#endif // CONTROL_H

#endif // VIZ
