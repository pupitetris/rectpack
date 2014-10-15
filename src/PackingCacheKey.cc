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

#include "PackingCacheKey.h"
#include "RectDecDeq.h"
#include "RectDecorator.h"

PackingCacheKey::PackingCacheKey() {
}

PackingCacheKey::PackingCacheKey(const BoxDimensions& b,
				 const std::deque<const Rectangle*>& v) :
  m_Box(b),
  m_vRectangles(v) {
}

PackingCacheKey::PackingCacheKey(const BoxDimensions& b,
				 const RectDecDeq& v) :
  m_Box(b) {
  for(RectDecDeq::const_iterator i = v.begin();
      i != v.end(); ++i)
    m_vRectangles.push_back((*i)->m_pRect);
}

PackingCacheKey::~PackingCacheKey() {
}

bool PackingCacheKey::operator<(const PackingCacheKey& rhs) const {
  if(m_Box != rhs.m_Box) {
    if(m_Box.m_nWidth != rhs.m_Box.m_nWidth)
      return(m_Box.m_nWidth < rhs.m_Box.m_nWidth);
    else
      return(m_Box.m_nHeight < rhs.m_Box.m_nHeight);
  }
  if(m_vRectangles != rhs.m_vRectangles)
    return(m_vRectangles < rhs.m_vRectangles);
  return(false);
}
