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

#include "GridViz.h"
#include "Parameters.h"
#include "SearchControl.h"
#include <boost/thread.hpp>

GridViz::GridViz(const Packer* pPacker) :
  Grid(pPacker),
  m_bInitialized(false),
  m_bDirty(false),
  m_pControl(NULL),
  m_nType(XF) {
}

GridViz::GridViz() {
}

GridViz::~GridViz() {
}

void GridViz::initialize(const Parameters* pParams) {
  boost::this_thread::interruption_point();
  Grid::initialize(pParams);
  //std::cout << __FILE__ << "::" << __LINE__ << ": m_Mutex.lock()" << std::endl;
  m_Mutex.lock();
  m_vSStack.clear();
  m_vSStack.reserve(pParams->m_vInstance.size());
  m_vCStack.clear();
  m_vCStack.reserve(pParams->m_vInstance.size() * 4);
  //std::cout << __FILE__ << "::" << __LINE__ << ": m_Mutex.unlock()" << std::endl;
  m_Mutex.unlock();
}

void GridViz::initialize(const BoxDimensions* pBox) {
  boost::this_thread::interruption_point();
  Grid::initialize(pBox);
  m_bInitialized = true;
}

void GridViz::resize(int nDim) {
  boost::this_thread::interruption_point();
  Grid::resize(nDim);
  //std::cout << __FILE__ << "::" << __LINE__ << ": m_Mutex.lock()" << std::endl;
  m_Mutex.lock();
  m_bDirty = true;
  //std::cout << __FILE__ << "::" << __LINE__ << ": m_Mutex.unlock()" << std::endl;
  m_Mutex.unlock();
}

void GridViz::add(const Rectangle* r, std::vector<Int>& v) {
  boost::this_thread::interruption_point();
  Grid::add(r, v);
  pushSStack(r);
}

void GridViz::add(const Rectangle* r) {
  boost::this_thread::interruption_point();
  Grid::add(r);
  pushSStack(r);
}

void GridViz::add(Component* c) {
  boost::this_thread::interruption_point();
  Grid::add(c);
  pushCStack(c);
}

void GridViz::add(Component* c, std::vector<Int>& v) {
  boost::this_thread::interruption_point();
  Grid::add(c, v);
  pushCStack(c);
}

void GridViz::addSimple(const Rectangle* r) {
  boost::this_thread::interruption_point();
  Grid::addSimple(r);
  pushSStack(r);
}

void GridViz::del(const Rectangle* r) {
  boost::this_thread::interruption_point();
  Grid::del(r);
  popSStack();
}

void GridViz::del(Component* c) {
  boost::this_thread::interruption_point();
  Grid::del(c);
  popCStack();
}

void GridViz::popSStack() {
  if(m_pControl) {
    //std::cout << __FILE__ << "::" << __LINE__ << ": m_Mutex.lock()" << std::endl;
    m_Mutex.lock();
    m_vSStack.pop_back();
    m_bDirty = true;
    //std::cout << __FILE__ << "::" << __LINE__ << ": m_Mutex.unlock_and_lock_shared()" << std::endl;
    m_Mutex.unlock_and_lock_shared();
    m_pControl->postDel(m_nType);
  }
}

void GridViz::popSStack(const std::list<Rectangle*>& l) {
  if(m_pControl) {
    //std::cout << __FILE__ << "::" << __LINE__ << ": m_Mutex.lock()" << std::endl;
    m_Mutex.lock();
    for(std::list<Rectangle*>::const_iterator i = l.begin();
	i != l.end(); ++i)
      m_vSStack.pop_back();
    m_bDirty = true;
    //std::cout << __FILE__ << "::" << __LINE__ << ": m_Mutex.unlock_and_lock_shared()" << std::endl;
    m_Mutex.unlock_and_lock_shared();
    m_pControl->postDel(m_nType);
  }
}

void GridViz::popCStack() {
  if(m_pControl) {
    //std::cout << __FILE__ << "::" << __LINE__ << ": m_Mutex.lock()" << std::endl;
    m_Mutex.lock();
    m_vCStack.pop_back();
    m_bDirty = true;
    //std::cout << __FILE__ << "::" << __LINE__ << ": m_Mutex.unlock_and_lock_shared()" << std::endl;
    m_Mutex.unlock_and_lock_shared();
    m_pControl->postDel(m_nType);
  }
}

void GridViz::pushSStack(const Rectangle* r) {
  if(m_pControl) {
    //std::cout << __FILE__ << "::" << __LINE__ << ": m_Mutex.lock()" << std::endl;
    m_Mutex.lock();
    m_vSStack.push_back(r);
    m_bDirty = true;
    //std::cout << __FILE__ << "::" << __LINE__ << ": m_Mutex.unlock_and_lock_shared()" << std::endl;
    m_Mutex.unlock_and_lock_shared();
    m_pControl->postAdd(m_nType);
  }
}

void GridViz::pushSStack(const std::list<Rectangle*>& l) {
  if(m_pControl) {
    //std::cout << __FILE__ << "::" << __LINE__ << ": m_Mutex.lock()" << std::endl;
    m_Mutex.lock();
    for(std::list<Rectangle*>::const_iterator i = l.begin();
	i != l.end(); ++i)
      m_vSStack.push_back(*i);
    m_bDirty = true;
    //std::cout << __FILE__ << "::" << __LINE__ << ": m_Mutex.unlock_and_lock_shared()" << std::endl;
    m_Mutex.unlock_and_lock_shared();
    m_pControl->postAdd(m_nType);
  }
}

void GridViz::pushCStack(Component* c) {
  if(m_pControl) {
    //std::cout << __FILE__ << "::" << __LINE__ << ": m_Mutex.lock()" << std::endl;
    m_Mutex.lock();
    m_vCStack.push_back(c);
    m_bDirty = true;
    //std::cout << __FILE__ << "::" << __LINE__ << ": m_Mutex.unlock_and_lock_shared()" << std::endl;
    m_Mutex.unlock_and_lock_shared();
    if(m_pControl) m_pControl->postAdd(m_nType);
  }
}

size_t GridViz::stackSize() const {
  return(std::max(m_vSStack.size(), m_vCStack.size()));
}

#endif // VIZ
