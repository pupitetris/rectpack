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

#ifndef GREATERHEIGHT_H
#define GREATERHEIGHT_H

class Dimensions;
class RDimensions;
class Rectangle;

class GreaterHeight {
 public:
  GreaterHeight();
  ~GreaterHeight();
  bool operator()(const Rectangle* r1, const Rectangle* r2) const;
  bool operator()(const Dimensions& d1, const Dimensions& d2) const;
  bool operator()(const RDimensions& d1, const RDimensions& d2) const;
};

#endif // GREATERHEIGHT_H
