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

#ifndef GRID_H
#define GRID_H

#include <vector>
#include "BoxDimensions.h"
#include "Coordinates.h"
#include "Dimensions.h"
#include "ICoords.h"
#include "Integer.h"
#include "Packer.h"
#include "Component.h"

#define GRIDEMPTY std::numeric_limits<UInt>::max()

class Interval;
class Parameters;
class Packer;

class Grid : public std::vector<std::vector<UInt> > {
 public:
  Grid(const Packer* pPacker);
  Grid();
  virtual ~Grid();
  void initialize(const Parameters* pParams);
  void initialize(const BoxDimensions* pBox);
  void set(const BoxDimensions& b);
  void resize(Int nDim);

  /**
   * Basic drawing function that doesn't update the vertical or
   * horizontal grids.
   */

  void draw(Int x, Int dx, Int y, Int dy, UInt id) {
    for(Int i = x; i < x + dx; ++i)
      std::fill(operator[](i).begin() + y,
		operator[](i).begin() + y + dy, id);
  };

  /**
   * Places the square in the location on the grid, updates the
   * horizontal and vertical grids, and also modifies the heuristic
   * compilation according to changes in accommodation capacity of the
   * cells.
   *
   * @param s the square to place on the grid. It is assumed that the
   * location of the square is contained within the class object.
   *
   * @param v the histogram which indicates how many cells of each
   * accommodating size there are.
   */

  void add(const Rectangle* s, std::vector<Int>& v);

  /**
   * Places the square in the location on the grid and updates the
   * horizontal and vertical grids.
   *
   * @param s the square to place on the grid. It is assumed that the
   * location of the square is contained within the class object.
   */

  void add(const Rectangle* s);
  void add(Component* c);
  void add(Component* c, std::vector<Int>& v);
  void addSimple(const Rectangle* r) {
    draw(r->x, r->m_nWidth, r->y, r->m_nHeight, r->m_nID);
  };

 public:
  void del(const Rectangle* s);
  void del(Component* c);
  bool empty(const ICoords& c) const {
    return(operator[](c.x).operator[](c.y) == GRIDEMPTY);
  }

  bool empty(Int x, Int y) const {
    return(operator[](x).operator[](y) == GRIDEMPTY);
  }

  bool empty(UInt x, UInt y) const {
    return(operator[](x).operator[](y) == GRIDEMPTY);
  }

  bool empty(Int x, UInt y) const {
    return(operator[](x).operator[](y) == GRIDEMPTY);
  }

  bool empty(UInt x, Int y) const {
    return(operator[](x).operator[](y) == GRIDEMPTY);
  }

  bool empty(Int x, Int xWidth, Int y, Int yWidth) {
    return(empty(x, y) &&
	   empty(x, y + yWidth - 1) &&
	   empty(x + xWidth - 1, y) &&
	   empty(x + xWidth - 1, y + yWidth - 1));
  };

  bool empty(const Rectangle* r) {
    return(empty(r->x, r->m_nWidth, r->y, r->m_nHeight));
  }

  bool emptyxi(const Rectangle* r) {
    return(empty(r->xi.m_nEnd, r->xi.m_nWidth, r->y, r->m_nHeight));
  };

  bool emptyyi(const Rectangle* r) {
    return(empty(r->x, r->m_nWidth, r->yi.m_nEnd, r->yi.m_nWidth));
  }

  UInt get(const ICoords& c) const {
    return(operator[](c.x)[c.y]);
  };
  
  UInt get(Int x, Int y) const {
    return(operator[](x)[y]);
  };

  UInt& get(Int x, Int y) {
    return(operator[](x)[y]);
  };

  UInt get(UInt x, UInt y) const {
    return(operator[](x)[y]);
  };

  UInt& get(UInt x, UInt y) {
    return(operator[](x)[y]);
  };

  const Rectangle* rect(Int x, Int y) const {
    return(m_pPacker->m_vRectPtrs[get(x, y)]);
  }

  UInt width(Int x, Int y) const {
    return(rect(x, y)->m_nWidth);
  }

  UInt height(Int x, Int y) const {
    return(rect(x, y)->m_nHeight);
  }

  bool occupied(Int x, Int y) const {
    return(operator[](x).operator[](y) != GRIDEMPTY);
  };

  /**
   * Checks the outline of the rectangle to see if it is occupied.
   */

  bool occupied(UInt nx, UInt ny, UInt w, UInt h) const {
    for(UInt x = nx; x < nx + w; ++x)
      if(occupied(x, ny) || occupied(x, ny + h - 1))
	return(true);
    for(UInt y = ny + 1; y < ny + h - 1; ++y)
      if(occupied(nx, y) || occupied(nx + w - 1, y))
	return(true);
    return(false);
  }

  bool occupied(UInt nx, UInt ny, const Dimensions& d) const {
    return(occupied(nx, ny, d.m_nWidth, d.m_nHeight));
  }
  
  bool occupied(const ICoords& c, const Dimensions& d) const {
    return(occupied((UInt) c.x, (UInt) c.y, d));
  }

  bool occupied(const Rectangle* r) const {
    return(occupied(r->x, r->y, r->m_nWidth, r->m_nHeight));
  }

  bool occupied(const Component* c) const {
    return(occupied(c->m_nX, c->m_nY, c->m_Dims));
  }

  bool areaOccupied(UInt nx, UInt ny, UInt w, UInt h) const {
    for(UInt x = nx; x < nx + w; ++x)
      for(UInt y = ny; y < ny + h; ++y)
	if(occupied(x, y)) return(true);
    return(false);
  }

  bool areaOccupied(const Rectangle* r) const {
    return(areaOccupied(r->x, r->y, r->m_nWidth, r->m_nHeight));
  }

  const BoxDimensions& box() const {
    return(m_Box);
  }

  void print() const;
  void printid() const;
  void printh() const;
  void printv() const;
  void printX() const;

  std::vector<std::vector<UInt> > m_vHor;
  std::vector<std::vector<UInt> > m_vVer;

 protected:
  void resize(Int n, const std::vector<UInt>& v);

  const Packer* m_pPacker;
  BoxDimensions m_Box;

 private:
  void printAux(const std::vector<std::vector<UInt> >& v) const;
  void printRaw(const std::vector<std::vector<UInt> >& v) const;
};

#endif // GRID_H
