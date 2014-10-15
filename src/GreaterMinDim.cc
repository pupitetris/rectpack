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

#include "Dimensions.h"
#include "GreaterMinDim.h"
#include "RDimensions.h"
#include "Rectangle.h"

GreaterMinDim::GreaterMinDim() {
}

GreaterMinDim::~GreaterMinDim() {
}

bool GreaterMinDim::operator()(const Rectangle* r1,
			       const Rectangle* r2) const {
  if(r1->m_nMinDim != r2->m_nMinDim)
    return(r1->m_nMinDim > r2->m_nMinDim);
  else
    return(r1 > r2);
}

bool GreaterMinDim::operator()(const Dimensions& d1,
			       const Dimensions& d2) const {
  return(d1.m_nMinDim > d2.m_nMinDim);
}

bool GreaterMinDim::operator()(const RDimensions& d1,
			       const RDimensions& d2) const {
  return(d1.m_nMinDim > d2.m_nMinDim);
}
