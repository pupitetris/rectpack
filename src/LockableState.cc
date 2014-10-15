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
#include "GridViz.h"
#include "LockableState.h"

LockableState::LockableState() :
  m_vLockable(4, false),
  m_pYI(NULL),
  m_pYF(NULL),
  m_pXI(NULL),
  m_pXF(NULL) {
}

LockableState::~LockableState() {
}

void LockableState::enable(VizType n) {
  m_vLockable[n] = true;
}

void LockableState::disable(VizType n) {
  m_vLockable[n] = false;
}

void LockableState::set(SearchControl* pControl) {
  if(m_pYI) m_pYI->m_pControl = pControl;
  if(m_pYF) m_pYF->m_pControl = pControl;
  if(m_pXI) m_pXI->m_pControl = pControl;
  if(m_pXF) m_pXF->m_pControl = pControl;
}

void LockableState::set(BinsViz* pBins, VizType n) {
  pBins->m_nType = n;
  switch(n) {
  case YI: m_pYI = pBins; break;
  case YF: m_pYF = pBins; break;
  default: break;
  };
}

void LockableState::set(GridViz* pGrid, VizType n) {
  pGrid->m_nType = n;
  switch(n) {
  case XI: m_pXI = pGrid; break;
  case XF: m_pXF = pGrid; break;
  default: break;
  };
}

BinsViz* LockableState::getBins(VizType n) {
  switch(n) {
  case YI: return(m_pYI);
  case YF: return(m_pYF);
  default: return(NULL);
  };
}

GridViz* LockableState::getGrid(VizType n) {
  switch(n) {
  case XI: return(m_pXI);
  case XF: return(m_pXF);
  default: return(NULL);
  };
}

void LockableState::lock_ptr(VizType n) {
  if(m_vLockable[n])
    m_vPointers[n].lock();
}

void LockableState::lock_shared_ptr(VizType n) {
  if(m_vLockable[n])
    m_vPointers[n].lock_shared();
}

void LockableState::unlock_ptr(VizType n) {
  if(m_vLockable[n])
    m_vPointers[n].unlock();
}

void LockableState::unlock_shared_ptr(VizType n) {
  if(m_vLockable[n])
    m_vPointers[n].unlock_shared();
}

void LockableState::lock(VizType n) {
  switch(n) {
  case YI: if(m_pYI) {
      //std::cout << __FILE__ << "::" << __LINE__ << ": m_pYI->m_Mutex.lock()" << std::endl;
      m_pYI->m_Mutex.lock(); break;
    }
  case YF: if(m_pYF) m_pYF->m_Mutex.lock(); break;
  case XI: if(m_pXI) m_pXI->m_Mutex.lock(); break;
  case XF: if(m_pXF) {
      //std::cout << __FILE__ << "::" << __LINE__ << ": m_Mutex.lock()" << std::endl;
      m_pXF->m_Mutex.lock(); break;
    }
  default: break;
  };
}

void LockableState::lock_shared(VizType n) {
  switch(n) {
  case YI: if(m_pYI) {
      //std::cout << __FILE__ << "::" << __LINE__ << ": m_pYI->m_Mutex.lock_shared()" << std::endl;
      m_pYI->m_Mutex.lock_shared(); break;
    }
  case YF: if(m_pYF) m_pYF->m_Mutex.lock_shared(); break;
  case XI: if(m_pXI) m_pXI->m_Mutex.lock_shared(); break;
  case XF: if(m_pXF) {
      //std::cout << __FILE__ << "::" << __LINE__ << ": m_Mutex.lock_shared()" << std::endl;
      m_pXF->m_Mutex.lock_shared(); break;
    }
  default: break;
  };
}

void LockableState::unlock(VizType n) {
  switch(n) {
  case YI: if(m_pYI) {
      //std::cout << __FILE__ << "::" << __LINE__ << ": m_pYI->m_Mutex.unlock()" << std::endl;
      m_pYI->m_Mutex.unlock(); break;
    }
  case YF: if(m_pYF) m_pYF->m_Mutex.unlock(); break;
  case XI: if(m_pXI) m_pXI->m_Mutex.unlock(); break;
  case XF: if(m_pXF) {
      //std::cout << __FILE__ << "::" << __LINE__ << ": m_Mutex.unlock()" << std::endl;
      m_pXF->m_Mutex.unlock(); break;
    }
  default: break;
  };
}

void LockableState::unlock_shared(VizType n) {
  switch(n) {
  case YI: if(m_pYI) {
      //std::cout << __FILE__ << "::" << __LINE__ << ": m_pYI->m_Mutex.unlock_shared()" << std::endl;
      m_pYI->m_Mutex.unlock_shared(); break;
    }
  case YF: if(m_pYF) m_pYF->m_Mutex.unlock_shared(); break;
  case XI: if(m_pXI) m_pXI->m_Mutex.unlock_shared(); break;
  case XF: if(m_pXF) {
      //std::cout << __FILE__ << "::" << __LINE__ << ": m_pXF->m_Mutex.unlock_shared()" << std::endl;
      m_pXF->m_Mutex.unlock_shared(); break;
    }
  default: break;
  };
}

int LockableState::stackSize(VizType n) const {
  switch(n) {
  case YI: return(m_pYI->stackSize());
  case YF: return(m_pYF->stackSize());
  case XI: return(m_pXI->stackSize());
  case XF: return(m_pXF->stackSize());
  default: return(-1);
  };
}

void LockableState::set(BinsBase* pBins, VizType n) {
}

void LockableState::set(Grid* pGrid, VizType n) {
}

#endif // VIZ
