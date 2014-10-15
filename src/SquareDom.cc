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

#include "SquareDom.h"
#include "Rectangle.h"

#define MAXSIZE 36
#define MAXGAP 10

UInt SquareDom::m_pSquare[MAXSIZE][MAXGAP] = /* for each square and distance from edge */
  {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  /* 0 */
   {0, 1, 0, 0, 0, 0, 0, 0, 0, 0},  /* 1 */
   {0, 1, 1, 0, 0, 0, 0, 0, 0, 0},  /* 2 */
   {0, 1, 1, 1, 0, 0, 0, 0, 0, 0},  /* 3 */
   {0, 1, 1, 0, 1, 0, 0, 0, 0, 0},  /* 4 */
   {0, 1, 1, 1, 0, 0, 0, 0, 0, 0},  /* 5 */
   {0, 1, 1, 1, 0, 0, 0, 0, 0, 0},  /* 6 */
   {0, 1, 1, 1, 0, 0, 0, 0, 0, 0},  /* 7 */
   {0, 1, 1, 1, 0, 0, 0, 0, 0, 0},  /* 8 */
   {0, 1, 1, 1, 1, 0, 0, 0, 0, 0},  /* 9 */
   {0, 1, 1, 1, 1, 0, 0, 0, 0, 0},  /* 10 */
   {0, 1, 1, 1, 1, 0, 0, 0, 0, 0},  /* 11 */
   {0, 1, 1, 1, 1, 1, 0, 0, 0, 0},  /* 12 */
   {0, 1, 1, 1, 1, 1, 0, 0, 0, 0},  /* 13 */
   {0, 1, 1, 1, 1, 1, 0, 0, 0, 0},  /* 14 */
   {0, 1, 1, 1, 1, 1, 0, 0, 0, 0},  /* 15 */
   {0, 1, 1, 1, 1, 1, 0, 0, 0, 0},  /* 16 */
   {0, 1, 1, 1, 1, 1, 0, 0, 0, 0},  /* 17 */
   {0, 1, 1, 1, 1, 1, 1, 0, 0, 0},  /* 18 */
   {0, 1, 1, 1, 1, 1, 1, 0, 0, 0},  /* 19 */
   {0, 1, 1, 1, 1, 1, 1, 0, 0, 0},  /* 20 */
   {0, 1, 1, 1, 1, 1, 1, 0, 0, 0},  /* 21 */
   {0, 1, 1, 1, 1, 1, 1, 1, 0, 0},  /* 22 */
   {0, 1, 1, 1, 1, 1, 1, 1, 0, 0},  /* 23 */
   {0, 1, 1, 1, 1, 1, 1, 1, 0, 0},  /* 24 */
   {0, 1, 1, 1, 1, 1, 1, 1, 0, 0},  /* 25 */
   {0, 1, 1, 1, 1, 1, 1, 1, 0, 0},  /* 26 */
   {0, 1, 1, 1, 1, 1, 1, 1, 0, 0},  /* 27 */
   {0, 1, 1, 1, 1, 1, 1, 1, 0, 0},  /* 28 */
   {0, 1, 1, 1, 1, 1, 1, 1, 0, 0},  /* 29 */
   {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},  /* 30 */
   {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},  /* 31 */
   {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},  /* 32 */
   {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},  /* 33 */
   {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},  /* 34 */
   {0, 1, 1, 1, 1, 1, 1, 1, 1, 1}}; /* 35 */

UInt SquareDom::m_pSquareSize[MAXSIZE] =
  {0,  /* 0 */
   2,  /* 1 */
   3,  /* 2 */
   4,  /* 3 */
   5,  /* 4 */
   4,  /* 5 */
   4,  /* 6 */
   4,  /* 7 */
   4,  /* 8 */
   5,  /* 9 */
   5,  /* 10 */
   5,  /* 11 */
   6,  /* 12 */
   6,  /* 13 */
   6,  /* 14 */
   6,  /* 15 */
   6,  /* 16 */
   6,  /* 17 */
   7,  /* 18 */
   7,  /* 19 */
   7,  /* 20 */
   7,  /* 21 */
   8,  /* 22 */
   8,  /* 23 */
   8,  /* 24 */
   8,  /* 25 */
   8,  /* 26 */
   8,  /* 27 */
   8,  /* 28 */
   8,  /* 29 */
   9,  /* 30 */
   9,  /* 31 */
   9,  /* 32 */
   9,  /* 33 */
   9,  /* 34 */
   10}; /* 35 */

UInt SquareDom::m_pSquareSkip[MAXSIZE] =
  {0,  /* 0 */
   2,  /* 1 */
   3,  /* 2 */
   4,  /* 3 */
   3,  /* 4 */
   4,  /* 5 */
   4,  /* 6 */
   4,  /* 7 */
   4,  /* 8 */
   5,  /* 9 */
   5,  /* 10 */
   5,  /* 11 */
   6,  /* 12 */
   6,  /* 13 */
   6,  /* 14 */
   6,  /* 15 */
   6,  /* 16 */
   6,  /* 17 */
   7,  /* 18 */
   7,  /* 19 */
   7,  /* 20 */
   7,  /* 21 */
   8,  /* 22 */
   8,  /* 23 */
   8,  /* 24 */
   8,  /* 25 */
   8,  /* 26 */
   8,  /* 27 */
   8,  /* 28 */
   8,  /* 29 */
   9,  /* 30 */
   9,  /* 31 */
   9,  /* 32 */
   9,  /* 33 */
   9,  /* 34 */
   10}; /* 35 */

SquareDom::SquareDom() {
  m_bEnabled = true;
}

SquareDom::SquareDom(const SquareDom& src) :
  Domination(src) {
}

SquareDom::~SquareDom() {
}

bool SquareDom::_dominated(const Rectangle* r, UInt nGap) const {
  if(nGap < MAXGAP && r->m_nWidth < MAXSIZE)
    return(m_pSquare[r->m_nWidth][nGap] == 1);
  return(false);
}

UInt SquareDom::_entries(const Rectangle* r) const {
  if(r->m_nWidth < MAXSIZE)
    return(m_pSquareSize[r->m_nWidth]);
  return(0);
}

UInt SquareDom::_gap(const Rectangle* r) const {
  if(r->m_nWidth < MAXSIZE)
    return(m_pSquareSkip[r->m_nWidth]);
  return(0);
}

Domination* SquareDom::clone() const {
  return(new SquareDom(*this));
}
