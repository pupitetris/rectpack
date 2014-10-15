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

#include "BinsViz.h"
#include "GridViz.h"
#include "Parameters.h"
#include "SearchControl.h"
#include "VizWindow.h"
#include <boost/bind.hpp>

VizWindow::VizWindow(Packer* pPacker, SearchControl* pControl,
		     VizType nVizType) :
  m_pPacker(pPacker),
  m_nType(nVizType),
  m_bQuit(false),
  m_nAspectRatio(1.0),
  m_bStatus(true),
  m_pControl(pControl) {
  Gtk::Window::set_size_request(100, 100);
  Gtk::Window::set_default_size(pPacker->m_pParams->m_nGeometryWidth,
				pPacker->m_pParams->m_nGeometryHeight);
  m_Drawing.show();
  m_Frame.add(m_Drawing);
  m_Frame.show();
  m_Status.show();
  m_Box.pack_start(m_Frame);
  m_Box.pack_start(m_Status, Gtk::PACK_SHRINK);
  m_Box.show();
  Gtk::Window::add(m_Box);
  Gtk::Window::show();
  m_Drawing.initialize();

  switch(nVizType) {
  case YI:
    set_title("Y Interval");
    Gtk::Window::move(200, 0);
    break;
  case YF:
    set_title("Y Fixed");
    Gtk::Window::move(200, 300);
    break;
  case XI:
    set_title("X Interval");
    break;
  case XF:
    set_title("X Fixed");
    Gtk::Window::move(200, 600);
    break;
  default:
    break;
  };
}

VizWindow::~VizWindow() {
  stopDrawing();
}

void VizWindow::resize(float nAspectRatio) {
  gdk_threads_enter();
  m_Frame.set(0.5, 0.5, nAspectRatio, false);
  gdk_threads_leave();
}

void VizWindow::relabel(const std::string& s) {
  gdk_threads_enter();
  m_Frame.set_label(s);
  gdk_threads_leave();
}

void VizWindow::startDrawing() {
  boost::thread t(boost::bind(&VizWindow::draw, this));
  m_tDrawing.swap(t);
}

void VizWindow::stopDrawing() {
  m_bQuit = true;
  if(m_tDrawing.joinable())
    m_tDrawing.join();
}

void VizWindow::draw() {
  while(!m_bQuit) {
    if(m_pPacker->m_bInitialized) {
      m_pPacker->m_BoxMutex.lock_shared();
      BoxDimensions b = m_pPacker->m_Box;
      m_pPacker->m_BoxMutex.unlock_shared();
      float nAspectRatio(b.ratio().get_f());
      if(m_nAspectRatio != nAspectRatio) {
	m_nAspectRatio = nAspectRatio;
	resize(m_nAspectRatio);
	relabel(b.toString());
      }

      GridViz* pGrid(NULL);
      BinsViz* pBins(NULL);
      switch(m_nType) {
      case YI:
	if((pBins = m_pPacker->m_State.getBins(YI)) &&
	   pBins->m_bInitialized) {
	  m_Status.set(m_pPacker->m_Nodes.get(YI));
	  m_Drawing.drawYI(pBins);
	}
	break;
      case YF:
	if((pBins = m_pPacker->m_State.getBins(YF)) &&
	   pBins->m_bInitialized) {
	  m_Status.set(m_pPacker->m_Nodes.get(YF));
	  m_Drawing.drawYF(pBins);
	}
	break;
      case XI:
	if((pGrid = m_pPacker->m_State.getGrid(XI)) &&
	   pGrid->m_bInitialized) {
	  m_Status.set(m_pPacker->m_Nodes.get(XI));
	  m_Drawing.drawXI(pGrid);
	}
	break;
      case XF:
	if((pGrid = m_pPacker->m_State.getGrid(XF)) &&
	   pGrid->m_bInitialized) {
	  m_Status.set(m_pPacker->m_Nodes.get(XF));
	  m_Drawing.drawXF(pGrid);
	}
	break;
      default:
	break;
      };

      /**
       * Update the node count in the status bar.
       */

      if(m_bStatus) {
	gdk_threads_enter();
	m_Status.redraw();
	gdk_threads_leave();
      }
    }
    usleep(10000);
  }
}

bool VizWindow::on_key_press_event(GdkEventKey* event) {
  if(event->type == GDK_KEY_PRESS) {
    if(m_Drawing.updateGranularity(event->keyval))
      return(true);
    switch(event->keyval) {
    case GDK_C:
    case GDK_c:
      m_pControl->toggleCompression();
      break;
    case GDK_D:
    case GDK_d:
      m_Drawing.m_bDomain = !m_Drawing.m_bDomain;
      m_Drawing.m_bDirty = true;
      break;
    case GDK_F:
      ++m_Drawing.m_nFontSize;
      m_Drawing.m_bDirty = true;
      break;
    case GDK_f:
      if(m_Drawing.m_nFontSize > 2) {
	--m_Drawing.m_nFontSize;
	m_Drawing.m_bDirty = true;
      }
      break;
    case GDK_H:
    case GDK_h:
      m_Drawing.m_bFlipHorz = !m_Drawing.m_bFlipHorz;
      m_Drawing.m_bDirty = true;
      break;
    case GDK_L:
      m_Drawing.m_nLineWidth += 0.1;
      m_Drawing.m_bDirty = true;
      break;
    case GDK_l:
      if(m_Drawing.m_nLineWidth > 0.2) {
	m_Drawing.m_nLineWidth -= 0.1;
	m_Drawing.m_bDirty = true;
      }
      break;
    case GDK_M:
    case GDK_m:
      m_Drawing.m_bMonochrome = !m_Drawing.m_bMonochrome;
      m_Drawing.m_bDirty = true;
      break;
    case GDK_P:
    case GDK_p:
      printBins(m_nType);
      break;
    case GDK_Q:
    case GDK_q:
    case GDK_Escape:
      Gtk::Main::quit();
      break;
    case GDK_R:
    case GDK_r:
      m_pControl->m_bRunAgain = true;
      Gtk::Main::quit();
      break;
    case GDK_s:
      m_Drawing.m_bSquareSizes = !m_Drawing.m_bSquareSizes;
      m_Drawing.m_bDirty = true;
      break;
    case GDK_V:
    case GDK_v:
      m_Drawing.m_bFlipVert = !m_Drawing.m_bFlipVert;
      m_Drawing.m_bDirty = true;
      break;
    case GDK_W:
      m_Drawing.m_bWritePng = true;
      m_Drawing.m_nPngIndex = 0;
      break;
    case GDK_w:
      m_Drawing.m_bWritePng = false;
      break;
    case GDK_F1:
    case GDK_question:
      printKeys();
      break;
    default:
      m_Status.setPrevious();
      m_pControl->set(event->keyval, m_nType);
      break;
    };
  }
  return(true);
}

void VizWindow::printBins(VizType n) {
  switch(n) {
  case YI:
    m_pPacker->m_State.getBins(n)->print();
    break;
  case YF:
    m_pPacker->m_State.getBins(n)->print();
    break;
  default:
    break;
  };
}

void VizWindow::printKeys() const {
  std::cout << std::endl
	    << "Recognized keyboard input:" << std::endl
	    << std::endl
	    << "c     Toggles stopping constraint propagation." << std::endl
	    << "F/f   In/Decreases the size of the font." << std::endl
	    << "h     Flips the graphic horizontally (mirror along vertical axis)." << std::endl
	    << "L/l   In/Decreases line width." << std::endl
	    << "m     Toggles color/monochrome." << std::endl
	    << "p     Prints the contents of the current 1D bins vector." << std::endl
	    << "q/ESC Quits the program." << std::endl
	    << "s     Draws in square/rectangle sizes into the graphic." << std::endl
	    << "v     Flips the graphic vertically (mirror along horizontal axis)." << std::endl
	    << "W/w   Begin/Stop writing frames of png files." << std::endl
	    << "0..9  Toggles the granularity of the grid." << std::endl
	    << "?/F1  Prints this message." << std::endl;
}

#endif // VIZ
