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

#include "OrderedCuSP.h"
#include "PlacementDec.h"

OrderedCuSP::OrderedCuSP() {
}

OrderedCuSP::~OrderedCuSP() {
}

bool OrderedCuSP::operator()(const PlacementDec* p1,
			     const PlacementDec* p2) const {

  /**
   * Sort first by the major coordinate. These rectangles must be
   * processed first.
   */

  if(p1->c1() != p2->c1())
    return(p1->c1() < p2->c1());

  /**
   * Then, we sort by the largest first dimension, so that it appears
   * that the longer ones are pulled down by gravity, and they stack
   * up smaller and smaller.
   */

  if(p1->d1() != p2->d1())
    return(p1->d1() > p2->d1());

  /**
   * If we're still tied, then we will sort by the one that is simply
   * larger in that second dimension.
   */

  if(p1->d2() != p2->d2())
    return(p1->d2() > p2->d2());

  /**
   * Finally just return an ordering just based on memory
   * allocation. It will make things slightly more deterministic,
   * which will be useful for debugging purposes.
   */

  return(p1 < p2);
}
