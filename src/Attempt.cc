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

#include "Attempt.h"

Attempt::Attempt() {
}

Attempt::Attempt(const Attempt& src) :
  m_Box(src.m_Box),
  m_bResult(src.m_bResult),
  m_Nodes(src.m_Nodes),
  m_Time(src.m_Time) {
}

Attempt::Attempt(const BoxDimensions& b, bool bResult) :
  m_Box(b),
  m_bResult(bResult) {
}

Attempt::Attempt(const BoxDimensions& b, bool bResult,
		 const NodeCount& nc, const TimeSpec& t) :
  m_Box(b),
  m_bResult(bResult),
  m_Nodes(nc),
  m_Time(t) {
}

Attempt::~Attempt() {
}

const Attempt& Attempt::operator=(const Attempt& src) {
  m_Box = src.m_Box;
  m_bResult = src.m_bResult;
  m_Nodes = src.m_Nodes;
  m_Time = src.m_Time;
  return(*this);
}

bool Attempt::operator<(const Attempt& rhs) const {
  if(m_Box.m_nArea != rhs.m_Box.m_nArea)
    return(m_Box.m_nArea < rhs.m_Box.m_nArea);
  if(m_bResult != rhs.m_bResult)
    return(m_bResult < rhs.m_bResult);
  if(m_Box.m_nHeight != rhs.m_Box.m_nHeight)
    return(m_Box.m_nHeight < rhs.m_Box.m_nHeight);
  if(m_Box.m_nWidth != rhs.m_Box.m_nWidth)
    return(m_Box.m_nWidth < rhs.m_Box.m_nWidth);
  if(m_Nodes != rhs.m_Nodes)
    return(m_Nodes < rhs.m_Nodes);
  return(m_Time < rhs.m_Time);
}
