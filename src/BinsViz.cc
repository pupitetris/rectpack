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
#include "SearchControl.h"

BinsViz::BinsViz() :
  m_bInitialized(false),
  m_bDirty(true),
  m_pControl(NULL) {
}

BinsViz::BinsViz(const BinsViz& src) :
  BinsBase(src),
  m_bInitialized(src.m_bInitialized),
  m_bDirty(src.m_bDirty),
  m_vStack(src.m_vStack),
  m_Box(src.m_Box),
  m_pControl(src.m_pControl),
  m_nType(src.m_nType) {
}

BinsViz::~BinsViz() {
}

void BinsViz::clear() {
  boost::unique_lock<boost::shared_mutex> lock(m_Mutex);
  {
    //std::cout << __FILE__ << "::" << __LINE__ << ": lock(m_Mutex)." << std::endl;
    BinsBase::clear();
    m_vStack.clear();
    m_bInitialized = false;
    m_bDirty = true;
  }
  //std::cout << __FILE__ << "::" << __LINE__ << ": ~lock(m_Mutex)." << std::endl;
}

void BinsViz::resizey(const BoxDimensions& b, const RectPtrArray& v) {
  boost::this_thread::interruption_point();
  BinsBase::resizey(b, v);
  m_Mutex.lock();
  m_Box = b;
  m_vStack.clear();
  m_bInitialized = true;
  m_bDirty = true;
  m_Mutex.unlock();
}

void BinsViz::pushyi(const Rectangle* r) {
  boost::this_thread::interruption_point();
  BinsBase::pushyi(r);
  pushStack(r);
}

void BinsViz::popyi(const Rectangle* r) {
  boost::this_thread::interruption_point();
  BinsBase::popyi(r);
  popStack();
}

void BinsViz::pushyi(const Rectangle* r, size_t nOldSize) {
  boost::this_thread::interruption_point();
  BinsBase::pushyi(r, nOldSize);
  pushStack(r);
}

void BinsViz::popyi(const Rectangle* r, size_t nOldSize) {
  boost::this_thread::interruption_point();
  BinsBase::popyi(r, nOldSize);
  popStack();
}

void BinsViz::pushx(const Rectangle* r) {
  boost::this_thread::interruption_point();
  BinsBase::pushx(r);
  pushStack(r);
}

void BinsViz::pushy(const Rectangle* r) {
  boost::this_thread::interruption_point();
  BinsBase::pushy(r);
  pushStack(r);
}

void BinsViz::popx(const Rectangle* r) {
  boost::this_thread::interruption_point();
  BinsBase::popx(r);
  popStack();
}

void BinsViz::popy(const Rectangle* r) {
  boost::this_thread::interruption_point();
  BinsBase::popy(r);
  popStack();
}

void BinsViz::popStack() {
  if(m_pControl) {
    //std::cout << __FILE__ << "::" << __LINE__ << ": m_Mutex.lock()." << std::endl;
    m_Mutex.lock();
    m_vStack.pop_back();
    m_bDirty = true;
    //std::cout << __FILE__ << "::" << __LINE__ << ": m_Mutex.unlock_and_lock_shared()." << std::endl;
    m_Mutex.unlock_and_lock_shared();
    m_pControl->postDel(m_nType);
  }
}

void BinsViz::pushStack(const Rectangle* r) {
  if(m_pControl) {
    //std::cout << __FILE__ << "::" << __LINE__ << ": m_Mutex.lock()." << std::endl;
    m_Mutex.lock();
    m_vStack.push_back(r);
    m_bDirty = true;
    //std::cout << __FILE__ << "::" << __LINE__ << ": m_Mutex.unlock_and_lock_shared()." << std::endl;
    m_Mutex.unlock_and_lock_shared();
    m_pControl->postAdd(m_nType);
  }
}

void BinsViz::pushSimpley(const Rectangle* r) {
  boost::this_thread::interruption_point();
  BinsBase::pushSimpley(r);
  pushStack(r);
}

void BinsViz::popSimpley(const Rectangle* r) {
  boost::this_thread::interruption_point();
  BinsBase::popSimpley(r);
  popStack();
}

void BinsViz::pushStripsy(const Rectangle* r) {
  boost::this_thread::interruption_point();
  BinsBase::pushStripsy(r);
  pushStack(r);
}

void BinsViz::popStripsy(const Rectangle* r) {
  boost::this_thread::interruption_point();
  BinsBase::popStripsy(r);
  popStack();
}

size_t BinsViz::stackSize() const {
  return(m_vStack.size());
}

const BinsViz& BinsViz::operator=(const BinsViz& rhs) {
  //std::cout << __FILE__ << "::" << __LINE__ << ": m_Mutex.lock()." << std::endl;
  m_Mutex.lock();
  BinsBase::operator=(rhs);
  m_bInitialized = rhs.m_bInitialized;
  m_bDirty = rhs.m_bDirty;
  m_vStack = rhs.m_vStack;
  m_Box = rhs.m_Box;
  m_pControl = rhs.m_pControl;
  m_nType = rhs.m_nType;
  //std::cout << __FILE__ << "::" << __LINE__ << ": m_Mutex.unlock()." << std::endl;
  m_Mutex.unlock();
  return(*this);
}

const BoxDimensions& BinsViz::box() {
  return(m_Box);
}

bool BinsViz::compress() {
  boost::this_thread::interruption_point();
  bool b;
  if(m_pControl) {
    m_Mutex.lock();
    b = BinsBase::compress();
    m_bDirty = true;
    m_Mutex.unlock_and_lock_shared();
    m_pControl->postCompress(m_nType);
  }
  else
    b = BinsBase::compress();
  return(b);
}

bool BinsViz::compress(std::deque<const Rectangle*>& v) {
  boost::this_thread::interruption_point();
  bool b;
  if(m_pControl) {
    m_Mutex.lock();
    b = BinsBase::compress(v);
    m_bDirty = true;
    m_Mutex.unlock_and_lock_shared();
    m_pControl->postCompress(m_nType);
  }
  else
    b = BinsBase::compress(v);
  return(b);
}

bool BinsViz::compress(Rectangle* pJustPlaced) {
  boost::this_thread::interruption_point();
  bool b;
  if(m_pControl) {
    m_Mutex.lock();
    b = BinsBase::compress(pJustPlaced);
    m_bDirty = true;
    m_Mutex.unlock_and_lock_shared();
    m_pControl->postCompress(m_nType);
  }
  else
    b = BinsBase::compress(pJustPlaced);
  return(b);
}

bool BinsViz::compress(const Rectangle* pJustPlaced,
		       std::deque<const Rectangle*>& v) {
  boost::this_thread::interruption_point();
  bool b;
  if(m_pControl) {
    m_Mutex.lock();
    b = BinsBase::compress(pJustPlaced, v);
    m_bDirty = true;
    m_Mutex.unlock_and_lock_shared();
    m_pControl->postCompress(m_nType);
  }
  else
    b = BinsBase::compress(pJustPlaced, v);
  return(b);
}

bool BinsViz::compress(Rectangle* pJustPlaced, VectorStack<UInt>& v) {
  boost::this_thread::interruption_point();
  bool b;
  if(m_pControl) {
    m_Mutex.lock();
    b = BinsBase::compress(pJustPlaced, v);
    m_bDirty = true;
    m_Mutex.unlock_and_lock_shared();
    m_pControl->postCompress(m_nType);
  }
  else
    b = BinsBase::compress(pJustPlaced);
  return(b);
}

#endif // VIZ
