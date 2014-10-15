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

#ifndef MAINVIZ_H
#define MAINVIZ_H

#include "MainBase.h"
#include "SearchControl.h"
#include "VizDrawing.h"
#include <gtkmm.h>

class Component;
class Control;
class VizWindow;

class MainViz : public MainBase {

 public:
  MainViz();
  ~MainViz();
  void initialize();
  void run();

 private:
  void runAux();
  void initWindows();
  void startDrawing();
  void stopDrawing();

  Gtk::Main* m_pMain;
  Control* m_pControl;
  SearchControl m_SControl;
  std::vector<VizWindow*> m_vWindows;
  bool m_bQuit;
  float m_nAspectRatio;

  static bool m_bGtkInitialized;
};

#endif // MAINVIZ_H

#endif // VIZ
