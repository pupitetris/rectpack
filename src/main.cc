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

#include <iostream>
#include <numeric>
#include "BoundingBoxes.h"
#include "Domination.h"
#include "Globals.h"
#include "Packer.h"
#include "Parameters.h"
#include "MainBase.h"
#include "MainViz.h"
#include "TimeSpec.h"
#include <boost/algorithm/string.hpp>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>

/**
 * This function is called when the user terminates or interrupts the
 * process. It basically cleans up and writes a quick summary to the
 * screen.
 */

void terminate(int nSignal) {
  bQuit = true;
}

#include "MpqWrapper.h"

int main(int argc, char** argv) {

  /**
   * Get the starting (wall-clock) as well as the CPU time.
   */

  TimeSpec rt1, ct1;
  clock_gettime(CLOCK_REALTIME, &rt1);
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ct1);

  /**
   * Limit the amount of memory used by this program. If we exceed
   * 1GB then there is definitely a bug somewhere.
   */

  rlimit rl;
  rl.rlim_cur = rl.rlim_max = 1000000000;
  setrlimit(RLIMIT_AS, &rl);

  /**
   * Parse the parameters.
   */

#ifdef VIZ
  MainViz m;
#else
  MainBase m;
#endif // VIZ

#ifdef IJCAI
  if(m.m_Params.parseIJCAI(argc, argv))
    return(0);
#else // IJCAI
  if(m.m_Params.parseCommand(argc, argv) != 0)
    return(0);
#endif // IJCAI

  /**
   * Set up the signals to catch interruptions.
   */

#if NDEBUG
  if(m.m_Params.m_sRegression.empty()) {
    signal(SIGHUP, terminate);
    signal(SIGINT, terminate);
    signal(SIGTERM, terminate);
  }
#endif
  bQuit = false;

  m.initialize();
  m.run();

  /**
   * Get the ending time and compute the time elapsed.
   */

  TimeSpec rt2, ct2;
  clock_gettime(CLOCK_REALTIME, &rt2);
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ct2);

  rt2 -= rt1;
  ct2 -= ct1;

  /**
   * If we were interrupted, then we must subtract out the time
   * incurred by the previous unfinished bounding box. Notice that we 
   */

  if(bQuit) {

    /**
     * We need to also subtract out the CPU time incurred by the last
     * abandoned packing attempt.
     */

    ct2 -= m.m_pBoxes->m_pPacker->m_Time;
  }

  std::cout << std::endl
	    << "Computation finished. Measurements are as follows:"
	    << std::endl;

  /**
   * Print statistics. If we had a previous log we need to account
   * for, do it here.
   */
  
  size_t nAttempts(m.m_pBoxes->attempts());
  std::string sSolutions(m.m_pBoxes->solutions());
  if(!m.m_Params.m_Log.empty()) {
    LogState* pLog = &m.m_Params.m_Log;
    ct2 += pLog->m_nTotalCPU;
    m.m_pBoxes->m_pPacker->timeDomination() = pLog->m_nDominance;
    m.m_pBoxes->m_TotalTime += pLog->m_nYX;
    m.m_pBoxes->m_TotalYTime += pLog->m_nY;
    m.m_pBoxes->m_TotalXTime += pLog->m_nX;
    m.m_pBoxes->m_Total.accumulate(pLog->m_vNodes);
    m.m_pBoxes->m_pPacker->m_nCuSP += (size_t) pLog->m_nCuSP;
    nAttempts += (size_t) pLog->m_nBoxes;
    sSolutions = pLog->m_sSolutions + " " + sSolutions;
    boost::trim(sSolutions);
  }

  std::cout << std::endl;
  std::cout << "    Wall-Clock Time : " << rt2 << " (" << rt2.toString() << ')' << std::endl;
  std::cout << "     Total CPU Time : " << ct2 << " (" << ct2.toString() << ')' << std::endl;
  if(m.m_Params.m_sRegression.empty()) {
#ifndef IJCAI
    std::cout << "   Dominance Tables : " << m.m_pBoxes->m_pPacker->timeDomination() << " (" << m.m_pBoxes->m_pPacker->timeDomination().toString() << ')' << std::endl;
    std::cout << "       X+Y CPU Time : "
	      << m.m_pBoxes->m_TotalXTime
	      << " (" << m.m_pBoxes->m_TotalXTime.toString() << ')'
	      << " + "
	      << m.m_pBoxes->m_TotalYTime
	      << " (" << m.m_pBoxes->m_TotalYTime.toString() << ')'
	      << " = "
	      << m.m_pBoxes->m_TotalTime
	      << " (" << m.m_pBoxes->m_TotalTime.toString() << ')'
	      << std::endl;
    std::cout << "  XI,XF,YI,YF Nodes : "
	      << m.m_pBoxes->m_Total.get(XI) << ", "
	      << m.m_pBoxes->m_Total.get(XF) << ", "
	      << m.m_pBoxes->m_Total.get(YI) << ", "
	      << m.m_pBoxes->m_Total.get(YF) << std::endl;
#endif // IJCAI
    std::cout << "        Total Nodes : " << m.m_pBoxes->m_Total.total() << std::endl;
#ifndef IJCAI
    std::cout << "       Nodes/Second : " << (unsigned long long) (m.m_pBoxes->m_Total.total() / m.m_pBoxes->m_TotalTime.toDouble()) << std::endl;
    std::cout << "     CuSP Solutions : " << m.m_pBoxes->m_pPacker->m_nCuSP << std::endl;
    std::cout << "       Boxes Tested : " << nAttempts << std::endl;
    std::cout << "  Optimal Solutions : " << sSolutions << std::endl;
#endif // IJCAI
  }
  std::cout << std::endl;
  return(0);
}
