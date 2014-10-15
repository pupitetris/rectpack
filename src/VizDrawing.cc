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
#include "VizDrawing.h"
#include <gdkmm/pixbuf.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <queue>

#define VIZDRAWING_GRID_ALPHA 0.3

VizDrawing::VizDrawing() :
  m_bDirty(false),
  m_nGranularity(0),
  m_bSquareSizes(false),
  m_bDomain(true),
  m_bMonochrome(false),
  m_bFlipHorz(false),
  m_bFlipVert(true),
  m_nFontSize(8),
  m_nLineWidth(0.5),
  m_bWritePng(false),
  m_nPngIndex(0),
  m_vDrawn(64, false) {
  Gtk::Widget::set_app_paintable(true);
  Gtk::Widget::set_double_buffered(false);
}

VizDrawing::~VizDrawing() {
}

void VizDrawing::initialize() {
  initPixmap();
}

bool VizDrawing::on_expose_event(GdkEventExpose* event) {
  Glib::RefPtr<Gdk::Window> window = Gtk::Widget::get_window();
  Glib::RefPtr<Gtk::Style> style = Gtk::Widget::get_style();
  window->draw_drawable(style->get_fg_gc(Gtk::Widget::get_state()),
			m_Pixmap, event->area.x, event->area.y,
			event->area.x, event->area.y,
			event->area.width, event->area.height);
  return true;
}

bool VizDrawing::on_configure_event(GdkEventConfigure* event) {
  int oldw, oldh;
  if(!m_Pixmap) initPixmap();
  m_Pixmap->get_size(oldw, oldh);
  //make our selves a properly sized pixmap if our window has been resized
  if (oldw != event->width || oldh != event->height){
    Glib::RefPtr<Gdk::Pixmap> tmppixmap =
      Gdk::Pixmap::create(Gtk::Widget::get_window(), event->width,  event->height, -1);
    //copy the contents of the old pixmap to the new pixmap.  This keeps ugly uninitialized
    //pixmaps from being painted upon resize
    int minw = oldw, minh = oldh;
    if( event->width < minw ){ minw =  event->width; }
    if( event->height < minh ){ minh =  event->height; }
    Glib::RefPtr<Gtk::Style> style = Gtk::Widget::get_style();
    tmppixmap->draw_drawable(style->get_fg_gc(Gtk::Widget::get_state()),
			     m_Pixmap, 0, 0, 0, 0, minw, minh);
    //we're done with our old pixmap, so we can get rid of it and replace it with our properly-sized one.
    m_Pixmap = tmppixmap;
    m_bDirty = true;
  }
  return true;
}

void VizDrawing::initPixmap() {
  if(!m_Pixmap) {
    int w, h;
    Gtk::Widget::get_window()->get_size(w, h);
    m_Pixmap = Gdk::Pixmap::create(Gtk::Widget::get_window(), w, h, -1);
  }
}

void VizDrawing::drawYI(BinsViz* pBins) {
  if(pBins->m_bDirty || m_bDirty) {
    m_bDirty = false;

    /**
     * Get the width and height.
     */

    int width, height;
    gdk_threads_enter();
    m_Pixmap->get_size(width, height);
    gdk_threads_leave();

    /**
     * Create a surface to draw on.
     */

    Cairo::RefPtr<Cairo::ImageSurface> cst =
      Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, width, height);
    Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(cst);
    setFont(cr);

    /**
     * Draw on the surface.
     */

    /**
     * Paint the background.
     */

    cr->set_source_rgb(0, 0, 0);
    cr->paint();

    /**
     * Compute the scale.
     */

    pBins->m_Mutex.lock_upgrade();
    double nXScale(width / (double) pBins->box().m_nWidth);
    double nYScale(height / (double) pBins->box().m_nHeight);


    /**
     * Draw the grid with alpha.
     */
    
    if(m_nGranularity > 0)
      drawGrid(cr, nXScale, nYScale, pBins->box());

    /**
     * Draw squares and components on top of it.
     */

    if(!pBins->m_vStack.empty()) {
      std::vector<int> vBins(pBins->box().m_nHeight, 0);
      m_vDrawn.reset();
      for(std::vector<const Rectangle*>::const_iterator i = pBins->m_vStack.begin();
	  i != pBins->m_vStack.end(); ++i) {

	if(m_vDrawn.test((*i)->m_nID)) continue;
	else m_vDrawn.set((*i)->m_nID);

	if(m_bMonochrome)
	  cr->set_source_rgb(1, 1, 1);
	else
	  cr->set_source_rgb((*i)->m_Color.r, (*i)->m_Color.g,
			     (*i)->m_Color.b);
	int nCount(0);

	if(m_bDomain)
	  for(int y = pBins->m_vDomain[(*i)->m_nID].m_nEnd;
	      y < (pBins->m_vDomain[(*i)->m_nID].m_nEnd +
		   pBins->m_vDomain[(*i)->m_nID].m_nWidth); ++y) {
	    ++nCount;
	    drawMapped(vBins[y], y, (int) (*i)->m_nWidth, 1,
		       nXScale, nYScale, cr, pBins->box());
	    cr->fill();
	    if(m_bSquareSizes)
	      drawStripSize(cr, *i, vBins[y], y, nXScale, nYScale, nCount,
			    pBins->box());

	    /**
	     * Move the pointer over so the next square that needs to
	     * draw on top of this bin will be shifted over by the
	     * appropriate amount.
	     */

	    vBins[y] += (*i)->m_nWidth;
	  }
	else
	  for(int y = (*i)->yi.m_nEnd; y < (*i)->yi.m_nEnd + (*i)->yi.m_nWidth; ++y) {
	    ++nCount;
	    drawMapped(vBins[y], y, (int) (*i)->m_nWidth, 1,
		       nXScale, nYScale, cr, pBins->box());
	    cr->fill();
	    if(m_bSquareSizes)
	      drawStripSize(cr, *i, vBins[y], y, nXScale, nYScale, nCount,
			    pBins->box());

	    /**
	     * Move the pointer over so the next square that needs to
	     * draw on top of this bin will be shifted over by the
	     * appropriate amount.
	     */

	    vBins[y] += (*i)->m_nWidth;
	  }
      }
    }

    /**
     * Done drawing. Unlock the grid.
     */

    pBins->m_Mutex.unlock_upgrade_and_lock();
    pBins->m_bDirty = false;
    pBins->m_Mutex.unlock();

    /**
     * We've completed drawing the items. Now we have to transfer this
     * into m_Pixmap.
     */

    gdk_threads_enter();
    {
      Cairo::RefPtr<Cairo::Context> cr_pixmap = m_Pixmap->create_cairo_context();
      cr_pixmap->set_source(cst, 0, 0);
      cr_pixmap->paint();
      if(m_bWritePng) writepng();
    }
    queue_draw();
    gdk_threads_leave();
  }
}

void VizDrawing::drawYF(BinsViz* pBins) {
  if(pBins->m_bDirty || m_bDirty) {
    m_bDirty = false;

    /**
     * Get the width and height.
     */

    int width, height;
    gdk_threads_enter();
    m_Pixmap->get_size(width, height);
    gdk_threads_leave();

    /**
     * Create a surface to draw on.
     */

    Cairo::RefPtr<Cairo::ImageSurface> cst =
      Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, width, height);
    Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(cst);
    setFont(cr);

    /**
     * Draw on the surface.
     */

    /**
     * Paint the background.
     */

    cr->set_source_rgb(0, 0, 0);
    cr->paint();

    /**
     * Compute the scale.
     */

    //std::cout << __FILE__ << "::" << __LINE__ << ": pBins->m_Mutex.lock_upgrade()." << std::endl;
    pBins->m_Mutex.lock_upgrade();
    double nXScale(width / (double) pBins->box().m_nWidth);
    double nYScale(height / (double) pBins->box().m_nHeight);

    /**
     * Draw the grid with alpha.
     */
    
    if(m_nGranularity > 0)
      drawGrid(cr, nXScale, nYScale, pBins->box());

    /**
     * Draw squares and components on top of it.
     */

    if(!pBins->m_vStack.empty()) {
      std::vector<int> vBins(pBins->box().m_nHeight, 0);
      for(std::vector<const Rectangle*>::const_iterator i = pBins->m_vStack.begin();
	  i != pBins->m_vStack.end(); ++i) {
	if(m_bMonochrome)
	  cr->set_source_rgb(1, 1, 1);
	else
	  cr->set_source_rgb((*i)->m_Color.r, (*i)->m_Color.g,
			     (*i)->m_Color.b);
	int nCount(0);
	for(size_t y = (*i)->y; y < (*i)->y + (*i)->m_nHeight; ++y) {
	  ++nCount;
	  drawMapped(vBins[y], y, (int) (*i)->m_nWidth, 1,
		     nXScale, nYScale, cr, pBins->box());
	  cr->fill();
	  if(m_bSquareSizes)
	    drawStripSize(cr, *i, vBins[y], y, nXScale, nYScale, nCount,
			  pBins->box());

	  /**
	   * Move the pointer over so the next square that needs to
	   * draw on top of this bin will be shifted over by the
	   * appropriate amount.
	   */

	  vBins[y] += (*i)->m_nWidth;
	}
      }
    }

    /**
     * Done drawing. Unlock the grid.
     */

    //std::cout << __FILE__ << "::" << __LINE__ << ": pBins->m_Mutex.unlock_upgrade_and_lock()." << std::endl;
    pBins->m_Mutex.unlock_upgrade_and_lock();
    pBins->m_bDirty = false;
    //std::cout << __FILE__ << "::" << __LINE__ << ": pBins->m_Mutex.unlock()." << std::endl;
    pBins->m_Mutex.unlock();

    /**
     * We've completed drawing the items. Now we have to transfer this
     * into m_Pixmap.
     */

    gdk_threads_enter();
    {
      Cairo::RefPtr<Cairo::Context> cr_pixmap = m_Pixmap->create_cairo_context();
      cr_pixmap->set_source(cst, 0, 0);
      cr_pixmap->paint();
      if(m_bWritePng) writepng();
    }
    queue_draw();
    gdk_threads_leave();
  }
}

void VizDrawing::drawXI(GridViz* pGrid) {
  if(pGrid->m_bDirty || m_bDirty) {
    m_bDirty = false;

    /**
     * Get the width and height.
     */

    int width, height;
    gdk_threads_enter();
    m_Pixmap->get_size(width, height);
    gdk_threads_leave();

    /**
     * Create a surface to draw on.
     */

    Cairo::RefPtr<Cairo::ImageSurface> cst =
      Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, width, height);
    Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(cst);
    setFont(cr);

    /**
     * Draw on the surface.
     */

    /**
     * Paint the background.
     */

    cr->set_source_rgb(0, 0, 0);
    cr->paint();

    /**
     * Compute the scale.
     */

    pGrid->m_Mutex.lock_upgrade();
    double nXScale(width / (double) pGrid->box().m_nWidth);
    double nYScale(height / (double) pGrid->box().m_nHeight);

    /**
     * Draw the grid with alpha.
     */
    
    if(m_nGranularity > 0)
      drawGrid(cr, nXScale, nYScale, pGrid->box());

    /**
     * Draw squares and components on top of it.
     */

    for(std::vector<const Rectangle*>::const_iterator i = pGrid->m_vSStack.begin();
	i != pGrid->m_vSStack.end(); ++i) {
      if(m_bMonochrome)
	cr->set_source_rgb(1, 1, 1);
      else
	cr->set_source_rgb((*i)->m_Color.r, (*i)->m_Color.g,
			   (*i)->m_Color.b);
      drawMapped((*i)->xi.m_nEnd, (*i)->y,
		 (int) (*i)->xi.m_nWidth, (int) (*i)->m_nHeight,
		 nXScale, nYScale, cr, pGrid->box());
      cr->fill();
    }
    
    if(m_bSquareSizes)
      for(std::vector<const Rectangle*>::const_iterator i = pGrid->m_vSStack.begin();
	  i != pGrid->m_vSStack.end(); ++i)
	drawSquareSize(cr, *i, nXScale, nYScale, pGrid->box());
    
    /**
     * Done drawing. Unlock the grid.
     */

    pGrid->m_Mutex.unlock_upgrade_and_lock();
    pGrid->m_bDirty = false;
    pGrid->m_Mutex.unlock();

    /**
     * We've completed drawing the items. Now we have to transfer this
     * into m_Pixmap.
     */

    gdk_threads_enter();
    {
      Cairo::RefPtr<Cairo::Context> cr_pixmap = m_Pixmap->create_cairo_context();
      cr_pixmap->set_source(cst, 0, 0);
      cr_pixmap->paint();
      if(m_bWritePng) writepng();
    }
    queue_draw();
    gdk_threads_leave();
  }
}

void VizDrawing::drawXF(GridViz* pGrid) {
  if(pGrid->m_bDirty || m_bDirty) {
    m_bDirty = false;

    /**
     * Get the width and height.
     */

    int width, height;
    gdk_threads_enter();
    m_Pixmap->get_size(width, height);
    gdk_threads_leave();

    /**
     * Create a surface to draw on.
     */

    Cairo::RefPtr<Cairo::ImageSurface> cst =
      Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, width, height);
    Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(cst);
    setFont(cr);

    /**
     * Draw on the surface.
     */

    /**
     * Paint the background.
     */

    cr->set_source_rgb(0, 0, 0);
    cr->paint();

    /**
     * Compute the scale.
     */

    //std::cout << __FILE__ << "::" << __LINE__ << ": m_Mutex.lock_upgrade()" << std::endl;
    pGrid->m_Mutex.lock_upgrade();
    double nXScale(width / (double) pGrid->box().m_nWidth);
    double nYScale(height / (double) pGrid->box().m_nHeight);

    if(m_nGranularity > 0)
      drawGrid(cr, nXScale, nYScale, pGrid->box());

    /**
     * Draw squares and components on top of it.
     */

    std::deque<const Rectangle*> vDrawn;
    if(!pGrid->m_vSStack.empty())
      for(std::vector<const Rectangle*>::const_iterator i = pGrid->m_vSStack.begin();
	  i != pGrid->m_vSStack.end(); ++i) {
	if(m_bMonochrome)
	  cr->set_source_rgb(1, 1, 1);
	else
	  cr->set_source_rgb((*i)->m_Color.r, (*i)->m_Color.g,
			     (*i)->m_Color.b);
	drawMapped((*i)->x, (*i)->y,
		   (int) (*i)->m_nWidth, (int) (*i)->m_nHeight,
		   nXScale, nYScale, cr, pGrid->box());
	cr->fill();
	vDrawn.push_back(*i);
      }
    if(!pGrid->m_vCStack.empty()) {

      /**
       * Instantiate all components in our stack. They are already
       * top-level because we don't place subtree components. They
       * each also do not have any common subtrees as well.
       */

      std::queue<Component*> d;
      for(std::vector<Component*>::const_iterator i = pGrid->m_vCStack.begin();
	  i != pGrid->m_vCStack.end(); ++i) {
	(*i)->instantiate();
	d.push(*i);
      }
      while(!d.empty()) {
	Component* c = d.front();
	d.pop();
	if(c->m_pRect) {
	  if(m_bMonochrome)
	    cr->set_source_rgb(1, 1, 1);
	  else
	    cr->set_source_rgb(c->m_pRect->m_Color.r,
			       c->m_pRect->m_Color.g,
			       c->m_pRect->m_Color.b);
	  drawMapped(c->m_pRect->x, c->m_pRect->y,
		     c->m_pRect->m_nWidth, c->m_pRect->m_nHeight,
		     nXScale, nYScale, cr, pGrid->box());
	  cr->fill();
	  vDrawn.push_back(c->m_pRect);
	}
	else {
	  cr->set_source_rgb(0.9, 0.9, 0.9);
	  cr->set_line_width(m_nLineWidth);
	  drawMapped(c->m_nX, c->m_nY,
		     c->m_Dims.m_nWidth, c->m_Dims.m_nHeight,
		     nXScale, nYScale, cr, pGrid->box());
	  cr->stroke();
	  for(std::set<Component*>::const_iterator i = c->m_Members.begin();
	      i != c->m_Members.end(); ++i)
	    d.push(*i);
	}
      }
    }

    /**
     * Draw the square sizes.
     */

    if(m_bSquareSizes)
      while(!vDrawn.empty()) {
	drawSquareSize(cr, vDrawn.front(), nXScale, nYScale,
		       pGrid->box());
	vDrawn.pop_front();
      }

    /**
     * Done drawing. Unlock the grid.
     */
    //std::cout << __FILE__ << "::" << __LINE__ << ": m_Mutex.unlock_upgrade_and_lock()" << std::endl;
    pGrid->m_Mutex.unlock_upgrade_and_lock();
    pGrid->m_bDirty = false;
    //std::cout << __FILE__ << "::" << __LINE__ << ": m_Mutex.unlock()" << std::endl;
    pGrid->m_Mutex.unlock();

    /**
     * We've completed drawing the items. Now we have to transfer this
     * into m_Pixmap.
     */

    gdk_threads_enter();
    {
      Cairo::RefPtr<Cairo::Context> cr_pixmap = m_Pixmap->create_cairo_context();
      cr_pixmap->set_source(cst, 0, 0);
      cr_pixmap->paint();
      if(m_bWritePng) writepng();
    }
    queue_draw();
    gdk_threads_leave();
  }
}

void VizDrawing::drawGrid(Cairo::RefPtr<Cairo::Context>& cr,
			  double nXScale, double nYScale,
			  const BoxDimensions& b) {

  /**
   * Draw the vertical hash.
   */

  cr->set_source_rgb(0.5, 0.5, 0.5);
  cr->set_line_width(m_nLineWidth);
  if(m_bFlipHorz)
    for(int x = b.m_nWidth - m_nGranularity; x >= 0; x -= m_nGranularity) {
      cr->move_to(x * nXScale, 0);
      cr->line_to(x * nXScale, b.m_nHeight * nYScale);
      cr->stroke();
    }
  else
    for(int x = m_nGranularity; x < (int) b.m_nWidth; x += m_nGranularity) {
      cr->move_to(x * nXScale, 0);
      cr->line_to(x * nXScale, b.m_nHeight * nYScale);
      cr->stroke();
    }

  /**
   * Draw the horizontal hash.
   */

  if(m_bFlipVert)
    for(int y = b.m_nHeight - m_nGranularity; y >= 0; y -= m_nGranularity) {
      cr->move_to(0, y * nYScale);
      cr->line_to(b.m_nWidth * nXScale, y * nYScale);
      cr->stroke();
    }
  else
    for(int y = m_nGranularity; y < (int) b.m_nHeight; y += m_nGranularity) {
      cr->move_to(0, y * nYScale);
      cr->line_to(b.m_nWidth * nXScale, y * nYScale);
      cr->stroke();
    }
}

bool VizDrawing::updateGranularity(guint n) {
  if(n < GDK_0 || n > GDK_9)
    return(false);
  switch(n) {
  case GDK_0:
    m_nGranularity = (m_nGranularity == 10 ? 0 : 10); break;
  case GDK_1:
    m_nGranularity = (m_nGranularity == 1 ? 0 : 1); break;
  case GDK_2:
    m_nGranularity = (m_nGranularity == 2 ? 0 : 2); break;
  case GDK_3:
    m_nGranularity = (m_nGranularity == 3 ? 0 : 3); break;
  case GDK_4:
    m_nGranularity = (m_nGranularity == 4 ? 0 : 4); break;
  case GDK_5:
    m_nGranularity = (m_nGranularity == 5 ? 0 : 5); break;
  case GDK_6:
    m_nGranularity = (m_nGranularity == 6 ? 0 : 6); break;
  case GDK_7:
    m_nGranularity = (m_nGranularity == 7 ? 0 : 7); break;
  case GDK_8:
    m_nGranularity = (m_nGranularity == 8 ? 0 : 8); break;
  case GDK_9:
    m_nGranularity = (m_nGranularity == 9 ? 0 : 9); break;
  default:
    break;
  };
  m_bDirty = true;
  return(true);
}

void VizDrawing::drawSquareSize(Cairo::RefPtr<Cairo::Context>& cr,
				const Rectangle* r, double nXScale,
				double nYScale, const BoxDimensions& b) {
  cr->save();
  cr->set_source_rgb(0, 0, 0);
  double nx = (m_bFlipHorz ? (b.m_nWidth - r->x - r->m_nWidth) : r->x) * nXScale;
  double ny = ((m_bFlipVert ? (b.m_nHeight - r->y - r->m_nHeight) : r->y) +
	       (r->m_nHeight / 2.0)) * nYScale - ((m_nFontSize + 10) / 2);
  cr->move_to(nx, ny);
  std::ostringstream oss;
  oss << r->m_nWidth << "x" << r->m_nHeight << std::flush;
  m_Pango->set_width(r->m_nWidth * nXScale * Pango::SCALE);
  m_Pango->set_text(oss.str());
  cr->set_line_width(m_nLineWidth);
  m_Pango->update_from_cairo_context(cr);
  m_Pango->add_to_cairo_context(cr);
  cr->fill();
  cr->restore();
}

void VizDrawing::drawStripSize(Cairo::RefPtr<Cairo::Context>& cr,
			       const Rectangle* r, int x, int y,
			       double nXScale, double nYScale,
			       int nCount, const BoxDimensions& b) {
  cr->save();
  cr->set_source_rgb(0, 0, 0);
  double nx = (m_bFlipHorz ? (b.m_nWidth - x - r->m_nWidth) : x) * nXScale;
  double ny = (m_bFlipVert ? (b.m_nHeight - y - 1) : y) * nYScale + 2;
  cr->move_to(nx, ny);
  std::ostringstream oss;
  oss << nCount << "/" << r->m_nWidth << "x"
      << r->m_nHeight << std::flush;
  m_Pango->set_width(r->m_nWidth * nXScale * Pango::SCALE);
  m_Pango->set_text(oss.str());
  cr->set_line_width(m_nLineWidth);
  m_Pango->update_from_cairo_context(cr);
  m_Pango->add_to_cairo_context(cr);
  cr->fill(); 
  cr->restore();
}

void VizDrawing::drawMapped(int x, int y, int w, int h,
			    double& xs, double& ys,
			    Cairo::RefPtr<Cairo::Context>& cr,
			    const BoxDimensions& b) {
  double nx = m_bFlipHorz ? ((b.m_nWidth - x - w) * xs) : (x * xs);
  double ny = m_bFlipVert ? ((b.m_nHeight - y - h) * ys) : (y * ys);
  cr->rectangle(nx + 0.5, ny + 0.5, w * xs - 1, h * ys - 1);
}

void VizDrawing::setFont(Cairo::RefPtr<Cairo::Context>& cr) {
  if(m_bSquareSizes) {
    std::ostringstream oss;
    oss << "sans bold " << m_nFontSize << std::flush;
    m_Pango = Pango::Layout::create(cr);
    Pango::FontDescription font_descr(oss.str().c_str());
    m_Pango->set_font_description(font_descr);
    m_Pango->set_alignment(Pango::ALIGN_CENTER);
  }
}

void VizDrawing::writepng() {
  Glib::RefPtr<Gdk::Drawable> pm(m_Pixmap);
  Glib::RefPtr<Gdk::Pixbuf> pb = Gdk::Pixbuf::create(pm, 0, 0, -1, -1);
  std::ostringstream oss;
  oss << std::setw(10) << std::setfill('0') << m_nPngIndex
      << ".png" << std::flush;
  pb->save(oss.str(), "png");
  ++m_nPngIndex;
}

#endif // VIZ
