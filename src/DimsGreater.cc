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

#include "DimsFunctor.h"
#include "DimsGreater.h"

DimsGreater::DimsGreater(const DimsFunctor* pDims) :
  m_pDims(pDims) {
}

DimsGreater::DimsGreater(const DimsGreater& src) :
  m_pDims(src.m_pDims) {
}

DimsGreater::~DimsGreater() {
}

bool DimsGreater::operator()(const Rectangle* r1,
			     const Rectangle* r2) const {
  return(operator()(*r1, *r2));
}

bool DimsGreater::operator()(const Rectangle& r1,
			     const Rectangle& r2) const {
  if(m_pDims->d1(r1) != m_pDims->d1(r2))
    return(m_pDims->d1(r1) > m_pDims->d1(r2));
  else return(m_pDims->d2(r1) > m_pDims->d2(r2));
}

bool DimsGreater::operator()(const RDimensions* r1,
			     const RDimensions* r2) const {
  return(operator()(*r1, *r2));
}

bool DimsGreater::operator()(const RDimensions& r1,
			     const RDimensions& r2) const {
  if(m_pDims->d1(r1) != m_pDims->d1(r2))
    return(m_pDims->d1(r1) > m_pDims->d1(r2));
  else return(m_pDims->d2(r1) > m_pDims->d2(r2));
}
