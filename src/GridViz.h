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

#ifndef GRIDVIZ_H
#define GRIDVIZ_H

#include "Grid.h"
#include "VizType.h"
#include <list>
#include <vector>
#include <boost/thread/shared_mutex.hpp>

class BoxDimensions;
class Component;
class Interval;
class Parameters;
class Packer;
class Rectangle;
class SearchControl;

class GridViz : public Grid {
 public:
  GridViz(const Packer* pPacker);
  GridViz();
  ~GridViz();
  void initialize(const Parameters* pParams);
  void initialize(const BoxDimensions* pBox);
  void resize(int nDim);
  void add(const Rectangle* s);
  void add(const Rectangle* s, std::vector<Int>& v);
  void add(Component* c);
  void add(Component* c, std::vector<Int>& v);
  void addSimple(const Rectangle* s);
  void del(const Rectangle* s);
  void del(Component* c);
  size_t stackSize() const;

  /**
   * These are not thread-safe.
   */

  void popSStack();
  void popSStack(const std::list<Rectangle*>& l);
  void popCStack();
  void pushSStack(const Rectangle* s);
  void pushSStack(const std::list<Rectangle*>& l);
  void pushCStack(Component* c);

  boost::shared_mutex m_Mutex;
  bool m_bInitialized;
  bool m_bDirty;
  std::vector<const Rectangle*> m_vSStack;
  std::vector<Component*> m_vCStack;
  SearchControl* m_pControl;
  VizType m_nType;
};

#endif // GRIDVIZ_H

#endif // VIZ
