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

#include "Rectangle.h"
#include "RectArray.h"
#include "RectPtrArray.h"

RectPtrArray::RectPtrArray() {
}

RectPtrArray::~RectPtrArray() {
}

void RectPtrArray::initialize(RectArray& ra) {
  clear();
  reserve(ra.size());
  for(RectArray::iterator i = ra.begin(); i != ra.end(); ++i)
    push_back(&(*i));
}

bool RectPtrArray::overlaps(const Rectangle* r, const_iterator iEnd) const {
  for(const_iterator i = begin(); i != iEnd; ++i)
    if(r->overlaps(*i)) return(true);
  return(false);
}
