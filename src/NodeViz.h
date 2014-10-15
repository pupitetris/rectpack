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

#ifndef NODEVIZ_H
#define NODEVIZ_H

#include "NodeBase.h"
#include <boost/thread/shared_mutex.hpp>

class NodeViz;

class NodeViz : public NodeBase {
 private:
  const NodeViz& operator+=(const NodeViz& src);
  boost::shared_mutex m_vMutex[4];

 public:
  NodeViz() { }
  NodeViz(const NodeViz& src) : NodeBase(src) { }
  ~NodeViz() { }

  void clear() {
    for(int i = 0; i < 4; ++i) m_vMutex[i].lock();
    NodeBase::clear();
    for(int i = 0; i < 4; ++i) m_vMutex[i].unlock();
  }
 
  const NodeViz& operator=(const NodeViz& src) {
    NodeBase::operator=(src);
    return(*this);
  }

  bool operator<(const NodeViz& rhs) const {
    for(size_t i = 0; i < 4; ++i)
      if(m_vNodes[i] != rhs.m_vNodes[i])
	return(m_vNodes[i] < rhs.m_vNodes[i]);
    return(false);
  }

  unsigned long long get(VizType n) const {
    unsigned long long m;
    const_cast<NodeViz*>(this)->m_vMutex[n].lock_shared();
    m = NodeBase::get(n);
    const_cast<NodeViz*>(this)->m_vMutex[n].unlock_shared();
    return(m);
  }

  void tick(VizType n) {
    m_vMutex[n].lock();
    NodeBase::tick(n);
    m_vMutex[n].unlock();
  }

  void tick(VizType n, int m) {
    m_vMutex[n].lock();
    NodeBase::tick(n, m);
    m_vMutex[n].unlock();
  }

  void accumulate(NodeViz& src) {
    for(int i = 0; i < 4; ++i) {
      src.m_vMutex[i].lock_shared();
      m_vMutex[i].lock();
    }
    NodeBase::accumulate(src);
    for(int i = 0; i < 4; ++i) {
      src.m_vMutex[i].unlock_shared();
      m_vMutex[i].unlock();
    }
  }

  void decumulate(NodeViz& src) {
    for(int i = 0; i < 4; ++i) {
      src.m_vMutex[i].lock_shared();
      m_vMutex[i].lock();
    }
    NodeBase::decumulate(src);
    for(int i = 0; i < 4; ++i) {
      src.m_vMutex[i].unlock_shared();
      m_vMutex[i].unlock();
    }
  }
};

#endif // NODEVIZ_H
