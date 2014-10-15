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

#ifndef BINSVIZ_H
#define BINSVIZ_H

#include "BinsBase.h"
#include "BoxDimensions.h"
#include "VectorStack.h"
#include "VizType.h"
#include <deque>
#include <boost/thread/shared_mutex.hpp>

class BinsViz;
class SearchControl;

class BinsViz : public BinsBase {
 public:
  BinsViz();
  BinsViz(const BinsViz& src);
  ~BinsViz();
  const BinsViz& operator=(const BinsViz& rhs);
  void clear();
  void resizey(const BoxDimensions& b, const RectPtrArray& v);

  void pushy(const Rectangle* r);
  void popy(const Rectangle* r);
  void pushx(const Rectangle* r);
  void popx(const Rectangle* r);
  void pushyi(const Rectangle* r);
  void popyi(const Rectangle* r);
  void pushyi(const Rectangle* r, size_t nOldSize);
  void popyi(const Rectangle* r, size_t nOldSize);

  void pushSimpley(const Rectangle* r);
  void popSimpley(const Rectangle* r);

  void pushStripsy(const Rectangle* r);
  void popStripsy(const Rectangle* r);

  bool compress();
  bool compress(std::deque<const Rectangle*>& v);
  bool compress(Rectangle* pJustPlaced);
  bool compress(Rectangle* pJustPlaced, VectorStack<UInt>& v);
  bool compress(const Rectangle* pJustPlaced, std::deque<const Rectangle*>& v);
  
  size_t stackSize() const;
  const BoxDimensions& box();

  /**
   * Thread-safe versions of stack manipulation.
   */

  void popStack();
  void pushStack(const Rectangle* r);
  boost::shared_mutex m_Mutex;
  bool m_bInitialized;
  bool m_bDirty;
  std::vector<const Rectangle*> m_vStack;
  BoxDimensions m_Box;
  SearchControl* m_pControl;
  VizType m_nType;
};

#endif // BINSVIZ_H

#endif // VIZ
