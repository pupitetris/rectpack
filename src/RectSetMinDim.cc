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

#include "RectSetMinDim.h"
#include "Rectangle.h"

RectSetMinDim::RectSetMinDim() {
}

RectSetMinDim::~RectSetMinDim() {
}

size_t RectSetMinDim::erase(Rectangle* r) {
  if(r->m_nMinDim > 1)
    return(std::set<Rectangle*, GreaterMinDim>::erase(r));
  else
    return(0);
}

std::pair<RectSetMinDim::iterator, bool>
RectSetMinDim::insert(Rectangle* r) {
  if(r->m_nMinDim > 1)
    return(std::set<Rectangle*, GreaterMinDim>::insert(r));
  else
    return(std::pair<iterator, bool>(end(), false));
}

void RectSetMinDim::print() const {
  for(const_iterator i = begin(); i != end(); ++i) {
    (*i)->print();
    std::cout << " ";
  }
  std::cout << std::endl;
}
