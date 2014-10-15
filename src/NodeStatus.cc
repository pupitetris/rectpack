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

#include "NodeStatus.h"

NodeStatus::NodeStatus() :
  m_bDirty(true),
  m_nCurrent(0),
  m_nPrevious(0) {
  sprintf(m_pBuffer, "Nodes: 0, +0");
  push(m_pBuffer);
}

NodeStatus::~NodeStatus() {
}

void NodeStatus::setPrevious() {
  m_Mutex.lock();
  m_nPrevious = m_nCurrent;
  m_bDirty = true;
  m_Mutex.unlock();
}

void NodeStatus::set(const unsigned long long& n) {
  m_Mutex.lock();
  if(m_nCurrent != n) {
    m_nCurrent = n;
    m_bDirty = true;
  }
  m_Mutex.unlock();
}

void NodeStatus::redraw() {
  m_Mutex.lock();
  if(m_bDirty) {
    m_bDirty = false;
    if(m_nCurrent >= m_nPrevious)
      sprintf(m_pBuffer, "Nodes: %llu - %llu = %llu", m_nCurrent,
	      m_nPrevious, m_nCurrent - m_nPrevious);
    else
      sprintf(m_pBuffer, "Nodes: %llu - 0 = %llu", m_nCurrent,
	      m_nCurrent);
    pop();
    push(m_pBuffer);
  }
  m_Mutex.unlock();
}

#endif // VIZ
