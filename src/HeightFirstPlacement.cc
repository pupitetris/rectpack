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

#include "HeightFirstPlacement.h"
#include "Placement.h"
#include "Rectangle.h"

HeightFirstPlacement::HeightFirstPlacement() {
}

HeightFirstPlacement::HeightFirstPlacement(Placement* pPlacement) :
  PlacementDec(pPlacement) {
}

HeightFirstPlacement::~HeightFirstPlacement() {
}

URational& HeightFirstPlacement::d1() const {
  return(m_pPlacement->m_Dims.m_nHeight);
}

URational& HeightFirstPlacement::d2() const {
  return(m_pPlacement->m_Dims.m_nHeight);
}

URational& HeightFirstPlacement::c1() const {
  return(m_pPlacement->m_nLocation.y);
}

URational& HeightFirstPlacement::c2() const {
  return(m_pPlacement->m_nLocation.x);
}

