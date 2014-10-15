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

#ifndef MUTEXMAP_H
#define MUTEXMAP_H

#include <map>

class Rectangle;

/**
 * A container representing a mapping from the given rectangle pointer
 * to its required orientation (the second number in the pair). In the
 * second number, 0 means the original orientation, 1 means rotated,
 * and 2 means unoriented.
 */

class MutexMap : public std::map<const Rectangle*, size_t> {
 public:
  MutexMap();
  ~MutexMap();
  void print() const;
};

#endif // MUTEXMAP_H
