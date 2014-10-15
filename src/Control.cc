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
#include "Packer.h"
#include "Parameters.h"

Control::Control() :
  m_bQuit(false) {
}

Control::~Control() {
}

void Control::finished() {
  set_title("RectPack (Finished)");
}

void Control::show() {
  m_VBox.show();
  m_CommandF.show();
  m_CommandL.show();
  m_NodeF.show();
  m_NodeT.show();
  for(int i = 0; i < 3; ++i)
    for(int j = 0; j < 2; ++j)
      m_vData[i][j].show();
  Gtk::Window::show();
}

void Control::initialize(const Packer* pPacker) {

  /**
   * Set up the command line text.
   */

  m_CommandF.set_label("Command Line");
  m_CommandL.set_text(pPacker->m_pParams->m_sCommand);
  m_CommandF.add(m_CommandL);
  m_VBox.pack_start(m_CommandF);
  
  /**
   * Set up the node information table's contents.
   */
  
  m_vData[0][0].set_text("Last Paused:");
  m_vData[1][0].set_text("Current:");
  m_vData[2][0].set_text("Difference:");
  for(int i = 0; i < 3; ++i) {
    m_vData[i][0].set_justify(Gtk::JUSTIFY_RIGHT);
    m_vData[i][1].set_text("0");
    m_vData[i][1].set_justify(Gtk::JUSTIFY_LEFT);
  }

  /**
   * Insert the data into the table.
   */

  m_NodeT.resize(3, 2);
  for(int i = 0; i < 3; ++i)
    for(int j = 0; j < 2; ++j)
      m_NodeT.attach(m_vData[i][j], j, j + 1, i, i + 1);
  m_NodeF.set_label("Nodes");
  m_NodeF.add(m_NodeT);
  m_VBox.pack_start(m_NodeF);

  /**
   * Finally, insert the vertical box.
   */

  add(m_VBox);

  /**
   * Copy the node count status for future access.
   */

  m_pCurrent = &pPacker->m_Nodes;
}

bool Control::on_key_press_event(GdkEventKey* event) {
  if(event->type == GDK_KEY_PRESS) {
    switch(event->keyval) {
    case GDK_Q:
    case GDK_q:
    case GDK_Escape:
      Gtk::Main::quit();
      break;
    default:
      break;
    };
  }
  return(true);
}

void Control::startNodePolling() {
  
}

void Control::stopNodePolling() {
  m_bQuit = true;
  if(m_tNodeUpdater.joinable())
    m_tNodeUpdater.join();
}

void Control::nodePollingThread() {
  while(!m_bQuit) {
    gdk_threads_enter();
    
    gdk_threads_leave();
    usleep(10000);
  };
}

#endif // VIZ
