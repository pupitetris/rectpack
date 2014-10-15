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

#include "ErrorInfo.h"

ErrorInfo::ErrorInfo() :
  m_bKorfAttempted(false),
  m_bMineAttempted(false) {
}

ErrorInfo::ErrorInfo(const RDimensions& b,
		     const std::deque<RDimensions>& v, bool bKorf,
		     bool bMine) :
  m_Box(b),
  m_vInstance(v),
  m_bKorfFeasible(bKorf),
  m_bMineFeasible(bMine),
  m_bKorfAttempted(false),
  m_bMineAttempted(false) {
}

ErrorInfo::ErrorInfo(const RDimensions& b,
		     const std::deque<RDimensions>& v, bool bKorf,
		     bool bMine, bool bKorf2, bool bMine2) :
  m_Box(b),
  m_vInstance(v),
  m_bKorfFeasible(bKorf),
  m_bMineFeasible(bMine),
  m_bKorfAttempted(bKorf2),
  m_bMineAttempted(bMine2) {
}

ErrorInfo::~ErrorInfo() {
}

std::ostream& operator<<(std::ostream& os, const ErrorInfo& ei) {
  os << ei.m_Box;
  if(ei.m_bKorfAttempted != ei.m_bMineAttempted) {
    if(ei.m_bKorfAttempted)
      os << " Rich tried but you didn't.";
    else
      os << " you tried but Rich didn't.";
    os << " Instance was:";
  }
  else {
    os << " should be "
       << (ei.m_bKorfFeasible ? "feasible" : "infeasible")
       << " with squares";
  }
  std::deque<RDimensions>::const_iterator i = ei.m_vInstance.begin();
  if(i != ei.m_vInstance.end())
    os << *i;
  for(++i; i != ei.m_vInstance.end(); ++i)
    os << ',' << *i;
  return(os);
}
