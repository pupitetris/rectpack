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

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "LogState.h"
#include <fstream>

LogState::LogState() {
}

LogState::~LogState() {
}

bool LogState::empty() const {
  return(m_lLog.empty());
}

bool LogState::load(const std::string& s) {
  if(s.empty()) return(false);
  std::ifstream ifs(s.c_str());
  if(!ifs) {
    std::cout << "Unable to open the previous log file."
	      << std::endl;
    return(false);
  }

  /**
   * Read in the entire log.
   */

  m_lLog.clear();
  while(!ifs.eof()) {
    char pLine[1024];
    ifs.getline(pLine, 1024);
    m_lLog.push_back(std::string(pLine));
  }

  /**
   * Now parse all of the information. First find the command line.
   */

  std::list<std::string>::iterator i = m_lLog.begin();

  for(; i != m_lLog.end(); ++i) {
    size_t n;
    if((n = i->find("Command line: ")) < i->size()) {
      m_vCommand.clear();
      std::string cmd(i->substr(n + sizeof("Command line: ") - 1));
      boost::trim(cmd);
      boost::split(m_vCommand, cmd, boost::is_any_of(std::string(" ")));
      break;
    }
  }

  if(i == m_lLog.end()) {
    std::cout << "Unable to extract previous command line." << std::endl;
    return(false);
  }

  /**
   * Next we want to know how insure that this is in fact a
   * continuable log, by finding where we interrupted it.
   */

  for(; i != m_lLog.end(); ++i) {
    if(i->find("Trying") < i->size()) m_iNextBox = i;
    if(i->find("Interrupted") < i->size())
      break;
  }
  if(i == m_lLog.end()) {
    std::cout << "Unable to find the interruption point." << std::endl;
    return(false);
  }

  /**
   * Next read in the previous times that have elapsed.
   */
  
  for(; i != m_lLog.end(); ++i)
    if(i->find("Total CPU Time") < i->size()) {
      if(!getSeconds(*i, m_nTotalCPU)) {
	std::cout << "Unable to parse line Total CPU Time." << std::endl;
	return(false);
      }
      break;
    }
  for(; i != m_lLog.end(); ++i)
    if(i->find("Dominance Tables") < i->size()) {
      if(!getSeconds(*i, m_nDominance)) {
	std::cout << "Unable to parse line Domination Tables." << std::endl;
	return(false);
      }
      break;
    }
  for(; i != m_lLog.end(); ++i)
    if(i->find("Y+X CPU Time") < i->size()) {
      if(!getSeconds(*i, m_nYX)) {
	std::cout << "Unable to parse line Y+X CPU Time." << std::endl;
	return(false);
      }
      break;
    }
  for(; i != m_lLog.end(); ++i)
    if(i->find("Y CPU Time") < i->size()) {
      if(!getSeconds(*i, m_nY)) {
	std::cout << "Unable to parse line Y CPU Time." << std::endl;
	return(false);
      }
      break;
    }
  for(; i != m_lLog.end(); ++i)
    if(i->find("X CPU Time") < i->size()) {
      if(!getSeconds(*i, m_nX)) {
	std::cout << "Unable to parse line X CPU Time." << std::endl;
	return(false);
      }
      break;
    }
  for(; i != m_lLog.end(); ++i)
    if(i->find("YI Nodes") < i->size()) {
      unsigned long long n;
      if(!getNodes(*i, n)) {
	std::cout << "Unable to parse line YI Nodes." << std::endl;
	return(false);
      }
      m_vNodes.set(YI, n);
      break;
    }
  for(; i != m_lLog.end(); ++i)
    if(i->find("YF Nodes") < i->size()) {
      unsigned long long n;
      if(!getNodes(*i, n)) {
	std::cout << "Unable to parse line YF Nodes." << std::endl;
	return(false);
      }
      m_vNodes.set(YF, n);
      break;
    }
  for(; i != m_lLog.end(); ++i)
    if(i->find("XI Nodes") < i->size()) {
      unsigned long long n;
      if(!getNodes(*i, n)) {
	std::cout << "Unable to parse line XI Nodes." << std::endl;
	return(false);
      }
      m_vNodes.set(XI, n);
      break;
    }
  for(; i != m_lLog.end(); ++i)
    if(i->find("XF Nodes") < i->size()) {
      unsigned long long n;
      if(!getNodes(*i, n)) {
	std::cout << "Unable to parse line XF Nodes." << std::endl;
	return(false);
      }
      m_vNodes.set(XF, n);
      break;
    }
  for(; i != m_lLog.end(); ++i)
    if(i->find("CuSP Solutions") < i->size()) {
      if(!getNodes(*i, m_nCuSP)) {
	std::cout << "Unable to parse line CuSP Solutions." << std::endl;
	return(false);
      }
      break;
    }
  for(; i != m_lLog.end(); ++i)
    if(i->find("Boxes Tested") < i->size()) {
      if(!getNodes(*i, m_nBoxes)) {
	std::cout << "Unable to parse line Boxes Tested." << std::endl;
	return(false);
      }
      break;
    }
  for(; i != m_lLog.end(); ++i)
    if(i->find("Optimal Solution") < i->size()) {
      if(!getString(*i, m_sSolutions)) {
	std::cout << "Unable to parse line Optimal Solution." << std::endl;
	return(false);
      }
      break;
    }
  return(true);
}

bool LogState::getString(const std::string& s, std::string& s2) const {
  size_t m = s.find(": ");
  if(m >= s.size()) return(false);
  s2 = s.substr(m + 2);
  boost::trim(s2);
  return(true);
}

bool LogState::getSeconds(const std::string& s, TimeSpec& ts) const {
  size_t m = s.find(": ");
  size_t n = s.find("(");
  if(m >= s.size() || n >= s.size()) return(false);
  std::string secs(s.substr(m + 2, n - (m + 2)));
  boost::trim(secs);
  ts.tv_sec = boost::lexical_cast<time_t>(secs);
  return(true);
}

bool LogState::getNodes(const std::string& s, unsigned long long& n) const {
  size_t m = s.find(": ");
  if(m >= s.size()) return(false);
  std::string nodes(s.substr(m + 2));
  boost::trim(nodes);
  n = boost::lexical_cast<unsigned long long>(nodes);
  return(true);
}

void LogState::print() const {
  for(std::list<std::string>::const_iterator i = m_lLog.begin();
      i != m_iNextBox; ++i)
    std::cout << *i << std::endl;
}
