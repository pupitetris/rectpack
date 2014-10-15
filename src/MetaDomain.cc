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

#include "MetaDomain.h"

MetaDomain::MetaDomain() {
  initialize();
}

MetaDomain::~MetaDomain() {
}

void MetaDomain::initialize() {
  clear();
  insert(ABOVE);
  insert(BELOW);
  insert(LEFTOF);
  insert(RIGHTOF);
}

bool MetaDomain::contains(ValueT n) const {
  return(find(n) != end());
}

std::ostream& operator<<(std::ostream& os, const MetaDomain& rhs) {
  os << '{';
  std::set<int>::const_iterator i = rhs.begin();
  if(!rhs.empty())
    os << rhs.toString(*i);
  if(rhs.size() > 1)
    for(++i; i != rhs.end(); ++i)
      os << ' ' << rhs.toString(*i);
  os << '}';
  return(os);
}

std::string MetaDomain::toString(int n) const {
  switch(n) {
  case MetaDomain::ABOVE:
    return(std::string("above"));
    break;
  case MetaDomain::BELOW:
    return(std::string("below"));
    break;
  case MetaDomain::LEFTOF:
    return(std::string("leftof"));
    break;
  case MetaDomain::RIGHTOF:
    return(std::string("rightof"));
    break;
  case MetaDomain::UNASSIGNED:
    break;
  default:
    return(std::string("unknown"));
    break;
  };
  return(std::string(""));
}
