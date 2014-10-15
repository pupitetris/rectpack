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

#ifndef LOGSTATE_H
#define LOGSTATE_H

#include "NodeCount.h"
#include "TimeSpec.h"
#include <list>
#include <string>
#include <vector>

class LogState {
 public:
  LogState();
  ~LogState();
  bool load(const std::string& s);
  void print() const;
  bool empty() const;
  bool getSeconds(const std::string& s, TimeSpec& ts) const;
  bool getNodes(const std::string& s, unsigned long long& n) const;
  bool getString(const std::string& s, std::string& s2) const;

  std::list<std::string> m_lLog;
  std::list<std::string>::const_iterator m_iNextBox;

  std::vector<std::string> m_vCommand;
  TimeSpec m_nTotalCPU;
  TimeSpec m_nDominance;
  TimeSpec m_nYX;
  TimeSpec m_nY;
  TimeSpec m_nX;
  NodeCount m_vNodes;
  unsigned long long m_nCuSP;
  unsigned long long m_nBoxes;
  std::string m_sSolutions;
};

#endif // LOGSTATE_H
