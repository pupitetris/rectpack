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

#ifndef VIZWINDOW_H
#define VIZWINDOW_H

#include <gtkmm.h>
#include <string>
#include "NodeStatus.h"
#include "VizDrawing.h"
#include "VizType.h"

class GridViz;
class SearchControl;

class VizWindow : public Gtk::Window {
 public:
  VizWindow(Packer* pPacker, SearchControl* pControl, VizType nVizType);
  ~VizWindow();
  bool on_key_press_event(GdkEventKey* event);
  void resize(float nAspectRatio);
  void relabel(const std::string& s);
  void startDrawing();
  void stopDrawing();

 private:

  /**
   * Text printout of the specified bin.
   */

  void printBins(VizType n);
  void draw();
  bool updateGranularity(guint n);
  void printKeys() const;
  Packer* m_pPacker;
  VizType m_nType;
  bool m_bQuit;

  /**
   * The vertical packing box.
   */

  Gtk::VBox m_Box;

  /**
   * The aspect frame.
   */

  float m_nAspectRatio;
  Gtk::AspectFrame m_Frame;

  /**
   * The drawing area.
   */

  boost::thread m_tDrawing;
  VizDrawing m_Drawing;

  /**
   * The status bar.
   */

  bool m_bStatus;
  NodeStatus m_Status;
  SearchControl* m_pControl;
};

#endif // VIZWINDOW_H

#endif // VIZ
