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

#ifndef LOCKABLESTATE_H
#define LOCKABLESTATE_H

#include "VizType.h"
#include <boost/thread/shared_mutex.hpp>
#include <vector>

class BinsBase;
class BinsViz;
class Grid;
class GridViz;
class SearchControl;

class LockableState {
 public:
  LockableState();
  ~LockableState();
  void enable(VizType n);
  void disable(VizType n);
  void set(SearchControl* pControl);
  void set(BinsViz* pBins, VizType n);
  void set(GridViz* pGrid, VizType n);
  void set(BinsBase* pBins, VizType n);
  void set(Grid* pGrid, VizType n);
  BinsViz* getBins(VizType n);
  GridViz* getGrid(VizType n);
  int stackSize(VizType n) const;
  void lock(VizType n);
  void lock_shared(VizType n);
  void unlock(VizType n);
  void unlock_shared(VizType n);
  void lock_ptr(VizType n);
  void lock_shared_ptr(VizType n);
  void unlock_ptr(VizType n);
  void unlock_shared_ptr(VizType n);

 private:
  boost::shared_mutex m_vPointers[4];
  std::vector<bool> m_vLockable;
  BinsViz* m_pYI;
  BinsViz* m_pYF;
  GridViz* m_pXI;
  GridViz* m_pXF;
};

#endif // LOCKABLESTATE_H

#endif // VIZ
