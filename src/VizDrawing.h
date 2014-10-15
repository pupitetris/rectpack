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

#ifndef VIZDRAWING_H
#define VIZDRAWING_H

#include <gtkmm.h>
#include <boost/dynamic_bitset.hpp>

class BoxDimensions;
class BinsViz;
class GridViz;

class VizDrawing : public Gtk::DrawingArea {
 public:
  VizDrawing();
  ~VizDrawing();
  void initialize();

  /**
   * Must be called immediately after all windows are shown but before
   * we start the event loop, as this will not be thread-safe.
   */

  void initPixmap();
  bool on_expose_event(GdkEventExpose* event);
  bool on_configure_event(GdkEventConfigure* event);
  void drawGrid(Cairo::RefPtr<Cairo::Context>& cr, double nXScale,
		double nYScale, const BoxDimensions& b);
  void drawSquareSize(Cairo::RefPtr<Cairo::Context>& cr,
		      const Rectangle* s, double nXScale,
		      double nYScale, const BoxDimensions& b);
  void drawStripSize(Cairo::RefPtr<Cairo::Context>& cr,
		     const Rectangle* s, int x, int y, double nXScale,
		     double nYScale, int nCount,
		     const BoxDimensions& b);
  void drawYI(BinsViz* pBins);
  void drawYF(BinsViz* pBins);
  void drawXF(GridViz* pGrid);
  void drawXI(GridViz* pGrid);
  bool updateGranularity(guint n);
  void drawMapped(int x, int y, int w, int h, double& xs, double& ys,
		  Cairo::RefPtr<Cairo::Context>& cr,
		  const BoxDimensions& b);
  void setFont(Cairo::RefPtr<Cairo::Context>& cr);
  void writepng();
  bool m_bDirty;
  size_t m_nGranularity;
  bool m_bSquareSizes;
  bool m_bDomain;
  bool m_bMonochrome;
  bool m_bFlipHorz;
  bool m_bFlipVert;
  size_t m_nFontSize;
  float m_nLineWidth;
  bool m_bWritePng;
  size_t m_nPngIndex;
  boost::dynamic_bitset<> m_vDrawn;

 private:
  Glib::RefPtr<Gdk::Pixmap> m_Pixmap;
  Glib::RefPtr<Pango::Layout> m_Pango;
};

#endif // VIZDRAWING_H

#endif // VIZ
