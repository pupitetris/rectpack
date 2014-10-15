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

#include "NodeBase.h"
#include "TimeSpec.h"

NodeBase::NodeBase() {
  std::fill(m_vNodes, m_vNodes + 4, 0);
}

NodeBase::NodeBase(const NodeBase& src) {
  for(size_t i = 0; i < 4; ++i)
    m_vNodes[i] = src.m_vNodes[i];
}

NodeBase::~NodeBase() {
}

void NodeBase::clear() {
  std::fill(m_vNodes, m_vNodes + 4, 0);
}

unsigned long long NodeBase::get(VizType n) const {
  return(m_vNodes[n]);
}

void NodeBase::set(VizType n, unsigned long long m) {
  m_vNodes[n] = m;
}

void NodeBase::print() const {
  std::cout << "XI=" << get(XI) << ", "
	    << "XF=" << get(XF) << ", "
	    << "YI=" << get(YI) << ", "
	    << "YF=" << get(YF) << std::flush;
}

void NodeBase::print(const TimeSpec& t, size_t nCuSP) const {
#ifndef IJCAI
  std::cout
    << "  XI=" << get(XI)
    << ", XF=" << get(XF)
    << ", XT=" << get(XI) + get(XF)
    << ", YI=" << get(YI)
    << ", YF=" << get(YF)
    << ", YT=" << get(YI) + get(YF)
    << ", CS=" << nCuSP
    << ", t=" << t << " (" << t.toString() << ')'
    << std::endl << std::endl;
#endif // IJCAI
}

void NodeBase::accumulate(NodeBase& src) {
  for(size_t i = 0; i < 4; ++i)
    m_vNodes[i] += src.m_vNodes[i];
}

void NodeBase::decumulate(NodeBase& src) {
  for(size_t i = 0; i < 4; ++i)
    m_vNodes[i] -= src.m_vNodes[i];
}

unsigned long long NodeBase::total() const {
  unsigned long long n(0);
  for(size_t i = 0; i < 4; ++i)
    n += m_vNodes[i];
  return(n);
}

const NodeBase& NodeBase::operator=(const NodeBase& src) {
  for(size_t i = 0; i < 4; ++i)
    m_vNodes[i] = src.m_vNodes[i];
  return(*this);
}

bool NodeBase::operator<(const NodeBase& rhs) const {
  for(size_t i = 0; i < 4; ++i)
    if(m_vNodes[i] != rhs.m_vNodes[i])
      return(m_vNodes[i] < rhs.m_vNodes[i]);
  return(false);
}

bool NodeBase::operator!=(const NodeBase& rhs) const {
  return(m_vNodes[0] == rhs.m_vNodes[0] &&
	 m_vNodes[1] == rhs.m_vNodes[1] &&
	 m_vNodes[2] == rhs.m_vNodes[2] &&
	 m_vNodes[3] == rhs.m_vNodes[3]);
}

void NodeBase::tick(VizType n, int m) {
  m_vNodes[n] += m;
}
