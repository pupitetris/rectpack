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
#include "LessRatioWHL.h"
#include "RDimensions.h"

LessRatioWHL::LessRatioWHL() {
}

LessRatioWHL::~LessRatioWHL() {
}

bool LessRatioWHL::operator()(const Dimensions& d1,
			     const Dimensions& d2) const {
  if(d1.ratioW() != d2.ratioW())
    return(d1.ratioW() < d2.ratioW());
  if(d1.ratioH() != d2.ratioH())
    return(d1.ratioH() < d2.ratioH());
  return(d1.ratioL() < d2.ratioL());
}

bool LessRatioWHL::operator()(const RDimensions& d1,
			     const RDimensions& d2) const {
  if(d1.ratioW() != d2.ratioW())
    return(d1.ratioW() < d2.ratioW());
  if(d1.ratioH() != d2.ratioH())
    return(d1.ratioH() < d2.ratioH());
  return(d1.ratioL() < d2.ratioL());
}
