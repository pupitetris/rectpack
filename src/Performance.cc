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

#include "BoundingBoxes.h"
#include "Performance.h"

Performance::Performance() {
}

Performance::~Performance() {
}

Performance::Performance(const BoundingBoxes& b,
			 const Instance& i) :
  m_Total(b.m_Total),
  m_vInstance(i) {
}

bool Performance::operator<(const Performance& rhs) const {
  if(m_Total.total() != rhs.m_Total.total())
    return(m_Total.total() < rhs.m_Total.total());
  else if(m_Total != rhs.m_Total)
    return(m_Total < rhs.m_Total);
  else if(m_vInstance != rhs.m_vInstance)
    return(m_vInstance < rhs.m_vInstance);
  else
    return(false);
}

void Performance::print() const {
  std::cout << *this;
}

void Performance::printHeader() const {
  std::cout << "Nodes,NodesYI,NodesYF,NodesXI,NodesXF";
  for(size_t i = 0; i < m_vInstance.size(); ++i)
    std::cout << ",r" << i;
}

std::ostream& operator<<(std::ostream& os,
			 const Performance& rhs) {
  os << rhs.m_Total.total() << ','
     << rhs.m_Total.get(YI) << ','
     << rhs.m_Total.get(YF) << ','
     << rhs.m_Total.get(XI) << ','
     << rhs.m_Total.get(XF);
  for(size_t i = 0; i < rhs.m_vInstance.size(); ++i)
    os << ',' << rhs.m_vInstance[i];
  return(os);
}
