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

#ifndef ATTEMPT_H
#define ATTEMPT_H

#include "RDimensions.h"
#include "NodeCount.h"
#include "TimeSpec.h"

class Attempt;
class BoxDimensions;

class Attempt {
 public:
  Attempt();
  Attempt(const Attempt& src);
  Attempt(const BoxDimensions& b, bool bResult);
  Attempt(const BoxDimensions& b, bool bResult, const NodeCount& nc,
	  const TimeSpec& t);
  virtual ~Attempt();
  const Attempt& operator=(const Attempt& src);
  bool operator<(const Attempt& rhs) const;
  RDimensions m_Box;
  bool m_bResult;
  NodeCount m_Nodes;
  TimeSpec m_Time;
};

#endif // ATTEMPT_H
