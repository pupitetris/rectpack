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

#include "SquarePacker.h"
#include "Domination.h"
#include "Parameters.h"
#include "Grid.h"
#include "GridViz.h"
#include <math.h>
#include <iostream>
#include <algorithm>
#include <assert.h>

/**
 * The create rect function as implemented by Rich has a bug that
 * causes us to miss solutions. Setting the following flag to 0
 * disables the buggy function.
 */

#define CREATERECT 0
#define TOOCLOSEFIX 1

SquarePacker::SquarePacker() :
  m_bTooCloseEnable(true) {
}

SquarePacker::~SquarePacker() {
}

void SquarePacker::initialize(const Parameters* pParams) {
  IntPack::initialize(pParams);
  m_vWastedXF.resize(m_vRects.size());
  for(std::vector<WastedBins>::iterator i = m_vWastedXF.begin();
      i != m_vWastedXF.end(); ++i)
    i->initialize(pParams);
  if(pParams->m_s1.find_first_of('d') < pParams->m_s1.size())
    m_bTooCloseEnable = false;
}

bool SquarePacker::pack() {
  Rectangle* r(m_vRectPtrs.front());
  WastedBins v(std::min(m_Box.m_nWidth, m_Box.m_nHeight) + 1, 0);
  v[std::min(m_Box.m_nWidth, m_Box.m_nHeight)] = m_Box.m_nArea;
  if(!m_pParams->m_vInstance.m_bSortedDecreasingMinDim) {
    initRMinDims();
    m_sRMinDims.erase(r);
  }

  if(m_Box.canFit(r) &&
     packFirst(m_vRectPtrs.begin(), m_vRectPtrs.end(), v))
    return(true);
  if(r->rotatable()) {
    r->rotate();
    if(m_Box.canFit(r) &&
       packFirst(m_vRectPtrs.begin(), m_vRectPtrs.end(), v))
      return(true);
  }
  return(false);
}

bool SquarePacker::packFirst(RectPtrArray::iterator iCurrent,
			     RectPtrArray::const_iterator iEnd,
			     const WastedBins& v) {
  Rectangle* r(*iCurrent);
  if(r->fixed()) {
    r->x = r->m_nFixX;
    r->y = r->m_nFixY;
    m_vWastedXF[r->m_nID] = v;
    m_pGrid->add(r, m_vWastedXF[r->m_nID]);
    m_Nodes.tick(XF);
    if(m_vRectPtrs.size() == 1 ||
       packAux(iCurrent + 1, iEnd, m_vWastedXF[r->m_nID]))
      return(true);
    m_pGrid->del(r);
  }
  else {
    UInt xdist = (m_Box.m_nWidth - r->m_nWidth) / 2;   /* x distance is floor(x/2-n/2) */
    UInt ydist = (m_Box.m_nHeight - r->m_nHeight) / 2;   /* y distance is floor(x/2-n/2) */
    for(r->x = 0; r->x <= xdist; r->x++) {          /* each column of upper-left quadrant */
      if(m_pDomination->dominatedh(r, r->x)) continue;        /* too close to left edge */
      for(r->y = 0; r->y <= ydist; r->y++) {           /* each row of upper-left quadrant */
	if(m_pDomination->dominatedw(r, r->y)) continue;   /* not too close to top edge */
	m_vWastedXF[r->m_nID] = v;
	m_pGrid->add(r, m_vWastedXF[r->m_nID]);
	m_Nodes.tick(XF);
	if((m_pParams->m_vInstance.m_bSortedDecreasingMinDim &&
	    m_vWastedXF[r->m_nID].canFitm(iCurrent + 1, iEnd)) ||
	   (!m_pParams->m_vInstance.m_bSortedDecreasingMinDim && 
	    m_vWastedXF[r->m_nID].canFit(m_sRMinDims)))
	  if(m_vRectPtrs.size() == 1 ||
	     packAux(iCurrent + 1, iEnd, m_vWastedXF[r->m_nID]))
	    return(true);
	m_pGrid->del(r);
      }
    }
  }
  return(false);
}

bool SquarePacker::packAux(RectPtrArray::iterator iCurrent,
			   RectPtrArray::const_iterator iEnd,
			   const WastedBins& v) {
  if(iCurrent == iEnd)
    return(true);
  if(m_Box.canFit(*iCurrent) &&
     packRest(iCurrent, iEnd, v))
    return(true);
  if((*iCurrent)->rotatable()) {
    (*iCurrent)->rotate();
    if(m_Box.canFit(*iCurrent) &&
       packRest(iCurrent, iEnd, v))
      return(true);
  }
  return(false);
}

bool SquarePacker::packRest(RectPtrArray::iterator iCurrent,
			    RectPtrArray::const_iterator iEnd,
			    const WastedBins& v) {
  Rectangle* r(*iCurrent);
  if(r->fixed()) {
    r->x = r->m_nFixX;
    r->y = r->m_nFixY;
    m_vWastedXF[r->m_nID] = v;
    m_pGrid->add(r, m_vWastedXF[r->m_nID]);
    m_Nodes.tick(XF);
    if(m_pParams->m_vInstance.m_bSortedDecreasingMinDim) {
      if(packAux(iCurrent + 1, iEnd, m_vWastedXF[r->m_nID]))
	return(true);
    }
    else {
      m_sRMinDims.erase(r);
      if(packAux(iCurrent + 1, iEnd, m_vWastedXF[r->m_nID]))
	return(true);
      m_sRMinDims.insert(r);
    }
    m_pGrid->del(r);
  }
  else {
    for(r->x = 0; r->x <= (m_Box.m_nWidth - r->m_nWidth); r->x++)     /* each column of grid where square could fit */
      for(r->y = 0; r->y <= (m_Box.m_nHeight - r->m_nHeight); r->y++)      /* each row of grid where square could fit */
	if(m_pGrid->occupied(r->x, r->y))
	  r->y += m_pGrid->height(r->x, r->y) - 1;       /* skip over rectangle in place */
	else if(m_pGrid->m_vVer[r->x][r->y] < r->m_nHeight)
	  r->y += m_pGrid->m_vVer[r->x][r->y] - 1;
	else if((m_pParams->m_vInstance.m_bSortedDecreasingSize &&
		 m_pGrid->empty(r->x + r->m_nWidth - 1, r->y) &&
		 m_pGrid->empty(r->x, r->y + r->m_nHeight - 1) &&
		 m_pGrid->empty(r->x + r->m_nWidth - 1, r->y + r->m_nHeight - 1)) ||
		(!m_pParams->m_vInstance.m_bSortedDecreasingSize &&
		 !m_pGrid->areaOccupied(r))) {
	  m_Nodes.tick(XF);
	  if(!tooClose(r)) {
	    m_vWastedXF[r->m_nID] = v;
	    m_pGrid->add(r, m_vWastedXF[r->m_nID]);
	    if(m_pParams->m_vInstance.m_bSortedDecreasingMinDim) {
	      if(m_vWastedXF[r->m_nID].canFitm(iCurrent + 1, iEnd) &&
		 (!CREATERECT || !createRect(r))) {
		if(packAux(iCurrent + 1, iEnd, m_vWastedXF[r->m_nID]))
		  return(true);
	      }
	    }
	    else {
	      m_sRMinDims.erase(r);
	      if(m_vWastedXF[r->m_nID].canFit(m_sRMinDims) &&
		 (!CREATERECT || !createRect(r))) {
		if(packAux(iCurrent + 1, iEnd, m_vWastedXF[r->m_nID]))
		  return(true);
	      }
	      m_sRMinDims.insert(r);
	    }
	    m_pGrid->del(r);
	  }
	}
  }
  return(false);  
}

bool SquarePacker::createRect(const Rectangle* r) {
  Int x1, y1;                                               /* utility indices */

  if(r->x > 0) {                               /* square is not against left edge */
    if(r->y > 0) y1 = r->y - 1;
    else y1 = r->y;                      /* scan cells immediately left of square */
    while(y1 < (Int) (r->y + r->m_nHeight) &&
	  m_pGrid->occupied(r->x - 1, y1)) y1++;   /* look for first empty cell */
    while(y1 < (Int) (r->y + r->m_nHeight) &&
	  m_pGrid->empty(r->x - 1, (UInt) y1)) y1++;   /* look for first full cell */
    if(y1 < (Int) (r->y + r->m_nHeight) &&                  /* found square to left of current square */
       rectAbove(m_pGrid->rect(r->x - 1, y1)))
      return(true);
  }

  if(r->x + r->m_nWidth < m_Box.m_nWidth) {                          /* square is not against right edge */
    if(r->y > 0) y1 = r->y - 1;  /* */
    else y1 = r->y;                     /* scan cells immediately right of square */
    while(y1 < (Int) (r->y + r->m_nHeight) &&
	  m_pGrid->occupied(r->x + r->m_nWidth, y1)) y1++;   /* look for first empty cell */
    while(y1 < (Int) (r->y + r->m_nHeight) &&
	  m_pGrid->empty(r->x + r->m_nWidth, (UInt) y1)) y1++;   /* look for first full cell */
    if(y1 < (Int) (r->y + r->m_nHeight) &&                 /* found square to right of current square */
       rectAbove(m_pGrid->rect(r->x + r->m_nWidth, y1)))
      return(true);
  }
  
  if(r->y > 0) {                             /* square is not against bottom edge */
    if(r->x > 0) x1 = r->x - 1;             /* scan cells immediately below square */
    else x1 = r->x;
    while(x1 < (Int) (r->x + r->m_nWidth) &&
	  m_pGrid->occupied(x1, r->y - 1)) x1++;   /* look for first empty cell */
    while(x1 < (Int) (r->x + r->m_nWidth) &&
	  m_pGrid->empty((UInt) x1, r->y - 1)) x1++;   /* look for first full cell */
    if(x1 < (Int) (r->x + r->m_nWidth) &&                  /* found square to left of current square */
       rectLeft(m_pGrid->rect(x1, r->y - 1)))
      return(true);
  }

  if(r->y + r->m_nHeight < m_Box.m_nHeight) {                    /* square is not against top edge */
    if(r->x > 0) x1 = r->x - 1;             /* scan cells immediately above square */
    else x1 = r->x;
    while(x1 < (Int) (r->x + r->m_nWidth) && m_pGrid->occupied(x1, r->y + r->m_nHeight)) x1++;   /* look for first empty cell */
    while(x1 < (Int) (r->x + r->m_nWidth) && m_pGrid->empty((UInt) x1, r->y + r->m_nHeight)) x1++;   /* look for first full cell */
    if(x1 < (Int) (r->x + r->m_nWidth) &&                  /* found square to left of current square */
       rectLeft(m_pGrid->rect(x1, r->y + r->m_nHeight)))
      return(true);
  }
  return(false);  
}

bool SquarePacker::rectAbove(const Rectangle* r) const {
  Int x1, y1;                                           /* utility coordinates */

  if(r->y == 0) return(false);          /* square is flush against top edge, no gap */

  for(y1 = r->y - 1; y1 >= 0; --y1)                       /* each row above square */
    {for(x1 = r->x; x1 < (Int) (r->x + r->m_nWidth); ++x1)                  /* each position in the row */
	if(m_pGrid->occupied(x1, y1)) break;                   /* whole row is not empty */
      if(x1 == (Int) (r->x + r->m_nWidth))                                     /* whole row is empty */
	{if(r->x > 0 && m_pGrid->empty(r->x - 1, (UInt) y1))
	    return(false);  /* left wall not solid */
	  if(r->x + r->m_nWidth < m_Box.m_nWidth &&
	     m_pGrid->empty(r->x + r->m_nWidth, (UInt) y1))
	    return(false);} /* right wall not solid */
      else break;}                      /* row y1 is at least partially occupied */

  if(y1 < 0) return(true);                     /* top wall is boundary, success */

  for(x1 = r->x; x1 < (Int) (r->x + r->m_nWidth); ++x1)                     /* for each position in row */
    if(m_pGrid->empty(x1, y1)) return(false);                   /* top wall not solid */
  return(true);  
}

bool SquarePacker::rectLeft(const Rectangle* r) const {
  Int x1, y1;                                           /* utility coordinates */

  if(r->x == 0) return(false);         /* square is flush against left edge, no gap */

  for(x1 = r->x - 1; x1 >= 0; --x1)                  /* each column left of square */
    {for(y1 = r->y; y1 < (Int) (r->y + r->m_nHeight); ++y1)               /* each position in the column */
	if(m_pGrid->occupied(x1, y1)) break;                /* whole column is not empty */
      if(y1 == (Int) (r->y + r->m_nHeight))                                  /* whole column is empty */
	{if(r->y > 0 && m_pGrid->empty((UInt) x1, r->y - 1)) return(false);   /* top wall not solid */
	  if(r->y + r->m_nHeight < m_Box.m_nHeight &&
	     m_pGrid->empty((UInt) x1, r->y + r->m_nHeight))
	    return(false);} /*bottom wall not solid */
      else break;}                      /* row y1 is at least partially occupied */

  if(x1 < 0) return(true);                    /* left wall is boundary, success */

  for(y1 = r->y; y1 < (Int) (r->y + r->m_nHeight); y1++)                  /* for each position in column */
    if(m_pGrid->empty(x1, y1)) return(false);                  /* left wall not solid */
  return(true);
}

bool SquarePacker::tooClose(const Rectangle* r) {
  if(!m_bTooCloseEnable) return(false);

  Int x1, y1;                                     /* utility indices into grid */
  Int ygap;                /* vertical gap between top edge and next square up */
  Int xgap;           /* horizontal gap between left edge and next square left */
  /* is rectangle too close to rectangles above it? */
  if(r->y > 0
#if TOOCLOSEFIX
     && (r->y + r->m_nHeight < m_Box.m_nHeight)
#endif // TOOCLOSEFIX
     )                                    /* possible vertical gap above */
    {for(y1 = r->y - 1; y1 >= 0; y1--) /*check each cell above upper-left corner */
	if(m_pGrid->occupied(r->x, y1)) break;                         /* occupied, no more gap */
      ygap = r->y - y1 - 1;                          /* gap above upper-left corner */
      if(ygap > 0)                       /* there is potentially a vertical gap */
	{x1 = (Int) (r->x + r->m_nWidth) - 1;             /* x coordinate of rightmost cell of square */
	  for(y1 = r->y - 1; y1 >= 0; y1--)                 /* check each cell above */
	    if(m_pGrid->occupied(x1, y1)) break;                      /* occupied, no more gap */
	  if((Int) (r->y  - y1 - 1) == ygap)                       /* gap same on both sides */
	    {if(y1 >= 0)                  /* top of empty strip is not top of box */
		{for(x1 = r->x; x1 < (Int) (r->x + r->m_nWidth); x1++)    /* see if row is solidly occupied */
		    if(m_pGrid->empty(x1, y1)) break;            /* hole in upper boundary */
		  if(x1 < (Int) (r->x + r->m_nWidth)) ygap = 0;} /* broke out prematurely, hole in upper boundary */
	      if(m_pDomination->dominatedw(r, ygap)) return(true);} /*forbidden gap width*/
	  else goto next1;}                /* different gap heights on either side */

      /* is there a perfect rectangle of empty space above rectangle? */
      if(ygap == 0) goto next1;                        /* no vertical gap above */
      if(r->x > 0)                                    /* not against left boundary */
	{for(y1 = (Int) (r->y - 1); y1 >= (Int) (r->y - ygap); y1--)               /* each row of gap */
	    if(m_pGrid->empty(r->x - 1, (UInt) y1)) break;        /* not solid wall to left of gap */
	  if(y1 >= (Int) (r->y - ygap)) goto next1;} /* broke out prematurely, no solid wall */
      if(r->x + r->m_nWidth < m_Box.m_nWidth)                               /* not against right boundary */
	{for(y1 = (Int) (r->y - 1); y1 >= (Int) (r->y - ygap); y1--)               /* each row of gap */
	    if(m_pGrid->empty(r->x + r->m_nWidth, (UInt) y1)) break;     /* not solid wall to right of gap */
	  if(y1 >= (Int) (r->y - ygap)) goto next1;} /* broke out prematurely, no solid wall */
      return(true);}       /* solid walls to left and right, could slide square up */

 next1:                     /* is rectangle too close to rectangles below it? */
  if(r->y + r->m_nHeight < m_Box.m_nHeight)                                /* possible vertical gap below */
    {for(y1 = (Int) (r->y + r->m_nHeight); y1 < (Int) m_Box.m_nHeight; y1++)  /*check each cell below lower-left corner */
	if(m_pGrid->occupied(r->x, y1)) break;                         /* occupied, no more gap */
      ygap = y1 - (r->y + r->m_nHeight);                        /* gap below lower-left corner */
      if(ygap > 0)                       /* there is potentially a vertical gap */
	{x1 = (Int) (r->x + r->m_nWidth) - 1;             /* x coordinate of rightmost cell of square */
	  for(y1 = r->y + r->m_nHeight; y1 < (Int) m_Box.m_nHeight; y1++)                  /* check each cell above */
	    if(m_pGrid->occupied(x1, y1)) break;                      /* occupied, no more gap */
	  if((Int) (y1 - (r->y + r->m_nHeight)) == ygap)                      /* gap same on both sides */
	    {if(y1 < (Int) m_Box.m_nHeight)             /* bottom of empty strip is not bottom of box */
		{for(x1 = r->x; x1 < (Int) (r->x + r->m_nWidth); x1++)      /* see if row is solidly occupied */
		    if(m_pGrid->empty(x1, y1)) break;            /* hole in upper boundary */
		  if(x1 < (Int) (r->x + r->m_nWidth)) ygap = 0;} /* broke out prematurely, gap in lower boundary */
	      if(r->y > 0)                                     /* not against top edge */
		{for(x1 = r->x; x1 < (Int) (r->x + r->m_nWidth); x1++) /*check row immediately above rectangle*/
		    if(m_pGrid->empty((UInt) x1, r->y - 1)) break;           /* row above is not solid */
		  if(x1 == (Int) (r->x + r->m_nWidth)) goto next2;  /* row above is solid */
		  if(x1 < (Int) (r->x + r->m_nWidth) && m_pDomination->dominatedw(r, ygap)) return(true);}}/*forbidden gap width*/
	  else ygap = 0;}}
  else ygap = 0;                                      /* no vertical gap below */

 next2:        /* is there a perfect rectangle of empty space below rectangle? */
  if(ygap == 0) goto next3;                          /* no vertical gap below */
  if(r->x > 0)                                      /* not against left boundary */
    {for(y1 = (Int) (r->y + r->m_nHeight);
	 y1 < (Int) (r->y + r->m_nHeight + ygap); y1++)              /* each row of gap */
	if(m_pGrid->empty(r->x - 1, (UInt) y1)) break;          /* not solid wall to left of gap */
      if(y1 < (Int) (r->y + r->m_nHeight + ygap)) goto next3;} /*broke out prematurely, no solid wall */
  if(r->x + r->m_nWidth < m_Box.m_nWidth)                                   /* not against right boundary */
    {for(y1 = (Int) (r->y + r->m_nHeight);
	 y1 < (Int) (r->y + r->m_nHeight + ygap); y1++)              /* each row of gap */
	if(m_pGrid->empty(r->x + r->m_nWidth, (UInt) y1)) break;         /* not solid wall to right of gap */
      if(y1 < (Int) (r->y + r->m_nHeight + ygap)) goto next3;} /*broke out prematurely, no solid wall */
  if(r->y > 0)                                           /* not against top edge */
    for(x1 = r->x; x1 < (Int) (r->x + r->m_nWidth); x1++)       /* is there hole in top edge of rect? */
      if(m_pGrid->empty((UInt) x1, r->y - 1)) return(true);         /* yes, no placement allowed */
  if(m_pDomination->dominatedw(r, ygap)) goto next3; /*top wall solid, narrow gap */
  if(r->x > 0)                                          /* not against left edge */
    for(y1 = r->y; y1 < (Int) (r->y + r->m_nHeight); y1++)    /* is there a hole in left side of rect? */
      if(m_pGrid->empty(r->x - 1, (UInt) y1)) return(true);         /* yes, no placement allowed */
  if(r->x + r->m_nWidth < m_Box.m_nWidth)                                     /* not against right edge */
    for(y1 = r->y; y1 < (Int) (r->y + r->m_nHeight); y1++)   /* is there a hole in right side of rect? */
      if(m_pGrid->empty(r->x + r->m_nWidth, (UInt) y1)) return(true);         /* yes, no placement allowed */

 next3:                  /* is rectangle too close to rectangles to its left? */
  if(r->x > 0 
#if TOOCLOSEFIX
     && (r->x + r->m_nWidth < m_Box.m_nWidth)
#endif // TOOCLOSEFIX
     ) /* possible horizontal gap */
    {for(x1 = r->x - 1; x1 >= 0; x1--) /*check each cell left of upper-left corner*/
	if(m_pGrid->occupied(x1, r->y)) break;                         /* occupied, no more gap */
      xgap = r->x - x1 - 1;                        /* gap left of upper-left corner */
      if(xgap > 0)                                 /* there is a horizontal gap */
	{y1 = r->y + r->m_nHeight - 1;            /* y coordinate of bottommost cell of square */
	  for(x1 = r->x - 1; x1 >= 0; x1--)           /* check each cell to the left */
	    if(m_pGrid->occupied(x1, y1)) break;                      /* occupied, no more gap */
	  if((Int) (r->x  - x1 - 1) == xgap)                   /* gap same on top and bottom */
	    {if(x1 >= 0)       /* left of empty strip is not left boundary of box */
		{for(y1 = r->y; y1 < (Int) (r->y + r->m_nHeight); y1++)  /*see if column is solidly occupied */
		    if(m_pGrid->empty(x1, y1)) break;             /* hole in left boundary */
		  if(y1 < (Int) (r->y + r->m_nHeight)) xgap = 0;}          /* broke out prematurely, no gap */
	      if(m_pDomination->dominatedh(r, xgap)) return(true);} /*forbidden gap width*/
	  else goto next4;}                   /* different gap widths on each side */

      /* is there a perfect rectangle of empty space to the left of rectangle? */
      if(xgap == 0) goto next4;                    /* no horizontal gap to left */
      if(r->y > 0)                                     /* not against top boundary */
	{for(x1 = r->x - 1; x1 >= (Int) (r->x - xgap); x1--)            /* each column of gap */
	    if(m_pGrid->empty((UInt) x1, r->y - 1)) goto next4;     /* not solid wall above of gap */
	  if(x1 >= (Int) (r->x - xgap)) goto next4;} /* broke out prematurely, no solid wall */
      if(r->y + r->m_nHeight < m_Box.m_nHeight)                              /* not against bottom boundary */
	{for(x1 = r->x - 1; x1 >= (Int) (r->x - xgap); x1--)            /* each column of gap */
	    if(m_pGrid->empty((UInt) x1, r->y + r->m_nHeight)) break;     /* not solid wall to right of gap */
	  if(x1 >= (Int) (r->x - xgap)) goto next4;} /* broke out prematurely, no solid wall */
      return(true);}       /* solid walls above and below, could slide square left */

 next4:           /* is rectangle too close to rectangles to the right of it? */
  if(r->x + r->m_nWidth < m_Box.m_nWidth)                           /* possible horizontal gap to right */
    {for(x1 = (Int) (r->x + r->m_nWidth); x1 < (Int) m_Box.m_nWidth; x1++)  /*check each cell right of uppper-right corner*/
	if(m_pGrid->occupied(x1, r->y)) break;                         /* occupied, no more gap */
      xgap = x1 - (r->x + r->m_nWidth);                 /* gap to right of upper-right corner */
      if(xgap > 0)                     /* there is potentially a horizontal gap */
	{y1 = r->y + r->m_nHeight - 1;                /* y coordinate of lowest cell of square */
	  for(x1 = (Int) (r->x + r->m_nWidth); x1 < (Int) m_Box.m_nWidth; x1++)               /* check each cell to right */
	    if(m_pGrid->occupied(x1, y1)) break;                      /* occupied, no more gap */
	  if((Int) (x1 - (r->x + r->m_nWidth)) == xgap)                      /* gap same on both sides */
	    {if(x1 < (Int) m_Box.m_nWidth)          /* right of empty strip is not right edge of box */
		{for(y1 = r->y; y1 < (Int) (r->y + r->m_nHeight); y1++)      /* see if row is solidly occupied */
		    if(m_pGrid->empty(x1, y1)) break;            /* hole in right boundary */
		  if(y1 < (Int) (r->y + r->m_nHeight)) xgap = 0;}          /* broke out prematurely, no gap */
	      if(r->x > 0)                                    /* not against left edge */
		{for(y1 = r->y; y1 < (Int) (r->y + r->m_nHeight); y1++) /*check col immediately left of rectangle*/
		    if(m_pGrid->empty(r->x - 1, (UInt) y1)) break;         /* col to left is not solid */
		  if(y1 == (Int) (r->y + r->m_nHeight)) goto next5;                  /* wall to left is solid */
		  if(y1 < (Int) (r->y + r->m_nHeight) &&
		     m_pDomination->dominatedh(r, xgap))
		    return(true);}} /*forbidden gap width*/
	  else return(false);}}             /* different gap widths on top and bottom */
  else return(false);

 next5:  /* is there a perfect rectangle of empty space to right of rectangle? */
  if(xgap == 0) return(false);                  /* no horizontal gap to right */
  if(r->y > 0)                                           /* not against top edge */
    {for(x1 = (Int) (r->x + r->m_nWidth); x1 < (Int) (r->x + r->m_nWidth) + xgap; x1++)           /* each column of gap */
	if(m_pGrid->empty((UInt) x1, r->y - 1)) break;          /* not solid wall above gap */
      if(x1 < (Int) (r->x + r->m_nWidth) + xgap) return(false);}       /* not solid wall above gap */
  if(r->y + r->m_nHeight < m_Box.m_nHeight)                                /* not against bottom edge */
    {for(x1 = (Int) (r->x + r->m_nWidth); x1 < (Int) (r->x + r->m_nWidth) + xgap; x1++)           /* each column of gap */
	if(m_pGrid->empty((UInt) x1, r->y + r->m_nHeight)) break;          /* not solid wall below gap */
      if(x1 < (Int) (r->x + r->m_nWidth) + xgap) return(false);}       /* not solid wall below gap */
  if(r->x > 0)                                          /* not against left edge */
    for(y1 = r->y; y1 < (Int) (r->y + r->m_nHeight); y1++)      /* is there hole in left edge of rect? */
      if(m_pGrid->empty(r->x - 1, (UInt) y1)) return(true);    /* yes, no placement allowed */
  if(m_pDomination->dominatedh(r, xgap)) return(false); /*left wall solid, narrow gap*/
  if(r->y > 0)                                           /* not against top edge */
    for(x1 = r->x; x1 < (Int) (r->x + r->m_nWidth); x1++)              /* is there a hole above rect? */
      if(m_pGrid->empty((UInt) x1, r->y - 1)) return(true);    /* yes, no placement allowed */
  if(r->y + r->m_nHeight < m_Box.m_nHeight)                              /* not against bottom edge */
    for(x1 = r->x; x1 < (Int) (r->x + r->m_nWidth); x1++)              /* is there a hole below rect? */
      if(m_pGrid->empty((UInt) x1, r->y + r->m_nHeight)) return(true);    /* yes, no placement allowed */
  return(false);
}

void SquarePacker::initRMinDims() {
  m_sRMinDims.clear();
  for(RectPtrArray::iterator i = m_vRectPtrs.begin();
      i != m_vRectPtrs.end(); ++i)
    if((*i)->m_nMinDim > 1)
      m_sRMinDims.insert(*i);
}

void SquarePacker::initialize(const BoundingBoxes* pBoxes) {
  IntPack::initialize(pBoxes);
}

void SquarePacker::placeUnitRectangles() {
  for(RectPtrArray::iterator i = m_iFirstUnit;
      i != m_vRectPtrs.end(); ++i) {
    Rectangle* r(*i);
    bool bPlaced(false);
    for(r->y = 0; r->y <= (m_Box.m_nHeight - r->m_nHeight); ++r->y) {
      for(r->x = 0; r->x <= (m_Box.m_nWidth - r->m_nWidth); ++r->x)
	if(!r->skippingX(m_pGrid)) {
	  m_Nodes.tick(XF);
	  m_pGrid->addSimple(r);
	  bPlaced = true;
	  break;
	}
      if(bPlaced) break;
    }
  }
}
