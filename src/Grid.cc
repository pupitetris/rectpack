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

#include "Grid.h"
#include "Parameters.h"
#include "BoxDimensions.h"
#include "ICoords.h"
#include "Packer.h"
#include <iomanip>
#include <iostream>

Grid::Grid(const Packer* pPacker) :
  m_pPacker(pPacker) {
}

Grid::Grid() {
}

Grid::~Grid() {
}

void Grid::initialize(const Parameters* pParams) {
}

void Grid::resize(Int n, const std::vector<UInt>& v) {
  std::vector<std::vector<UInt> >::resize(n, v);
}

void Grid::initialize(const BoxDimensions* pBox) {
  m_Box = *pBox;
  std::vector<std::vector<UInt> >::clear();
  std::vector<std::vector<UInt> >::
    resize(pBox->m_nWidth, std::vector<UInt>(pBox->m_nHeight, GRIDEMPTY));
  
  m_vHor.clear();
  m_vHor.resize(pBox->m_nWidth + 1,
		std::vector<UInt>(pBox->m_nHeight, pBox->m_nWidth));
  m_vVer.clear();
  m_vVer.resize(pBox->m_nWidth + 1,
		std::vector<UInt>(pBox->m_nHeight, pBox->m_nHeight));
}

void Grid::resize(Int nDim) {
  std::vector<std::vector<UInt> >::clear();
  resize(nDim + 1, std::vector<UInt>(nDim, GRIDEMPTY));
  m_vHor.clear();
  m_vHor.resize(nDim + 1, std::vector<UInt>(nDim, 0));
  m_vVer.clear();
  m_vVer.resize(nDim + 1, std::vector<UInt>(nDim, 0));
}

void Grid::del(const Rectangle* s) {
  Int x, y;                                 /* coordinates of the grid to test */
  Int horz, vert;                  /* old horizontal width and vertical height */
  Int top;                                /* y index of top of old empty strip */
  Int left;                    /* x index of left-most cell of old empty strip */

  for (x = s->x; x < (Int) (s->x + s->m_nWidth); x++)    /* for each column occupied by square */
    std::fill(operator[](x).begin() + s->y, operator[](x).begin() + s->y + s->m_nHeight, GRIDEMPTY);

  for (x = s->x; x < (Int) (s->x + s->m_nWidth); x++)     /* each column occupied by placement */
    {vert = m_vVer[x][s->y];                    /* old height of vertical strip */
      if (s->y >= 1 && empty(x, (Int) s->y - 1)) /* column has at least one empty position above*/
	top = s->y - m_vVer[x][s->y - 1];   /* index of top of old vertical strip */
      else top = s->y;
      for (y = top; y < (Int) s->y; y++)                  /* for each cell of column */
	m_vVer[x][y] = vert;                       /* new vertical height of column */
      for (y = s->y + s->m_nHeight; y <= top + vert - 1; y++)      /* each cell of column below */
	m_vVer[x][y] = vert;}                      /* old vertical height of column */

  for(y = s->y; y < (Int) (s->y + s->m_nHeight); y++)    /* for each row occupied by placement */
    {horz = m_vHor[s->x][y];                   /* old width of horizontal strip */
      if(s->x >= 1 && empty((Int) (s->x - 1), y)) /* row has at least one empty position to left*/
	left = s->x - m_vHor[s->x - 1][y]; /* index of left edge of old empty row */
      else left = s->x;
      for (x = left; x < (Int) s->x; x++)            /* for each cell of row to left */
	m_vHor[x][y] = horz;                         /* new horizontal width of row */
      for (x = s->x + s->m_nWidth; x <= left + horz - 1; x++)           /* each cell of column */
	m_vHor[x][y] = horz;
    }
}

void Grid::add(const Rectangle* s, std::vector<Int>& v) {
  Int x, y;
  Int run;              /* length of horizontal or vertical run of empty space */

  for (x = (Int) s->x; x < (Int) (s->x + s->m_nWidth); x++)    /* for each column occupied by square */
    for (y = (Int) s->y; y < (Int) (s->y + s->m_nHeight); y++) {    /* for each row occupied by square */
      operator[](x)[y] = s->m_nID;                                       /* fill in square */
      if (m_vVer[x][y] <= m_vHor[x][y])
	v[m_vVer[x][y]]--;     /* remove old value */
      else
	v[m_vHor[x][y]]--;                         /* remove old value */
    }
  /* update vertical grid above placement */
  for (x = (Int) s->x; x < (Int) (s->x + s->m_nWidth); x++) {/* for each column occupied by placement */
    run = 0;                                                    /* no run yet */
    for(y = s->y - 1; y >= 0; y--)            /* go up column above placement */
      if(empty(x, y)) run++;                   /* cell is empty, continue run */
      else break;                    /* cell is occupied, vertical run is over */
    for (y = y + 1; y < (Int) s->y; y++) {            /* for each cell of empty run */
      if (run < (Int) m_vHor[x][y]) {          /* new vertical strip < horizontal strip */
	if (m_vVer[x][y] <= m_vHor[x][y]) v[m_vVer[x][y]]--;  /* remove old value */
	else v[m_vHor[x][y]]--;                          /* remove old value */
	v[run]++;}                                    /* insert new value */
      m_vVer[x][y] = run;                        /* new height of vertical strip */
    }
    /* update vertical grid below placement */
    run = m_vVer[x][s->y] - run - s->m_nHeight; /*run below is previous - above - rectangle*/
    for (y = (Int) (s->y + s->m_nHeight + run - 1);
	 y >= (Int) (s->y + s->m_nHeight); y--) {/* for each cell of empty run */
      if (run < (Int) m_vHor[x][y]) {          /* new vertical strip < horizontal strip */
	if (m_vVer[x][y] <= m_vHor[x][y]) v[m_vVer[x][y]]--;  /* remove old value */
	else v[m_vHor[x][y]]--;                          /* remove old value */
	v[run]++;                                    /* insert new value */
      }
      m_vVer[x][y] = run;                       /* new height of vertical strip */
    }
  }
  /* update horizontal grid left of placement */
  for(y = (Int) s->y; y < (Int) (s->y + s->m_nHeight); y++) {   /* for each row occupied by placement */
    run = 0;                                                    /* no run yet */
    for(x = s->x - 1; x >= 0; x--)                    /* go left of placement */
      if(empty(x, y)) run++;                   /* cell is empty, continue run */
      else break;                    /* cell is occupied, vertical run is over */
    for(x = x + 1; x < (Int) s->x; x++) {            /* for each cell of empty run */
      if(run < (Int) m_vVer[x][y]) {          /* new horizontal strip < vertical strip */
	if (m_vVer[x][y] <= m_vHor[x][y]) v[m_vVer[x][y]]--;  /* remove old value */
	else v[m_vHor[x][y]]--;                          /* remove old value */
	v[run]++;                                    /* insert new value */
      }
      m_vHor[x][y] = run;                        /* new height of vertical strip */
    }
    /* update horizontal grid right of placement */
    run = (Int) (m_vHor[s->x][y] - run - s->m_nWidth); /* run right is previous - left - rectangle*/
    for(x = (Int) (s->x + s->m_nWidth + run - 1);
	x >= (Int) (s->x + s->m_nWidth); x--) { /* for each cell of empty run */
      if(run < (Int) m_vVer[x][y]) {          /* new horizontal strip < vertical strip */
	if (m_vVer[x][y] <= m_vHor[x][y]) v[m_vVer[x][y]]--;  /* remove old value */
	else v[m_vHor[x][y]]--;                          /* remove old value */
	v[run]++;}                                    /* insert new value */
      m_vHor[x][y] = run;                      /* new height of vertical strip */
    }
  }
}

void Grid::add(const Rectangle* s) {
  Int x, y;
  Int run;              /* length of horizontal or vertical run of empty space */

  for (x = (Int) s->x; x < (Int) (s->x + s->m_nWidth); x++)    /* for each column occupied by square */
    for (y = (Int) s->y; y < (Int) (s->y + s->m_nHeight); y++)
      operator[](x)[y] = s->m_nID;
  /* update vertical grid above placement */
  for (x = (Int) s->x; x < (Int) (s->x + s->m_nWidth); x++) {/* for each column occupied by placement */
    run = 0;                                                    /* no run yet */
    for (y = s->y-1; y >= 0; y--)            /* go up column above placement */
      if(empty(x, y)) run++;                   /* cell is empty, continue run */
      else break;                    /* cell is occupied, vertical run is over */
    for (y = y + 1; y < (Int) s->y; y++)
      m_vVer[x][y] = run;                        /* new height of vertical strip */
    /* update vertical grid below placement */
    run = (Int) (m_vVer[x][s->y] - run - s->m_nHeight); /*run below is previous - above - rectangle*/
    for(y = (Int) (s->y + s->m_nHeight + run - 1);
	y >= (Int) (s->y + s->m_nHeight); y--)
      m_vVer[x][y] = run;                       /* new height of vertical strip */
  }
  /* update horizontal grid left of placement */
  for(y = (Int) s->y; y < (Int) (s->y + s->m_nHeight); y++) {   /* for each row occupied by placement */
    run = 0;                                                    /* no run yet */
    for(x = s->x - 1; x >= 0; x--)                    /* go left of placement */
      if(empty(x, y)) run++;                   /* cell is empty, continue run */
      else break;                    /* cell is occupied, vertical run is over */
    for(x = x + 1; x < (Int) s->x; x++)
      m_vHor[x][y] = run;                        /* new height of vertical strip */
    /* update horizontal grid right of placement */
    run = m_vHor[s->x][y] - run - s->m_nWidth; /* run right is previous - left - rectangle*/
    for(x = (Int) (s->x + s->m_nWidth + run - 1);
	x >= (Int) (s->x + s->m_nWidth); x--)
      m_vHor[x][y] = run;                      /* new height of vertical strip */
  }
}

void Grid::add(Component* c) {

  /**
   * Draw the outline of the component.
   */

  Int x, y;
  Int run;              /* length of horizontal or vertical run of empty space */
  for(x = c->m_nX; x < (Int) (c->m_nX + c->m_Dims.m_nWidth); ++x)
    std::fill(operator[](x).begin() + c->m_nY,
	      operator[](x).begin() + c->m_nY + c->m_Dims.m_nHeight,
	      c->m_nID);

  /* update vertical grid above placement */
  for(x = (Int) c->m_nX; x < (Int) (c->m_nX + c->m_Dims.m_nWidth); ++x) {/* for each column occupied by placement */
    run = 0;                                                    /* no run yet */
    for (y = c->m_nY - 1; y >= 0; --y)            /* go up column above placement */
      if(empty(x, y)) ++run;                   /* cell is empty, continue run */
      else break;                    /* cell is occupied, vertical run is over */
    std::fill(m_vVer[x].begin() + y + 1, m_vVer[x].begin() + c->m_nY, run); // new height of vertical strip.
    /* update vertical grid below placement */
    run = m_vVer[x][c->m_nY] - run - c->m_Dims.m_nHeight; /*run below is previous - above - rectangle*/
    std::fill(m_vVer[x].begin() + c->m_nY + c->m_Dims.m_nHeight,
	      m_vVer[x].begin() + c->m_nY + c->m_Dims.m_nHeight + run, run); // new height of vertical strip.
  }
  /* update horizontal grid left of placement */
  for(y = (Int) c->m_nY; y < (Int) (c->m_nY + c->m_Dims.m_nHeight); ++y) {   /* for each row occupied by placement */
    run = 0;                                                    /* no run yet */
    for(x = c->m_nX - 1; x >= 0; --x)                    /* go left of placement */
      if(empty(x, y)) ++run;                   /* cell is empty, continue run */
      else break;                    /* cell is occupied, vertical run is over */
    for(x = x + 1; x < (Int) c->m_nX; ++x)
      m_vHor[x][y] = run;                        /* new height of vertical strip */
    /* update horizontal grid right of placement */
    run = m_vHor[c->m_nX][y] - run - c->m_Dims.m_nWidth; /* run right is previous - left - rectangle*/
    for(x = (Int) c->m_nX + c->m_Dims.m_nWidth + run - 1;
	x >= (Int) (c->m_nX + c->m_Dims.m_nWidth); --x)
      m_vHor[x][y] = run;                      /* new height of vertical strip */
  }
}

void Grid::add(Component* c, std::vector<Int>& v) {
  Int x, y;
  Int run;              /* length of horizontal or vertical run of empty space */

  for(x = c->m_nX; x < (Int) (c->m_nX + c->m_Dims.m_nWidth); ++x)
    for (y = c->m_nY; y < (Int) (c->m_nY + c->m_Dims.m_nHeight); ++y) {
      operator[](x)[y] = c->m_nID;
      if(m_vVer[x][y] <= m_vHor[x][y])
	--v[m_vVer[x][y]];
      else
	--v[m_vHor[x][y]];
    }
  
  for(x = (Int) c->m_nX; x < (Int) (c->m_nX + c->m_Dims.m_nWidth); ++x) {/* for each column occupied by placement */
    run = 0;                                                    /* no run yet */
    for(y = c->m_nY - 1; y >= 0; y--)            /* go up column above placement */
      if(empty(x, y)) run++;                   /* cell is empty, continue run */
      else break;                    /* cell is occupied, vertical run is over */
    for (y = y + 1; y < (Int) c->m_nY; ++y) {            /* for each cell of empty run */
      if(run < (Int) m_vHor[x][y]) {          /* new vertical strip < horizontal strip */
	if (m_vVer[x][y] <= m_vHor[x][y]) --v[m_vVer[x][y]];  /* remove old value */
	else --v[m_vHor[x][y]];                          /* remove old value */
	++v[run];}                                    /* insert new value */
      m_vVer[x][y] = run;                        /* new height of vertical strip */
    }
    /* update vertical grid below placement */
    run = m_vVer[x][c->m_nY] - run - c->m_Dims.m_nHeight; /*run below is previous - above - rectangle*/
    for(y = (Int) c->m_nY + c->m_Dims.m_nHeight + run - 1;
	y >= (Int) (c->m_nY + c->m_Dims.m_nHeight); --y) {/* for each cell of empty run */
      if(run < (Int) m_vHor[x][y]) {          /* new vertical strip < horizontal strip */
	if(m_vVer[x][y] <= m_vHor[x][y]) --v[m_vVer[x][y]];  /* remove old value */
	else --v[m_vHor[x][y]];                          /* remove old value */
	++v[run];                                    /* insert new value */
      }
      m_vVer[x][y] = run;                       /* new height of vertical strip */
    }
  }
  /* update horizontal grid left of placement */
  for(y = (Int) c->m_nY; y < (Int) (c->m_nY + c->m_Dims.m_nHeight); ++y) {   /* for each row occupied by placement */
    run = 0;                                                    /* no run yet */
    for(x = c->m_nX - 1; x >= 0; --x)                    /* go left of placement */
      if(empty(x, y)) ++run;                   /* cell is empty, continue run */
      else break;                    /* cell is occupied, vertical run is over */
    for(x = x + 1; x < (Int) c->m_nX; ++x) {            /* for each cell of empty run */
      if(run < (Int) m_vVer[x][y]) {          /* new horizontal strip < vertical strip */
	if(m_vVer[x][y] <= m_vHor[x][y]) --v[m_vVer[x][y]];  /* remove old value */
	else --v[m_vHor[x][y]];                          /* remove old value */
	++v[run];                                    /* insert new value */
      }
      m_vHor[x][y] = run;                        /* new height of vertical strip */
    }
    /* update horizontal grid right of placement */
    run = m_vHor[c->m_nX][y] - run - c->m_Dims.m_nWidth; /* run right is previous - left - rectangle*/
    for(x = (Int) c->m_nX + c->m_Dims.m_nWidth + run - 1;
	x >= (Int) (c->m_nX + c->m_Dims.m_nWidth); --x) { /* for each cell of empty run */
      if(run < (Int) m_vVer[x][y]) {          /* new horizontal strip < vertical strip */
	if(m_vVer[x][y] <= m_vHor[x][y]) --v[m_vVer[x][y]];  /* remove old value */
	else --v[m_vHor[x][y]];                          /* remove old value */
	++v[run];}                                    /* insert new value */
      m_vHor[x][y] = run;                      /* new height of vertical strip */
    }
  }
}

void Grid::del(Component* c) {

  Int x, y;                                 /* coordinates of the grid to test */
  Int horz, vert;                  /* old horizontal width and vertical height */
  Int top;                                /* y index of top of old empty strip */
  Int left;                    /* x index of left-most cell of old empty strip */

  /**
   * Undraw the component's outline.
   */

  for(x = c->m_nX; x < (Int) (c->m_nX + c->m_Dims.m_nWidth); ++x)
    std::fill(operator[](x).begin() + c->m_nY,
	      operator[](x).begin() + c->m_nY + c->m_Dims.m_nHeight,
	      GRIDEMPTY);

  for(x = c->m_nX; x < (Int) (c->m_nX + c->m_Dims.m_nWidth); ++x) {   /* each column occupied by placement */
    vert = m_vVer[x][c->m_nY];                /* old height of vertical strip */
    if(c->m_nY >= 1 && empty(x, (Int) (c->m_nY - 1))) /* column has at least one empty position above*/
      top = (Int) c->m_nY - m_vVer[x][c->m_nY - 1]; /* index of top of old vertical strip */
    else top = c->m_nY;

    /**
     * Rewrite the vertical cells above the component and below.
     */

    std::fill(m_vVer[x].begin() + top, m_vVer[x].begin() + c->m_nY, vert);
    std::fill(m_vVer[x].begin() + c->m_nY + c->m_Dims.m_nHeight,
	      m_vVer[x].begin() + top + vert, vert);
  }

  /**
   * Rewrite the horizontal cells to the left and right of the
   * component.
   */

  for (y = c->m_nY; y < (Int) (c->m_nY + c->m_Dims.m_nHeight); ++y) {   /* for each row occupied by placement */
    horz = m_vHor[c->m_nX][y];                 /* old width of horizontal strip */
    if(c->m_nX >= 1 && empty((Int) (c->m_nX - 1), y))  /* row has at least one empty position to left*/
      left = c->m_nX - m_vHor[c->m_nX - 1][y]; /* index of left edge of old empty row */
    else left = c->m_nX;
    for(x = left; x < (Int) c->m_nX; ++x)      /* for each cell of row to left */
      m_vHor[x][y] = horz;                     /* new horizontal width of row */
    for(x = c->m_nX + c->m_Dims.m_nWidth; x <= left + horz - 1; ++x) /* each cell of column */
      m_vHor[x][y] = horz;
  }
}

void Grid::print() const {
  printAux(*this);
}

void Grid::printid() const {
  printX();
  std::cout << std::endl;
  for(UInt y = 0; y < m_Box.m_nHeight; ++y) {
    std::cout << std::setw(2) << y << " ";
    for(UInt x = 0; x < m_Box.m_nWidth; ++x)
      if(empty(x, y))
	std::cout << " .";
      else 
	std::cout << std::setw(2) << (Int) get(x, y);
    std::cout << std::endl;
  }
}

void Grid::printX() const {
  std::cout << "   ";
  for(UInt x = 0; x < m_Box.m_nWidth; ++x)
    if(x % 5 > 0) std::cout << "  ";
    else std::cout << std::setw(2) << x;
}

void Grid::printh() const {
  printRaw(m_vHor);
}

void Grid::printv() const {
  printRaw(m_vVer);
}

void Grid::printAux(const std::vector<std::vector<UInt> >& v) const {
  for(Int y = (Int) (m_Box.m_nHeight - 1); y >= 0; --y) {
    for(UInt x = 0; x < m_Box.m_nWidth; x++)
      if(v[x][y] == GRIDEMPTY)
	std::cout << " .";
      else 
	std::cout << std::setw(2) << std::setfill(' ') << v[x][y];
    std::cout << std::endl;
  }
}

void Grid::printRaw(const std::vector<std::vector<UInt> >& v) const {
  for(UInt y = 0; y < m_Box.m_nHeight; ++y) {
    for(UInt x = 0; x < m_Box.m_nWidth; x++)
      if(v[x][y] == 0)
	std::cout << " .";
      else 
	std::cout << std::setw(2) << (Int) v[x][y];
    std::cout << std::endl;
  }
}

void Grid::set(const BoxDimensions& b) {
  m_Box = b;
}
