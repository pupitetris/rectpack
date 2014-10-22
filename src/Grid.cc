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

void Grid::resize(Int n, const std::vector<std::vector<UInt> >& v) {
  std::vector<std::vector<std::vector<UInt> > >::resize(n, v);
}

void Grid::initialize(const BoxDimensions* pBox) {
  m_Box = *pBox;
  std::vector<std::vector<std::vector<UInt> > >::clear();
  std::vector<std::vector<std::vector<UInt> > >::
    resize(pBox->m_nWidth, std::vector<std::vector<UInt> >(pBox->m_nHeight, std::vector<UInt>(pBox->m_nLength, GRIDEMPTY)));

  m_vHor.clear();
  m_vHor.resize(pBox->m_nWidth + 1,
		std::vector<std::vector<UInt> >(pBox->m_nHeight, std::vector<UInt>(pBox->m_nLength, pBox->m_nWidth)));
  m_vVer.clear();
  m_vVer.resize(pBox->m_nWidth + 1,
		std::vector<std::vector<UInt> >(pBox->m_nHeight, std::vector<UInt>(pBox->m_nLength, pBox->m_nHeight)));
  m_vLon.clear();
  m_vLon.resize(pBox->m_nWidth + 1,
		std::vector<std::vector<UInt> >(pBox->m_nHeight, std::vector<UInt>(pBox->m_nLength, pBox->m_nLength)));
}

void Grid::resize(Int nDim) {
  std::vector<std::vector<std::vector<UInt> > >::clear();
  resize(nDim + 1, std::vector<std::vector<UInt> >(nDim, std::vector<UInt>(nDim, GRIDEMPTY)));
  m_vHor.clear();
  m_vHor.resize(nDim + 1, std::vector<std::vector<UInt> >(nDim, std::vector<UInt>(nDim, 0)));
  m_vVer.clear();
  m_vVer.resize(nDim + 1, std::vector<std::vector<UInt> >(nDim, std::vector<UInt>(nDim, 0)));
  m_vLon.clear();
  m_vLon.resize(nDim + 1, std::vector<std::vector<UInt> >(nDim, std::vector<UInt>(nDim, 0)));
}

void Grid::del(const Rectangle* s) {
  Int x, y, z;		// coordinates of the grid to test
  Int horz, vert, lon;	// old horizontal width and vertical height and longitudinal length
  Int left, top, far;   // index of top of old empty strip

  for(x = s->x; x < (Int) (s->x + s->m_nWidth); x++)
    for(y = s->y; y < (Int) (s->y + s->m_nHeight); y++)         // for each column occupied by square
      std::fill(operator[](x)[y].begin() + s->z,
		operator[](x)[y].begin() + s->z + s->m_nLength,
		GRIDEMPTY);

  // Horizontal
  for(z = s->z; z < (Int) (s->z + s->m_nLength); z++) {         // each line occupied by placement
    for(y = s->y; y < (Int) (s->y + s->m_nHeight); y++) {       // for each row occupied by placement
      horz = m_vHor[s->x][y][z];                                // old width of horizontal strip

      if(s->x >= 1 && empty((Int) (s->x - 1), y, z))            // row has at least one empty position to left
	left = s->x - m_vHor[s->x - 1][y][z];                   // index of left edge of old empty row
      else
	left = s->x;

      for(x = left; x < (Int) s->x; x++)                        // for each cell of row to left
	m_vHor[x][y][z] = horz;                                 // new horizontal width of row
      for(x = s->x + s->m_nWidth; x <= left + horz - 1; x++)    // each cell of column
	m_vHor[x][y][z] = horz;
    }
  }

  // Vertical
  for(z = s->z; z < (Int) (s->z + s->m_nLength); z++) {		// each line occupied by placement
    for(x = s->x; x < (Int) (s->x + s->m_nWidth); x++) {	// each column occupied by placement
      vert = m_vVer[x][s->y][z];                                // old height of vertical strip

      if (s->y >= 1 && empty(x, (Int) s->y - 1, z))             // column has at least one empty position above
	top = s->y - m_vVer[x][s->y - 1][z];                    // index of top of old vertical strip
      else
	top = s->y;

      for(y = top; y < (Int) s->y; y++)				// for each cell of column
	m_vVer[x][y][z] = vert;                                 // new vertical height of column
      for(y = s->y + s->m_nHeight; y <= top + vert - 1; y++)	// each cell of column below
	m_vVer[x][y][z] = vert;                                 // old vertical height of column
    }
  }

  // Longitudinal
  for(x = s->x; x < (Int) (s->x + s->m_nWidth); x++) {		// each column occupied by placement
    for(y = s->y; y < (Int) (s->y + s->m_nHeight); y++) {       // for each row occupied by placement
      lon = m_vLon[x][y][s->z];                                 // old length of longitudinal strip

      if(s->z >= 1 && empty(x, y, (Int) (s->z - 1)))            // line has at least one empty position to back
	far = s->z - m_vLon[s->z - 1][y][z];                    // index of far edge of old empty line
      else
	far = s->z;

      for(z = far; z < (Int) s->z; z++)				// for each cell of line to back
	m_vLon[x][y][z] = lon;                                  // new longitudinal length of line
      for(z = s->z + s->m_nLength; z <= far + lon - 1; z++)	// each cell of column
	m_vLon[x][y][z] = lon;
    }
  }
}

#define RUN_START(dim,init)							\
  run = 0;				/* no run yet */			\
  for(dim = init; dim >= 0; dim--)	/* go up */				\
    if (empty(x,y,z))								\
      run++;				/* cell is empty, continue run */	\
    else									\
      break				/* cell is occupied, run is over */


void Grid::removeOldValue(std::vector<Int>& v,Int x, Int y, Int z) {
  if (m_vHor[x][y][z] <= m_vVer[x][y][z] &&
      m_vHor[x][y][z] <= m_vLon[x][y][z])
    v[m_vHor[x][y][z]]--;						// remove old value
  else if (m_vVer[x][y][z] <= m_vHor[x][y][z] &&
	   m_vVer[x][y][z] <= m_vLon[x][y][z])
    v[m_vVer[x][y][z]]--;						// remove old value
  else
    v[m_vLon[x][y][z]]--;						// remove old value
}

void Grid::add(const Rectangle* s, std::vector<Int>& v) {
  Int x, y, z;
  Int run;								// length of horizontal or vertical run of empty space

  for (x = (Int) s->x; x < (Int) (s->x + s->m_nWidth); x++)		// for each column occupied by square
    for (y = (Int) s->y; y < (Int) (s->y + s->m_nHeight); y++)		// for each row occupied by square
      for (z = (Int) s->z; z < (Int) (s->z + s->m_nLength); z++) {	// for each line occupied by square
	operator[](x)[y][z] = s->m_nID;					// fill in square
	removeOldValue(v,x,y,z);
      }

  // Horizontal
  for(y = (Int) s->y; y < (Int) (s->y + s->m_nHeight); y++)	// for each row occupied by placement
    for(z = (Int) s->z; z < (Int) (s->z + s->m_nLength); z++) {	// for each line occupied by placement
      RUN_START(x,s->x - 1);

      // update horizontal grid above placement
      for (x = x + 1; x < (Int) s->x; x++) {			// for each cell of empty run
	if (run < (Int) m_vVer[x][y][z] ||			// pupitetris FIXME: this could be &&
	    run < (Int) m_vLon[x][y][z]) {			// new vertical strip < horizontal strip
	  removeOldValue(v,x,y,z);
	  v[run]++;                                             // insert new value
	}
	m_vHor[x][y][z] = run;					// new height of vertical strip
      }

      // update horizontal grid below placement
      run = m_vHor[s->x][y][z] - run - s->m_nWidth;			// run below is previous - above - rectangle
      for (x = (Int) (s->x + s->m_nWidth + run - 1);
	   x >= (Int) (s->x + s->m_nWidth); x--) {		// for each cell of empty run
	if (run < (Int) m_vHor[x][y][z] ||			// pupitetris FIXME: this could be &&
	    run < (Int) m_vLon[x][y][z]) {			// new vertical strip < horizontal strip
	  removeOldValue(v,x,y,z);
	  v[run]++;                                             // insert new value
	}
	m_vHor[x][y][z] = run;					// new height of vertical strip
      }
    }

  // Vertical
  for(x = (Int) s->x; x < (Int) (s->x + s->m_nWidth); x++)	// for each column occupied by placement
    for(z = (Int) s->z; z < (Int) (s->z + s->m_nLength); z++) {	// for each line occupied by placement
      RUN_START(y,s->y - 1);

      // update vertical grid above placement
      for (y = y + 1; y < (Int) s->y; y++) {			// for each cell of empty run
	if (run < (Int) m_vHor[x][y][z] ||			// pupitetris FIXME: this could be &&
	    run < (Int) m_vLon[x][y][z]) {			// new vertical strip < horizontal strip
	  removeOldValue(v,x,y,z);
	  v[run]++;                                             // insert new value
	}
	m_vVer[x][y][z] = run;					// new height of vertical strip
      }

      // update vertical grid below placement
      run = m_vVer[x][s->y][z] - run - s->m_nHeight;		// run below is previous - above - rectangle
      for (y = (Int) (s->y + s->m_nHeight + run - 1);
	   y >= (Int) (s->y + s->m_nHeight); y--) {		// for each cell of empty run
	if (run < (Int) m_vHor[x][y][z] ||			// pupitetris FIXME: this could be &&
	    run < (Int) m_vLon[x][y][z]) {			// new vertical strip < horizontal strip
	  removeOldValue(v,x,y,z);
	  v[run]++;                                             // insert new value
	}
	m_vVer[x][y][z] = run;					// new height of vertical strip
      }
    }

  // Longitudinal
  for(x = (Int) s->x; x < (Int) (s->x + s->m_nWidth); x++)      // for each column occupied by placement
    for(y = (Int) s->y; y < (Int) (s->y + s->m_nHeight); y++) { // for each row occupied by placement
      RUN_START(z,s->z - 1);

      // update longitudinal grid above placement
      for (z = z + 1; z < (Int) s->z; z++) {			// for each cell of empty run
	if (run < (Int) m_vHor[x][y][z] ||			// pupitetris FIXME: this could be &&
	    run < (Int) m_vVer[x][y][z]) {			// new longitudinal strip < horizontal strip
	  removeOldValue(v,x,y,z);
	  v[run]++;                                             // insert new value
	}
	m_vLon[x][y][z] = run;					// new height of longitudinal strip
      }

      // update longitudinal grid below placement
      run = m_vLon[x][y][s->z] - run - s->m_nLength;			// run below is previous - above - rectangle
      for (z = (Int) (s->z + s->m_nLength + run - 1);
	   z >= (Int) (s->z + s->m_nLength); z--) {		// for each cell of empty run
	if (run < (Int) m_vHor[x][y][z] ||			// pupitetris FIXME: this could be &&
	    run < (Int) m_vVer[x][y][z]) {			// new longitudinal strip < horizontal strip
	  removeOldValue(v,x,y,z);
	  v[run]++;                                             // insert new value
	}
	m_vLon[x][y][z] = run;					// new height of longitudinal strip
      }
    }
}

void Grid::add(const Rectangle* s) {
  Int x, y, z;
  Int run;								// length of horizontal or vertical or longitudinal run of empty space

  for (x = (Int) s->x; x < (Int) (s->x + s->m_nWidth); x++)		// for each column occupied by placement
    for (y = (Int) s->y; y < (Int) (s->y + s->m_nHeight); y++)		// for each row occupied by placement
      for(z = (Int) s->z; z < (Int) (s->z + s->m_nLength); z++)		// for each line occupied by placement
	operator[](x)[y][z] = s->m_nID;

  // Horizontal
  for (y = (Int) s->y; y < (Int) (s->y + s->m_nHeight); y++)	// for each row occupied by placement
    for(z = (Int) s->z; z < (Int) (s->z + s->m_nLength); z++) {	// for each line occupied by placement
      RUN_START(x,s->x - 1);

      // update horizontal grid above placement
      for (x = x + 1; x < (Int) s->x; x++)
	m_vHor[x][y][z] = run;					// new height of horizontal strip
      
      // update horizontal grid below placement
      run = (Int) (m_vHor[s->x][y][z] - run - s->m_nWidth);	// run below is previous - left - rectangle
      for(x = (Int) (s->x + s->m_nWidth + run - 1);
	  x >= (Int) (s->x + s->m_nWidth); x--)
	m_vHor[x][y][z] = run;					// new height of horizontal strip
    }

  // Vertical
  for(x = (Int) s->x; x < (Int) (s->x + s->m_nWidth); x++)	// for each column occupied by placement
    for(z = (Int) s->z; z < (Int) (s->z + s->m_nLength); z++) {	// for each line occupied by placement
      RUN_START(y,s->y - 1);

      // update vertical grid above placement
      for (y = y + 1; y < (Int) s->y; y++)
	m_vVer[x][y][z] = run;					// new height of vertical strip
      
      // update vertical grid below placement
      run = (Int) (m_vVer[x][s->y][z] - run - s->m_nHeight);	// run below is previous - above - rectangle
      for(y = (Int) (s->y + s->m_nHeight + run - 1);
	  y >= (Int) (s->y + s->m_nHeight); y--)
	m_vVer[x][y][z] = run;					// new height of vertical strip
    }

  // Longitudinal
  for(x = (Int) s->x; x < (Int) (s->x + s->m_nWidth); x++)	// for each column occupied by placement
    for(y = (Int) s->y; y < (Int) (s->y + s->m_nHeight); y++) { // for each row occupied by placement
      RUN_START(z,s->z - 1);

      // update longitudinal grid above placement
      std::fill(m_vLon[x][y].begin() + z + 1, 
		m_vLon[x][y].begin() + s->z, run);		// new height of longitudinal strip
      
      // update longitudinal grid below placement
      run = (Int) (m_vLon[x][y][s->z] - run - s->m_nLength);	// run below is previous - above - rectangle
      std::fill(m_vLon[x][y].begin() + s->z + s->m_nLength,
		m_vLon[x][y].begin() + s->z + s->m_nLength + run, run);	// new length of longitudinal strip.
    }
}

void Grid::add(Component* c) {

  /**
   * Draw the outline of the component.
   */

  Int x, y, z;
  Int run;              // length of horizontal or vertical run of empty space

  for (x = c->m_nX; x < (Int) (c->m_nX + c->m_Dims.m_nWidth); ++x)
    for (y = c->m_nY; y < (Int) (c->m_nY + c->m_Dims.m_nHeight); ++y)
      std::fill(operator[](x)[y].begin() + c->m_nZ,
		operator[](x)[y].begin() + c->m_nZ + c->m_Dims.m_nLength,
		c->m_nID);

  // Horizontal
  for(y = (Int) c->m_nY; y < (Int) (c->m_nY + c->m_Dims.m_nHeight); ++y)	// for each column occupied by placement
    for(z = (Int) c->m_nZ; z < (Int) (c->m_nZ + c->m_Dims.m_nLength); ++z) {    // for each line occupied by placement
      RUN_START(x,c->m_nX - 1);

      // update horizontal grid above placement
      for(x++; x < c->m_nX; ++x)
	m_vHor[x][y][z] = run;							// new width of horizontal strip. 
      
      // update horizontal grid below placement
      run = m_vHor[c->m_nX][y][z] - run - c->m_Dims.m_nWidth;                   // run below is previous - above - rectangle
      for (x = (Int) (c->m_nX + c->m_Dims.m_nWidth); 
	   x < (Int) (c->m_nX + c->m_Dims.m_nWidth) + run; ++x)
	m_vHor[x][y][z] = run;							// new width of horizontal strip.
    }

  // Vertical
  for(x = (Int) c->m_nX; x < (Int) (c->m_nX + c->m_Dims.m_nWidth); ++x)		// for each column occupied by placement
    for(z = (Int) c->m_nZ; z < (Int) (c->m_nZ + c->m_Dims.m_nLength); ++z) {    // for each line occupied by placement
      RUN_START(y,c->m_nY - 1);

      // update vertical grid above placement
      for(y++; y < c->m_nY; ++y)
	m_vVer[x][y][z] = run;							// new height of vertical strip. 
      
      /* pupitetris TODO: these structures should be:
	 m_vHor[y][z][x], m_vVer[x][z][y] for optimum performance. Some refactoring required. */
      
      // update vertical grid below placement
      run = m_vVer[x][c->m_nY][z] - run - c->m_Dims.m_nHeight;                  // run below is previous - above - rectangle
      for (y = (Int) (c->m_nY + c->m_Dims.m_nHeight); 
	   y < (Int) (c->m_nY + c->m_Dims.m_nHeight) + run; ++y)
	m_vVer[x][y][z] = run;							// new height of vertical strip.
    }

  // Longitudinal
  for(x = (Int) c->m_nX; x < (Int) (c->m_nX + c->m_Dims.m_nWidth); ++x)			// for each column occupied by placement
    for(y = (Int) c->m_nY; y < (Int) (c->m_nY + c->m_Dims.m_nHeight); ++y) {		// for each column occupied by placement
      RUN_START(z,c->m_nZ - 1);

      // update longitudinal grid above placement
      std::fill(m_vLon[x][y].begin() + z + 1, m_vLon[x][y].begin() + c->m_nZ, run);	// new length of longitudinal strip.
      
      // update longitudinal grid below placement
      run = m_vLon[c->m_nZ][y][z] - run - c->m_Dims.m_nLength;				// run below is previous - above - rectangle
      std::fill(m_vLon[x][y].begin() + c->m_nZ + c->m_Dims.m_nLength,
		m_vLon[x][y].begin() + c->m_nZ + c->m_Dims.m_nLength + run, run);	// new length of longitudinal strip.
    }
}

void Grid::add(Component* c, std::vector<Int>& v) {
  Int x, y, z;
  Int run;              // length of horizontal or vertical run of empty space

  for (x = c->m_nX; x < (Int) (c->m_nX + c->m_Dims.m_nWidth); ++x)
    for (y = c->m_nY; y < (Int) (c->m_nY + c->m_Dims.m_nHeight); ++y)
      for (z = c->m_nZ; z < (Int) (c->m_nZ + c->m_Dims.m_nLength); ++z) {
	operator[](x)[y][z] = c->m_nID;
	removeOldValue(v,x,y,z);
      }

  // Horizontal
  for(y = c->m_nY; y < (Int) (c->m_nY + c->m_Dims.m_nHeight); ++y)	// for each column occupied by placement
    for (z = c->m_nZ; z < (Int) (c->m_nZ + c->m_Dims.m_nLength); ++z) { // for each line occupied by placement
      RUN_START(x,c->m_nX - 1);

      // update horizontal grid above placement
      for (x = x + 1; x < (Int) c->m_nX; ++x) {                         // for each cell of empty run
	if (run < (Int) m_vHor[x][y][z] ||				// pupitetris FIXME: this could be &&
	    run < (Int) m_vLon[x][y][z]) {				// new horizontal strip < longitudinal strip
	  removeOldValue(v,x,y,z);
	  ++v[run];                                                     // insert new value
	}
	m_vHor[x][y][z] = run;                                          // new width of horizontal strip
      }

      // update horizontal grid below placement
      run = m_vHor[c->m_nX][y][z] - run - c->m_Dims.m_nWidth;           // run below is previous - above - rectangle
      for(x = (Int) c->m_nX + c->m_Dims.m_nWidth + run - 1;
	  x >= (Int) (c->m_nX + c->m_Dims.m_nWidth); --x) {             // for each cell of empty run
	if (run < (Int) m_vHor[x][y][z] ||				// pupitetris FIXME: this could be &&
	    run < (Int) m_vLon[x][y][z]) {				// new horizontal strip < longitudinal strip
	  removeOldValue(v,x,y,z);
	  ++v[run];                                                     // insert new value
	}
	m_vHor[x][y][z] = run;                                          // new width of horizontal strip
      }
    }

  // Vertical
  for(x = c->m_nX; x < (Int) (c->m_nX + c->m_Dims.m_nWidth); ++x)	// for each row occupied by placement
    for (z = c->m_nZ; z < (Int) (c->m_nZ + c->m_Dims.m_nLength); ++z) { // for each line occupied by placement
      RUN_START(y,c->m_nY - 1);

      // update vertical grid above placement
      for (y = y + 1; y < (Int) c->m_nY; ++y) {                         // for each cell of empty run
	if (run < (Int) m_vHor[x][y][z] ||				// pupitetris FIXME: this could be &&
	    run < (Int) m_vLon[x][y][z]) {				// new vertical strip < horizontal strip
	  removeOldValue(v,x,y,z);
	  ++v[run];                                                     // insert new value
	}
	m_vVer[x][y][z] = run;                                          // new height of vertical strip
      }

      // update vertical grid below placement
      run = m_vVer[x][c->m_nY][z] - run - c->m_Dims.m_nHeight;          // run below is previous - above - rectangle
      for(y = (Int) c->m_nY + c->m_Dims.m_nHeight + run - 1;
	  y >= (Int) (c->m_nY + c->m_Dims.m_nHeight); --y) {            // for each cell of empty run
	if (run < (Int) m_vHor[x][y][z] ||				// pupitetris FIXME: this could be &&
	    run < (Int) m_vLon[x][y][z]) {				// new vertical strip < horizontal strip
	  removeOldValue(v,x,y,z);
	  ++v[run];                                                     // insert new value
	}
	m_vVer[x][y][z] = run;                                          // new height of vertical strip
      }
    }

  // Longitudinal
  for(x = c->m_nX; x < (Int) (c->m_nX + c->m_Dims.m_nWidth); ++x)	// for each row occupied by placement
    for(y = c->m_nY; y < (Int) (c->m_nY + c->m_Dims.m_nHeight); ++y) {	// for each column occupied by placement
      RUN_START(z,c->m_nZ - 1);

      // update longitudinal grid above placement
      for (z = z + 1; z < (Int) c->m_nZ; ++z) {                         // for each cell of empty run
	if (run < (Int) m_vHor[x][y][z] ||				// pupitetris FIXME: this could be &&
	    run < (Int) m_vVer[x][y][z]) {				// new longitudinal strip < vertical strip
	  removeOldValue(v,x,y,z);
	  ++v[run];                                                     // insert new value
	}
	m_vLon[x][y][z] = run;                                          // new length of longitudinal strip
      }

      // update longitudinal grid below placement
      run = m_vLon[x][y][c->m_nZ] - run - c->m_Dims.m_nLength;          // run below is previous - above - rectangle
      for(z = (Int) c->m_nZ + c->m_Dims.m_nLength + run - 1;
	  z >= (Int) (c->m_nZ + c->m_Dims.m_nLength); --z) {            // for each cell of empty run
	if (run < (Int) m_vHor[x][y][z] ||				// pupitetris FIXME: this could be &&
	    run < (Int) m_vVer[x][y][z]) {				// new longitudinal strip < vertical strip
	  removeOldValue(v,x,y,z);
	  ++v[run];                                                     // insert new value
	}
	m_vLon[x][y][z] = run;                                          // new length of longitudinal strip
      }
    }
}

void Grid::del(Component* c) {
  Int x, y, z;		// coordinates of the grid to test
  Int horz, vert, lon;	// old horizontal width and vertical height and longitudinal length
  Int left, top, far;   // index of top of old empty strip

  for(x = c->m_nX; x < (Int) (c->m_nX + c->m_Dims.m_nWidth); x++)
    for(y = c->m_nY; y < (Int) (c->m_nY + c->m_Dims.m_nHeight); y++)         // for each column occupied by square
      std::fill(operator[](x)[y].begin() + c->m_nZ,
		operator[](x)[y].begin() + c->m_nZ + c->m_Dims.m_nLength,
		GRIDEMPTY);

  // Horizontal
  for(z = c->m_nZ; z < (Int) (c->m_nZ + c->m_Dims.m_nLength); z++) {    // each line occupied by placement
    for(y = c->m_nY; y < (Int) (c->m_nY + c->m_Dims.m_nHeight); y++) {  // for each row occupied by placement
      horz = m_vHor[c->m_nX][y][z];					// old width of horizontal strip

      if(c->m_nX >= 1 && empty((Int) (c->m_nX - 1), y, z))		// row has at least one empty position to left
	left = c->m_nX - m_vHor[c->m_nX - 1][y][z];			// index of left edge of old empty row
      else
	left = c->m_nX;

      for(x = left; x < (Int) c->m_nX; x++)				// for each cell of row to left
	m_vHor[x][y][z] = horz;						// new horizontal width of row
      for(x = c->m_nX + c->m_Dims.m_nWidth; x <= left + horz - 1; x++)  // each cell of column
	m_vHor[x][y][z] = horz;
    }
  }

  // Vertical
  for(z = c->m_nZ; z < (Int) (c->m_nZ + c->m_Dims.m_nLength); z++) {	// each line occupied by placement
    for(x = c->m_nX; x < (Int) (c->m_nX + c->m_Dims.m_nWidth); x++) {	// each column occupied by placement
      vert = m_vVer[x][c->m_nY][z];					// old height of vertical strip

      if (c->m_nY >= 1 && empty(x, (Int) c->m_nY - 1, z))		// column has at least one empty position above
	top = c->m_nY - m_vVer[x][c->m_nY - 1][z];			// index of top of old vertical strip
      else
	top = c->m_nY;

      for(y = top; y < (Int) c->m_nY; y++)				// for each cell of column
	m_vVer[x][y][z] = vert;						// new vertical height of column
      for(y = c->m_nY + c->m_Dims.m_nHeight; y <= top + vert - 1; y++)	// each cell of column below
	m_vVer[x][y][z] = vert;						// old vertical height of column
    }
  }

  // Longitudinal
  for(x = c->m_nX; x < (Int) (c->m_nX + c->m_Dims.m_nWidth); x++)	// each column occupied by placement
    for(y = c->m_nY; y < (Int) (c->m_nY + c->m_Dims.m_nHeight); y++) {  // for each row occupied by placement
      lon = m_vLon[x][y][c->m_nZ];					// old length of longitudinal strip

      if(c->m_nZ >= 1 && empty(x, y, (Int) (c->m_nZ - 1)))		// line has at least one empty position to back
	far = c->m_nZ - m_vLon[x][y][c->m_nZ - 1];			// index of far edge of old empty line
      else
	far = c->m_nZ;

      std::fill(m_vLon[x][y].begin() + far, 
		m_vLon[x][y].begin() + c->m_nZ, lon);			// new length of longitudinal strip
      std::fill(m_vLon[x][y].begin() + c->m_nZ + c->m_Dims.m_nLength, 
		m_vLon[x][y].begin() + far + lon, lon);			// new length of longitudinal strip
    }
}

void Grid::print() const {
  printAux(*this);
}

void Grid::printid() const {
  printX();
  std::cout << std::endl;
  for(UInt z = 0; z < m_Box.m_nLength; ++z) {
    std::cout << std::endl << "Z = " << z << std::endl;
    for(UInt y = 0; y < m_Box.m_nHeight; ++y) {
      std::cout << std::setw(2) << y << " ";
      for(UInt x = 0; x < m_Box.m_nWidth; ++x)
	if(empty(x, y, z))
	  std::cout << " .";
	else
	  std::cout << std::setw(2) << (Int) get(x, y, z);
      std::cout << std::endl;
    }
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

void Grid::printAux(const std::vector<std::vector<std::vector<UInt> > >& v) const {
  for(UInt z = 0; z < m_Box.m_nLength; z++) {
    std::cout << std::endl << "Z = " << z << std::endl;
    for(UInt y = (UInt) (m_Box.m_nHeight - 1); y >= 0; --y) {
      for(UInt x = 0; x < m_Box.m_nWidth; x++)
	if(v[x][y][z] == GRIDEMPTY)
	  std::cout << " .";
	else
	  std::cout << std::setw(2) << std::setfill(' ') << v[x][y][z];
      std::cout << std::endl;
    }
  }
}

void Grid::printRaw(const std::vector<std::vector<std::vector<UInt> > >& v) const {
  for(UInt z = 0; z < m_Box.m_nLength; z++) {
    std::cout << std::endl << "Z = " << z << std::endl;
    for(UInt y = 0; y < m_Box.m_nHeight; ++y) {
      for(UInt x = 0; x < m_Box.m_nWidth; x++)
	if(v[x][y][z] == 0)
	  std::cout << " .";
	else
	  std::cout << std::setw(2) << (Int) v[x][y][z];
      std::cout << std::endl;
    }
  }
}

void Grid::set(const BoxDimensions& b) {
  m_Box = b;
}
