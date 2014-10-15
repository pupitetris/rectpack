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

#include "RectDecArray.h"
#include "RectDecorator.h"
#include "RectPtrArray.h"

RectDecArray::RectDecArray() {
}

RectDecArray::~RectDecArray() {
  for(iterator i = begin(); i != end(); ++i)
    delete *i;
}

RectDecArray::RectDecArray(const RectPtrArray& ra,
			   const RectDecorator* rd) {
  initialize(ra, rd);
}

void RectDecArray::initialize(const RectPtrArray& ra,
			      const RectDecorator* rd) {
  for(iterator i = begin(); i != end(); ++i)
    delete *i;
  clear();
  reserve(ra.size());
  for(RectPtrArray::const_iterator i = ra.begin(); i != ra.end();
      ++i) {
    push_back(rd->clone());
    back()->initialize(*i);
  }
}
