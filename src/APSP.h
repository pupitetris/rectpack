/**
 * This file is part of rectpack.
 *
 * rectpack is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 **/

#ifndef APSP_H
#define APSP_H

#include <iostream>
#include <vector>
#include "Integer.h"
#include "MetaDomain.h"

class APSP;
class MetaVarDesc;
class Rectangle;

class APSP : public std::vector<std::vector<Int> >{
  friend std::ostream& operator<<(std::ostream& os, const APSP& rhs);

 public:
  APSP();
  APSP(const APSP& src);
  virtual ~APSP();
  void initialize(size_t nRectangles);
  void assign(const MetaVarDesc* pDesc, int nValue);
  void negate(const MetaVarDesc* pDesc, int nValue);
  Int minWidth(const MetaVarDesc* pDesc) const;
  Int minHeight(const MetaVarDesc* pDesc) const;

  /**
   * Takes the current matrix and computes all pairs shortest paths in
   * n^3 time.
   */

  void floydWarshall();

  /**
   * Updated weights going to and from k, so we will take n^2 time to
   * update any paths necessary.
   */

  void floydWarshall(size_t k);

  /**
   * Checks the diagonal of the matrix to see if there exists any
   * negative cycles.
   */

  bool negativeCycles() const;
  void update(const Rectangle* r1, const Rectangle* r2, Int n);
  size_t width(const Int& n) const;
  void columnWidths(std::vector<size_t>& v) const;
  void print() const;
};

std::ostream& operator<<(std::ostream& os, const APSP& rhs);

#endif // APSP_H
