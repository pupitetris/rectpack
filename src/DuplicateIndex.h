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

#ifndef DUPLICATEINDEX_H
#define DUPLICATEINDEX_H

#include "RectPtrArray.h"

class Parameters;

/**
 * Data structure that tells us which rectangles are duplicates of
 * which other rectangles. In particular, you may query, given your
 * current rectangle, what is the next or previous duplicate of this
 * rectangle in the given sequence. We assume that optimistically if
 * we have an oriented rectangle of wxh and an unoriented one of wxh
 * that they are duplicates. Of course, based on bounding box
 * constraints, this might be an exact estimate if the unoriented one
 * were forced to a particular orientation.
 */

class DuplicateIndex {
 public:
  DuplicateIndex();
  ~DuplicateIndex();

  /**
   * This should be called after the instantiation of the
   * rectangles. It is typically not necessary to reinitialize after
   * every bounding box. Even if the bounding box were to constrain
   * some rectangles into particular orientations, notice that whether
   * or not two rectangles were duplicates of each other can't change!
   */

  void initialize(const Parameters* p, RectPtrArray& r);
  Rectangle* next(const Rectangle* r) const;
  Rectangle* prev(const Rectangle* r) const;
  bool first(const Rectangle* r) const;
  bool last(const Rectangle* r) const;

 private:
  RectPtrArray m_vNext;
  RectPtrArray m_vPrev;
}; // DuplicateIndex

#endif // DUPLICATEINDEX_H
