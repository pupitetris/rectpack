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

#include "AsciiGrid.h"
#include "NodeCount.h"
#include "Packer.h"
#include "ParallelBoxes.h"
#include "Parameters.h"
#include "Placements.h"
#include <boost/bind.hpp>
#include <limits>

ParallelBoxes::ParallelBoxes() :
  m_pWorkers(NULL),
  m_bSolutionFound(false) {
}

ParallelBoxes::~ParallelBoxes() {
  delete[] m_pWorkers;
}

void ParallelBoxes::initialize(const Parameters* pParams) {
  BoundingBoxes::initialize(pParams);
  m_pPacker = NULL;
}

bool ParallelBoxes::run() {

  m_sAttempts.clear();
  m_Total.clear();
  m_bSolutionFound = false;
  m_nMaxArea = std::min(m_pParams->m_vInstance.m_nStacked.m_nWidth.get_ui() *
			m_pParams->m_vInstance.m_nMax.m_nHeight.get_ui(),
			m_pParams->m_vInstance.m_nStacked.m_nHeight.get_ui() *
			m_pParams->m_vInstance.m_nMax.m_nWidth.get_ui());
  m_TotalTime = 0;

  BoxDimensions b;

  /**
   * Create the threads and set them to run. As we create the threads,
   * we'll assign the packer to that thread as well. Note that none of
   * the threads will proceed until we finally unlock the mutex.
   */

  {
    boost::unique_lock<boost::mutex> lock(m_Mutex);
    delete[] m_pWorkers;
    m_pWorkers = new boost::thread[m_vPackers.size()];
    for(size_t i = 0; i < m_vPackers.size(); ++i) {
      boost::thread t(boost::bind(&ParallelBoxes::runThread, this));
      m_pWorkers[i].swap(t);
      m_mAssignment[m_pWorkers[i].get_id()] = m_vPackers[i];
    }
    
    /**
     * Enter the polling loop.
     */
    
    while(!m_bSolutionFound) m_SolutionFound.wait(lock);
  }

  /**
   * Someone found a solution. So what we'll do is to send a
   * termination signal to every thread that is currently searching a
   * box that is larger than the current bound.
   *
   * @todo It appears that if we have several threads working on
   * different bounding boxes lower than the upper bound, and one of
   * them can tighten the upper bound, we don't loop to terminate the
   * other threads! Technically we should continue to loop, sending
   * termination signals to anybody over the upper bound, until we're
   * done. In fact, we can only exit this termination loop if all the
   * running threads are working on boxes EQUAL to the upper bound,
   * because they are essentially additional solutions to the optimal
   * one.
   */

  for(size_t i = 0; i < m_vPackers.size(); ++i) {
    m_vPackers[i]->m_BoxMutex.lock();
    if(m_vPackers[i]->m_Box.m_nArea > m_nMaxArea)
      m_pWorkers[i].interrupt();
    m_vPackers[i]->m_BoxMutex.unlock();
  }
  
  /**
   * Join with all of the threads.
   */

  for(size_t i = 0; i < m_vPackers.size(); ++i)
    if(m_pWorkers[i].joinable()) m_pWorkers[i].join();

  /**
   * Find the optimal solutions.
   */

  std::set<Attempt>::iterator i = m_sAttempts.begin();
  for(; i != m_sAttempts.end(); ++i)
    if(i->m_Box.m_nArea >= (URational) m_nMaxArea)
      break;

  /**
   * Print them out.
   */

  std::cout << "Optimal Solutions: ";
  for(; i != m_sAttempts.end(); ++i)
    if(i->m_Box.m_nArea > (URational) m_nMaxArea)
      break;
    else if(i->m_bResult)
      std::cout << i->m_Box << "(" << i->m_Box.m_nArea << ")  ";
  std::cout << std::endl;

  /**
   * Erase the rest.
   */

  for(std::set<Attempt>::iterator j = i; j != m_sAttempts.end(); ++j) {
    NodeCount nc(j->m_Nodes);
    m_Total.decumulate(nc);
    m_TotalTime -= j->m_Time;
  }
  m_sAttempts.erase(i, m_sAttempts.end());
  return(true);
}

void ParallelBoxes::runThread() {

  /**
   * Box dimensions and timing structures that we need.
   */

  HeapBox hb;

  /**
   * As long as we should continue, grab the next box to solve, solve
   * it, and report the results. Make sure we also grab the packer.
   */

  m_Mutex.lock();
  Packer* pPacker = getPacker();
  assert(pPacker != NULL);

  while(getNext(hb) && boxValid(hb.m_Box)) {
    m_Mutex.unlock();

    /**
     * Print the current attempt.
     */

    m_Console.lock();
    printStarting(hb.m_Box);
    m_Console.unlock();

    /**
     * Try the packing attempt.
     */

    bool bResult = pPacker->pack(hb);

    /**
     * Since we just returned from our packing attempt, it's entirely
     * possible that we've found the optimal solution and so our
     * current results don't matter anymore. Let's check before we add
     * our current results to the record.
     */

    m_Mutex.lock();

    /**
     * Update the results and accumulate the nodes.
     */

    m_sAttempts.insert(Attempt(hb.m_Box, bResult, pPacker->m_Nodes, pPacker->m_Time));
    printid();
    
    m_Total.accumulate(pPacker->m_Nodes);
    m_TotalTime += pPacker->m_Time;

    /**
     * Print the results.
     */

    m_Console.lock();
    if(bResult) {
      if(!m_pParams->m_bQuiet) {
	AsciiGrid ag;
	Placements v;
	m_pPacker->get(v);
	ag.draw(m_pParams, v);
	ag.print();
	std::cout << "  Rectangles were placed in these (x,y) locations:"
		  << std::endl;
	v *= m_pParams->m_vInstance.m_nScale;
	v.print();
      }
      m_nMaxArea = std::min(m_nMaxArea, hb.m_Box.m_nArea);
      m_bSolutionFound = true;
      m_SolutionFound.notify_all();
    }
    else {
      std::cout << "Finished " << hb.m_Box.m_nWidth << " X "
		<< hb.m_Box.m_nHeight
		<< " = " << hb.m_Box.m_nArea
		<< " (infeasible)." << std::endl;
      pPacker->printNodes();
    }
    m_Console.unlock();
  }
  m_Mutex.unlock();
}

void ParallelBoxes::set(const std::deque<Packer*>& vPackers) {
  m_vPackers = vPackers;
  delete[] m_pWorkers;
  m_pWorkers = new boost::thread[m_vPackers.size()];
}

bool ParallelBoxes::boxValid(const BoxDimensions& b) const {
  return(b.m_nArea <= m_nMaxArea);
}

Packer* ParallelBoxes::getPacker() {
  std::map<boost::thread::id, Packer*>::iterator i =
    m_mAssignment.find(boost::this_thread::get_id());
  if(i == m_mAssignment.end()) return(NULL);
  return(i->second);
}

void ParallelBoxes::printStarting(const BoxDimensions& b) const {
  printid();
  std::cout << "Starting " << b.m_nWidth << " X " << b.m_nHeight
	    << " = " << b.m_nArea
	    << std::endl << std::endl;
}

void ParallelBoxes::printid() const {
  std::cout << boost::this_thread::get_id()
	    << " ------------------------------------------------------------"
	    << std::endl;
}
