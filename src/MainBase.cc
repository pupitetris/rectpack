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

#include "AnytimeBoxes.h"
#include "BoundingBoxes.h"
#include "ErrorInfo.h"
#include "Grid.h"
#include "GridViz.h"
#include "MainBase.h"
#include "ParallelBoxes.h"
#include "Performance.h"
#include "Rectangle.h"
#include "SquarePacker.h"
#include <algorithm>
#include <deque>
#include <list>
#include <set>

MainBase::MainBase() :
  m_pBoxes(NULL) {
}

MainBase::~MainBase() {
  deallocatePackers();
  delete m_pBoxes;
}

void MainBase::initialize() {
  srandom(m_Params.m_nRandom);

  /**
   * Instantiate the packer. The packer itself doesn't do any
   * computation at all about the problem instance.
   */

  deallocatePackers();
  if(m_Params.m_bKorf)
    m_vPackers.push_back(new SquarePacker());
  else if(!instantiatePacker()) {
    std::cout << "Packer " << m_Params.m_nWeakening << " doesn't exist."
	      << std::endl;
    exit(0);
  }

  /**
   * Before we conduct any other instantiations, we should check to
   * see if the packer we just instantiated requires the instance to
   * be discretized. If so, we should perform our discretization.
   */

  if(m_Params.m_s0.find_first_of('n') >= m_Params.m_s0.size())
    m_Params.rescaleInstance();
  if(m_Params.m_bVerbose) m_Params.print();

  /**
   * Instantiate the bounding box search engine.
   */

  delete m_pBoxes;
  if(m_Params.m_nThreads > 1)
    m_pBoxes = new ParallelBoxes();
  else {
    if(m_Params.m_nWeakening == 6)
      m_pBoxes = new AnytimeBoxes();
    else
      m_pBoxes = new BoundingBoxes();
  }
  m_pBoxes->initialize(m_vPackers);
  for(std::deque<Packer*>::const_iterator i = m_vPackers.begin();
      i != m_vPackers.end(); ++i) {
    (*i)->initialize(m_pBoxes);
    (*i)->initialize(&m_Params);
  }
  m_pBoxes->initialize(&m_Params);
}

bool MainBase::instantiatePacker() {
  deallocatePackers();
  for(int i = 0; i < m_Params.m_nThreads; ++i) {
    m_vPackers.push_back(Packer::create(&m_Params));
    if(m_vPackers.back() == NULL) {
      std::cout << "MainBase::instantiatePacker: Failed to instantiate packer. Aborting."
		<< std::endl;
      exit(0);
    }
  }
  return(!m_vPackers.empty());
}

void MainBase::run() {
  if(!m_Params.m_sRegression.empty())
    runRegression();
  else if(m_Params.m_nOrdering == 0)
    runOrdering();
  else if(m_Params.m_nWeakening >= 0)
    m_pBoxes->run();
}

void MainBase::deallocatePackers() {
  while(!m_vPackers.empty()) {
    delete m_vPackers.back();
    m_vPackers.pop_back();
  }
}

void MainBase::runRegression() {
  Parameters pk, p2;
  std::list<ErrorInfo> lErrors;
  BoundingBoxes kb;
  SquarePacker korf;
  std::cout << "----------------------------------------------------------------------"
	    << std::endl << ">>> Regression invoked with: "
	    << m_Params.m_sCommand << std::endl;
  for(size_t i = 1; i <= m_Params.m_nTrials; ++i) {
    std::cout << ">>> Beginning Trial " << i << '/' << m_Params.m_nTrials
	      << " with Rich's packer." << std::endl << std::endl;
    m_Params.rerollRegression();
    std::cout << "Instance: ";
    m_Params.m_vInstance.print();
    if(m_Params.m_s0.find_first_of('n') >= m_Params.m_s0.size())
      m_Params.rescaleInstance();
    std::cout << std::endl << std::endl;

    /**
     * Link the packers with the bounding boxes.
     */

    pk = m_Params;
    if(pk.m_sRegression.find_first_of('e') < pk.m_sRegression.size() &&
       pk.m_s1.find_first_of('d') >= pk.m_s1.size())
      pk.m_s1.push_back('d');
    if(pk.m_sRegression.find_first_of('x') < pk.m_sRegression.size() &&
       pk.m_s0.find_first_of('m') >= pk.m_s0.size())
      pk.m_s0.push_back('m');
    korf.initialize(&kb);
    kb.initialize(&korf);
    korf.initialize(&pk);
    kb.initialize(&pk);
    for(std::deque<Packer*>::iterator j = m_vPackers.begin();
	j != m_vPackers.end(); ++j)
      (*j)->initialize(m_pBoxes);
    m_pBoxes->initialize(m_vPackers);

    /**
     * Initialize the data structures with the parameters.
     */

    p2 = m_Params;
    for(std::deque<Packer*>::iterator j = m_vPackers.begin();
	j != m_vPackers.end(); ++j)
      (*j)->initialize(&p2);
    m_pBoxes->initialize(&p2);

    /**
     * Now run the computation.
     */

    kb.run();
    std::cout << ">>> And now with the other packer: "
	      << std::endl << std::endl;
    m_pBoxes->run();
    std::cout << std::endl
	      << "----------------------------------------------------------------------"
	      << std::endl << ">>> Trial " << i << '/'
	      << m_Params.m_nTrials << " complete." << std::endl;

    /**
     * Compare successes and failures.
     */

    for(std::set<Attempt>::const_iterator j = m_pBoxes->m_sAttempts.begin();
	j != m_pBoxes->m_sAttempts.end(); ++j) {
      std::set<Attempt>::iterator k = kb.m_sAttempts.find(*j);
      if(k == kb.m_sAttempts.end())
	lErrors.push_back(ErrorInfo(j->m_Box, m_Params.m_vInstance,
				    true, j->m_bResult, false, true));
      else if(k->m_bResult != j->m_bResult) {
	std::cout << ">>> Error: " << j->m_Box.m_nWidth << 'x'
		  << j->m_Box.m_nHeight << " should be "
		  << (k->m_bResult ? "feasible." : "infeasible.")
		  << std::endl << std::endl;
	lErrors.push_back(ErrorInfo(j->m_Box, m_Params.m_vInstance,
				    k->m_bResult, j->m_bResult));
      }
    }
    for(std::set<Attempt>::const_iterator j = kb.m_sAttempts.begin();
	j != kb.m_sAttempts.end(); ++j) {
      std::set<Attempt>::iterator k = m_pBoxes->m_sAttempts.find(*j);
      if(k == m_pBoxes->m_sAttempts.end())
	lErrors.push_back(ErrorInfo(j->m_Box, m_Params.m_vInstance,
				    j->m_bResult, true, true, false));
    }
  }
  std::cout << ">>> All trials complete. ";
  if(lErrors.empty())
    std::cout << "No errors were found.";
  else {
    std::cout << "These errors were found:" << std::endl;
    for(std::list<ErrorInfo>::iterator i = lErrors.begin();
	i != lErrors.end(); ++i)
      std::cout << *i << std::endl;
  }
  std::cout << std::endl << std::endl;
}

void MainBase::runOrdering() {

  /**
   * Estimate how much work we will be doing.
   */

  unsigned long long nTrials(1);
  for(size_t i = 2; i <= m_Params.m_vInstance.size(); ++i)
    nTrials *= i;

  /**
   * The number of items we will be keeping in our two sorted sets.
   */

  const size_t nEntries(20);
  typedef std::multiset<Performance> PerformanceSet;
  PerformanceSet mMost, mFewest;
  Packer* m_pPacker = m_vPackers.front();

  /**
   * Start with the lexicographically smallest arrangement, and
   * iterate over all permutations until we get to the largest.
   */

  m_Params.m_vInstance.sortIncreasingArea();
  m_Params.m_vInstance.inferInstanceProperties();
  m_Params.m_vInstance.m_bSequential = false;
  unsigned long long nCurrent(0);
  do {

    /**
     * Print the current attempt.
     */

    std::cout << "Trial " << ++nCurrent << "/" << nTrials << ": ";
    m_Params.m_vInstance.print();
    std::cout << std::endl;

    /**
     * Collect the data for a single run.
     */

    m_pPacker->initialize(&m_Params);
    m_pBoxes->initialize(&m_Params);
    m_pPacker->initialize(m_pBoxes);
    m_pBoxes->initialize(m_pPacker);
    m_pBoxes->run();

    /**
     * Save the data and update our top best and bottom worst lists.
     */

    mMost.insert(Performance(*m_pBoxes, m_Params.m_vInstance));
    mFewest.insert(Performance(*m_pBoxes, m_Params.m_vInstance));
    if(mFewest.size() > nEntries) {
      PerformanceSet::iterator j(mFewest.end());
      mFewest.erase(--j);
    }
    if(mMost.size() > nEntries)
      mMost.erase(mMost.begin());
  } while(std::next_permutation(m_Params.m_vInstance.begin(),
				m_Params.m_vInstance.end()));

  /**
   * Ranking by most nodes generated.
   */

  std::cout << "Most nodes generated variable orders:" << std::endl;
  std::cout << "Rank,";
  if(!mMost.empty()) mMost.begin()->printHeader();
  std::cout << std::endl;
  size_t nCounter = 0;
  for(PerformanceSet::const_reverse_iterator i = mMost.rbegin();
      i != mMost.rend(); ++i)
    std::cout << ++nCounter << ',' << *i << std::endl;
  std::cout << std::endl;

  /**
   * Ranking by fewest nodes generated.
   */

  std::cout << "Fewest nodes generated variable orders:" << std::endl;
  std::cout << "Rank,";
  if(!mFewest.empty()) mFewest.begin()->printHeader();
  std::cout << std::endl;
  nCounter = 0;
  for(PerformanceSet::const_iterator i = mFewest.begin();
      i != mFewest.end(); ++i)
    std::cout << ++nCounter << ',' << *i << std::endl;
}

void MainBase::print(size_t nIndent) const {
  std::string s(nIndent, ' ');
}
