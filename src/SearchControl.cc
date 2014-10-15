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

#include "Component.h"
#include "LockableState.h"
#include "SearchControl.h"
#include <gdk/gdkkeysyms.h>

SearchControl::SearchControl() :
  m_pState(NULL),
  m_nType(XF),
  m_nCondition(PAUSE),
  m_nDepth(0),
  m_bBlockOnCompression(false),
  m_bContinue(false),
  m_bRunAgain(false) {
}

SearchControl::~SearchControl() {
}

void SearchControl::postAdd(VizType nType) {
  {
    boost::unique_lock<boost::mutex> lock(m_Mutex);

    /**
     * General pause directive.
     */

    if(m_nCondition == PAUSE)
      block(nType, lock);

    /**
     * We're in the same search space and we're supposed to pause after
     * the addition of the element at this specific stack depth.
     */

    else if(m_nType == nType && m_nCondition == PAUSE_AFTER_ADD &&
	    m_pState->stackSize(nType) == m_nDepth) {
      m_nCondition = PAUSE;
      block(nType, lock);
    }

    /**
     * Pausing at the next depth is automatic if we reached a new
     * search space or if we're in the current search space and we're
     * one more than before.
     */

    else if(m_nCondition == PAUSE_NEXT_DEPTH &&
	    (nType > m_nType || // We've reached the next search space or...
	     m_nType == nType && m_pState->stackSize(nType) > m_nDepth)) {
      m_nCondition = PAUSE;
      block(nType, lock);
    }
    else {
      m_pState->unlock_shared(nType);
      m_pState->unlock_shared_ptr(nType);
    }
  }
}

void SearchControl::postDel(VizType nType) {
  boost::unique_lock<boost::mutex> lock(m_Mutex);

  /**
   * General pause directive.
   */

  if(m_nCondition == PAUSE)
    block(nType, lock);

  /**
   * Pause at the deletion of whatever previous depth we remembered
   * and also pause when we've reverted to a previous search space.
   */

  else if(m_nCondition == PAUSE_AFTER_DEL &&
	  (nType < m_nType || // We're in the previous search space or...
	   m_nType == nType && m_pState->stackSize(nType) < m_nDepth)) {
    m_nCondition = PAUSE;
    block(nType, lock);
  }
  else {
    //std::cout << __FILE__ << "::" << __LINE__ << ": m_pState->unlock_shared(" << (int) nType << ")" << std::endl;
    m_pState->unlock_shared(nType);
    m_pState->unlock_shared_ptr(nType);
  }
}

void SearchControl::postClear() {
  boost::unique_lock<boost::mutex> lock(m_Mutex);
  if(m_nCondition != PLAY) {
    m_nCondition = PAUSE;
    m_bContinue = false;
    while(!m_bContinue) m_Continue.wait(lock);
  }
}

void SearchControl::postClear(VizType nType) {
  boost::unique_lock<boost::mutex> lock(m_Mutex);
  if(m_nCondition != PLAY) {
    m_nCondition = PAUSE;
    std::cout << "blocking for " << nType << std::endl;
    block(nType, lock);
  }
  else {
    //std::cout << __FILE__ << "::" << __LINE__ << ": m_pState->unlock_shared(" << (int) nType << ")" << std::endl;
    m_pState->unlock_shared(nType);
    m_pState->unlock_shared_ptr(nType);
  }
}

void SearchControl::set(guint nKey, VizType nType) {
  m_pState->lock_shared_ptr(nType);
  m_pState->lock_shared(nType);
  m_Mutex.lock();
  switch(nKey) {
  case GDK_space:
    m_nCondition = ((m_nCondition == PLAY) ? PAUSE : PLAY);
    break;
  case GDK_Up:
    m_nCondition = PAUSE_AFTER_DEL;
    break;
  case GDK_Down:
    m_nCondition = PAUSE_NEXT_DEPTH;
    break;
  case GDK_Right:
    m_nCondition = PAUSE_AFTER_ADD;
    break;
  case GDK_Return:
    m_nCondition = PAUSE;
    break;
  default:
    m_Mutex.unlock();
    m_pState->unlock_shared(nType);
    m_pState->unlock_shared_ptr(nType);
    return;
    break;
  };
  m_nDepth = m_pState->stackSize(nType);
  m_nType = nType;
  m_bContinue = true;
  m_Continue.notify_all();
  m_Mutex.unlock();
  m_pState->unlock_shared(nType);
  m_pState->unlock_shared_ptr(nType);
}

void SearchControl::block(VizType nType,
			  boost::unique_lock<boost::mutex>& lock) {
  m_bContinue = false;
  m_pState->unlock_shared(nType);
  m_pState->unlock_shared_ptr(nType);
  while(!m_bContinue) m_Continue.wait(lock);
}

void SearchControl::set(LockableState* pState) {
  m_pState = pState;
}

void SearchControl::postCompress(VizType nType) {
  boost::unique_lock<boost::mutex> lock(m_Mutex);
  if(m_nCondition != PLAY && m_bBlockOnCompression) {
    m_nCondition = PAUSE;
    block(nType, lock);
  }
  else {
    m_pState->unlock_shared(nType);
    m_pState->unlock_shared_ptr(nType);
  }
}

void SearchControl::toggleCompression() {
  m_bBlockOnCompression = !m_bBlockOnCompression;
}

#endif // VIZ
