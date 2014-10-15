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

#include "URectDom.h"
#include "Rectangle.h"

#define MAXSIZE 28
#define MAXGAP 8

UInt URectDom::m_pRectShort[MAXSIZE][MAXGAP] = /* assumes short side is parallel to edge */
  {{0, 0, 0, 0, 0, 0, 0, 0},  /* 0 */
   {0, 1, 0, 0, 0, 0, 0, 0},  /* 1x2 */
   {0, 1, 1, 0, 0, 0, 0, 0},  /* 2x3 */
   {0, 1, 0, 1, 0, 0, 0, 0},  /* 3x4 */
   {0, 1, 1, 0, 0, 0, 0, 0},  /* 4x5 */
   {0, 1, 1, 0, 0, 0, 0, 0},  /* 5x6 */
   {0, 1, 1, 0, 0, 0, 0, 0},  /* 6x7 */
   {0, 1, 1, 1, 0, 0, 0, 0},  /* 7x8 */
   {0, 1, 1, 1, 0, 0, 0, 0},  /* 8x9 */
   {0, 1, 1, 1, 0, 0, 0, 0},  /* 9x10 */
   {0, 1, 1, 1, 1, 0, 0, 0},  /* 10x11 */
   {0, 1, 1, 1, 1, 0, 0, 0},  /* 11x12 */
   {0, 1, 1, 1, 1, 0, 0, 0},  /* 12x13 */
   {0, 1, 1, 1, 1, 0, 0, 0},  /* 13x14 */
   {0, 1, 1, 1, 1, 1, 0, 0},  /* 14x15 */
   {0, 1, 1, 1, 1, 1, 0, 0},  /* 15x16 */
   {0, 1, 1, 1, 1, 1, 0, 0},  /* 16x17 */
   {0, 1, 1, 1, 1, 1, 0, 0},  /* 17x18 */
   {0, 1, 1, 1, 1, 1, 0, 0},  /* 18x19 */
   {0, 1, 1, 1, 1, 1, 1, 0},  /* 19x20 */
   {0, 1, 1, 1, 1, 1, 1, 0},  /* 20x21 */
   {0, 1, 1, 1, 1, 1, 1, 0},  /* 21x22 */
   {0, 1, 1, 1, 1, 1, 1, 0},  /* 22x23 */
   {0, 1, 1, 1, 1, 1, 1, 0},  /* 23x24 */
   {0, 1, 1, 1, 1, 1, 1, 0},  /* 24x25 */
   {0, 1, 1, 1, 1, 1, 1, 1},  /* 25x26 */
   {0, 1, 1, 1, 1, 1, 1, 1},  /* 26x27 */
   {0, 1, 1, 1, 1, 1, 1, 1}}; /* 27x28 */

UInt URectDom::m_pRectLong[MAXSIZE][MAXGAP] = /* assumes long side is parallel to edge */
  {{0, 0, 0, 0, 0, 0, 0, 0},  /* 0 */
   {0, 1, 0, 0, 0, 0, 0, 0},  /* 1x2 */
   {0, 1, 1, 0, 0, 0, 0, 0},  /* 2x3 */
   {0, 1, 1, 1, 0, 0, 0, 0},  /* 3x4 */
   {0, 1, 1, 0, 1, 0, 0, 0},  /* 4x5 */
   {0, 1, 1, 0, 0, 0, 0, 0},  /* 5x6 */
   {0, 1, 1, 1, 0, 0, 0, 0},  /* 6x7 */
   {0, 1, 1, 1, 0, 0, 0, 0},  /* 7x8 */
   {0, 1, 1, 1, 0, 0, 0, 0},  /* 8x9 */
   {0, 1, 1, 1, 1, 0, 0, 0},  /* 9x10 */
   {0, 1, 1, 1, 1, 0, 0, 0},  /* 10x11 */
   {0, 1, 1, 1, 1, 0, 0, 0},  /* 11x12 */
   {0, 1, 1, 1, 1, 0, 0, 0},  /* 12x13 */
   {0, 1, 1, 1, 1, 1, 0, 0},  /* 13x14 */
   {0, 1, 1, 1, 1, 1, 0, 0},  /* 14x15 */
   {0, 1, 1, 1, 1, 1, 0, 0},  /* 15x16 */
   {0, 1, 1, 1, 1, 1, 0, 0},  /* 16x17 */
   {0, 1, 1, 1, 1, 1, 0, 0},  /* 17x18 */
   {0, 1, 1, 1, 1, 1, 1, 0},  /* 18x19 */
   {0, 1, 1, 1, 1, 1, 1, 0},  /* 19x20 */
   {0, 1, 1, 1, 1, 1, 1, 0},  /* 20x21 */
   {0, 1, 1, 1, 1, 1, 1, 0},  /* 21x22 */
   {0, 1, 1, 1, 1, 1, 1, 0},  /* 22x23 */
   {0, 1, 1, 1, 1, 1, 1, 0},  /* 23x24 */
   {0, 1, 1, 1, 1, 1, 1, 1},  /* 24x25 */
   {0, 1, 1, 1, 1, 1, 1, 1},  /* 25x26 */
   {0, 1, 1, 1, 1, 1, 1, 1},  /* 26x27 */
   {0, 1, 1, 1, 1, 1, 1, 1}}; /* 27x28 */

UInt URectDom::m_pRectSizeShort[MAXSIZE] =
  {0,  /* 0 */
   2,  /* 1 */
   3,  /* 2 */
   4,  /* 3 */
   3,  /* 4 */
   3,  /* 5 */
   3,  /* 6 */
   4,  /* 7 */
   4,  /* 8 */
   4,  /* 9 */
   5,  /* 10 */
   5,  /* 11 */
   5,  /* 12 */
   5,  /* 13 */
   6,  /* 14 */
   6,  /* 15 */
   6,  /* 16 */
   6,  /* 17 */
   6,  /* 18 */
   7,  /* 19 */
   7,  /* 20 */
   7,  /* 21 */
   7,  /* 22 */
   7,  /* 23 */
   7,  /* 24 */
   8,  /* 25 */
   8,  /* 26 */
   8}; /* 27 */

UInt URectDom::m_pRectSizeLong[MAXSIZE] =
  {0,  /* 0 */
   2,  /* 1 */
   3,  /* 2 */
   4,  /* 3 */
   5,  /* 4 */
   3,  /* 5 */
   4,  /* 6 */
   4,  /* 7 */
   4,  /* 8 */
   5,  /* 9 */
   5,  /* 10 */
   5,  /* 11 */
   5,  /* 12 */
   6,  /* 13 */
   6,  /* 14 */
   6,  /* 15 */
   6,  /* 16 */
   6,  /* 17 */
   7,  /* 18 */
   7,  /* 19 */
   7,  /* 20 */
   7,  /* 21 */
   7,  /* 22 */
   7,  /* 23 */
   8,  /* 24 */
   8,  /* 25 */
   8,  /* 26 */
   8}; /* 27 */

UInt URectDom::m_pRectSkipShort[MAXSIZE] =
  {0,  /* 0 */
   2,  /* 1 */
   3,  /* 2 */
   2,  /* 3 */
   3,  /* 4 */
   3,  /* 5 */
   3,  /* 6 */
   4,  /* 7 */
   4,  /* 8 */
   4,  /* 9 */
   5,  /* 10 */
   5,  /* 11 */
   5,  /* 12 */
   5,  /* 13 */
   6,  /* 14 */
   6,  /* 15 */
   6,  /* 16 */
   6,  /* 17 */
   6,  /* 18 */
   7,  /* 19 */
   7,  /* 20 */
   7,  /* 21 */
   7,  /* 22 */
   7,  /* 23 */
   7,  /* 24 */
   8,  /* 25 */
   8,  /* 26 */
   8}; /* 27 */

UInt URectDom::m_pRectSkipLong[MAXSIZE] =
  {0,  /* 0 */
   2,  /* 1 */
   3,  /* 2 */
   4,  /* 3 */
   3,  /* 4 */
   3,  /* 5 */
   4,  /* 6 */
   4,  /* 7 */
   4,  /* 8 */
   5,  /* 9 */
   5,  /* 10 */
   5,  /* 11 */
   5,  /* 12 */
   6,  /* 13 */
   6,  /* 14 */
   6,  /* 15 */
   6,  /* 16 */
   6,  /* 17 */
   7,  /* 18 */
   7,  /* 19 */
   7,  /* 20 */
   7,  /* 21 */
   7,  /* 22 */
   7,  /* 23 */
   8,  /* 24 */
   8,  /* 25 */
   8,  /* 26 */
   8}; /* 27 */

URectDom::URectDom() {
  m_bEnabled = true;
}

URectDom::URectDom(const URectDom& src) :
  Domination(src) {
}

URectDom::~URectDom() {
}

bool URectDom::dominatedw(const Rectangle* r, UInt nGap) const {
  if(nGap < MAXGAP) {
    if(r->m_nWidth < r->m_nHeight)
      return(m_pRectShort[r->m_nWidth][nGap] == 1);
    else
      return(m_pRectLong[r->m_nHeight][nGap] == 1);
  }
  return(false);
}

bool URectDom::dominatedh(const Rectangle* r, UInt nGap) const {
  if(nGap < MAXGAP) {
    if(r->m_nHeight < r->m_nWidth)
      return(m_pRectShort[r->m_nHeight][nGap] == 1);
    else
      return(m_pRectLong[r->m_nWidth][nGap] == 1);
  }
  return(false);
}

UInt URectDom::entriesw(const Rectangle* r) const {
  if(r->m_nWidth < r->m_nHeight)
    return(m_pRectSizeShort[r->m_nWidth]);
  else
    return(m_pRectSizeLong[r->m_nHeight]);
}

UInt URectDom::entriesh(const Rectangle* r) const {
  if(r->m_nHeight < r->m_nWidth)
    return(m_pRectSizeShort[r->m_nHeight]);
  else
    return(m_pRectSizeLong[r->m_nWidth]);
}

UInt URectDom::gapw(const Rectangle* r) const {
  if(r->m_nWidth < r->m_nHeight)
    return(m_pRectSkipShort[r->m_nWidth]);
  else
    return(m_pRectSkipLong[r->m_nHeight]);
}

UInt URectDom::gaph(const Rectangle* r) const {
  if(r->m_nHeight < r->m_nWidth)
    return(m_pRectSkipShort[r->m_nHeight]);
  else
    return(m_pRectSkipLong[r->m_nWidth]);
}

Domination* URectDom::clone() const {
  return(new URectDom(*this));
}
