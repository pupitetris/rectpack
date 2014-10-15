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

#include "Control.h"
#include "GridViz.h"
#include "MainViz.h"
#include "Packer.h"
#include "VizDrawing.h"
#include "VizWindow.h"
#include <boost/bind.hpp>
#include <boost/thread.hpp>

bool MainViz::m_bGtkInitialized = false;

MainViz::MainViz() :
  m_pMain(NULL),
  m_pControl(NULL),
  m_bQuit(false),
  m_nAspectRatio(1.0) {
}

MainViz::~MainViz() {
 if(m_pControl) delete m_pControl;
 while(!m_vWindows.empty()) {
   delete m_vWindows.back();
   m_vWindows.pop_back();
 }
 if(m_pMain) delete m_pMain;
}

void MainViz::initialize() {
  if(!m_bGtkInitialized) {
    m_bGtkInitialized = true;
    if(!Glib::thread_supported())
      Glib::thread_init();
    gdk_threads_init();
  }

  MainBase::initialize();

  /**
   * Instantiate our windows.
   */

  if(!m_pMain)
    m_pMain = new Gtk::Main(m_Params.m_nArgc, m_Params.m_pArgv);
}

void MainViz::run() {

  /**
   * Give each of the visualization data structures a handle to the
   * search control.
   */

  m_vPackers.front()->set(&m_SControl);
  m_SControl.set(&m_vPackers.front()->m_State);

  /**
   * Set up the windows necessary.
   */

  initWindows();

  /**
   * Start the drawing threads.
   */

  startDrawing();

  /**
   * Start the computation thread.
   */
  
  boost::thread tComputation(boost::bind(&MainViz::runAux, this));

  /**
   * Start the GUI loop.
   */

  gdk_threads_enter();
  Gtk::Main::run(*m_pControl);
  gdk_threads_leave();

  /**
   * Stop the drawing threads.
   */

  stopDrawing();

  /**
   * Stop the computation loop.
   */

  tComputation.interrupt();
  if(tComputation.joinable())
    tComputation.join();
}

void MainViz::initWindows() {

  /**
   * Instantiate the main GUI control.
   */

  m_pControl = new Control();
  m_pControl->initialize(m_vPackers.front());
  m_pControl->show();

  /**
   * Instantiate the visualization windows.
   */

  m_vWindows.clear();
  m_vWindows.resize(4, NULL);
  if(m_vPackers.front()->m_State.getBins(YI))
    m_vWindows[0] = new VizWindow(m_vPackers.front(), &m_SControl, YI);
  if(m_vPackers.front()->m_State.getBins(YF))
    m_vWindows[1] = new VizWindow(m_vPackers.front(), &m_SControl, YF);
  if(m_vPackers.front()->m_State.getGrid(XI))
    m_vWindows[2] = new VizWindow(m_vPackers.front(), &m_SControl, XI);
  if(m_vPackers.front()->m_State.getGrid(XF))
    m_vWindows[3] = new VizWindow(m_vPackers.front(), &m_SControl, XF);
}

void MainViz::runAux() {
  MainBase::run();
  gdk_threads_enter();
  m_pControl->finished();
  gdk_threads_leave();
}

void MainViz::stopDrawing() {
  for(size_t i = 0; i < m_vWindows.size(); ++i)
    if(m_vWindows[i])
      m_vWindows[i]->stopDrawing();
}

void MainViz::startDrawing() {
  for(size_t i = 0; i < m_vWindows.size(); ++i)
    if(m_vWindows[i])
      m_vWindows[i]->startDrawing();
}

#endif // VIZ
