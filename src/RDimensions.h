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

#ifndef RDIMENSIONS_H
#define RDIMENSIONS_H

#include "Integer.h"
#include "Rational.h"
#include <iostream>

class BoxDimensions;
class Component;
class Dimensions;
class DimsFunctor;
class Rectangle;
class RDimensions;

class RDimensions {
  friend std::ostream& operator<<(std::ostream& os, const RDimensions& d);

 public:
  RDimensions();
  RDimensions(const Rectangle& r);
  RDimensions(const Dimensions& d);
  RDimensions(const RDimensions& d);
  RDimensions(const BoxDimensions& b);
  RDimensions(URational nWidth, URational nHeight);
  RDimensions(const Component* c);
  ~RDimensions();
  void initMax();
  void initMin();
  void print(std::ostream& os) const;
  void print() const;
  bool canFit(const URational& nWidth,
	      const URational& nHeight) const;
  bool square() const;
  bool unit() const;
  const RDimensions& operator=(const RDimensions& d);
  const RDimensions& operator*=(const URational& n);
  const RDimensions& operator/=(const URational& n);
  bool operator<(const RDimensions& d) const;
  bool operator>(const RDimensions& d) const;
  bool operator==(const RDimensions& d) const;
  bool operator!=(const RDimensions& d) const;
  bool operator==(const Dimensions& d) const;
  bool operator!=(const Dimensions& d) const;

  /**
   * Determines whether or not this rectangle can be congruent to the
   * given rectangle. It will infer which rectangles can be rotated
   * for purposes of this congruency.
   *
   * @param d the other rectangle description to compare against.
   *
   * @param bUnoriented the global flag that specified whether or not
   * the instance should be assumed to be unoriented.
   */

  bool canEqual(const RDimensions& d, bool bUnoriented) const;

  /**
   * @param bUnoriented the global flag that specified whether or not
   * the instance should be assumed to be unoriented.
   */

  bool rotatable(bool bUnoriented) const;
  void rotate();
  void relax();
  void setArea();
  URational ratiowh() const;
  URational ratiohw() const;
  const URational& maxDim() const;
  
  /**
   * Given a constraint on the first dimension, and a rectangle to
   * compare to, report the minimum height of the other dimension. If
   * they can be stacked side by side along the first dimension then
   * we report the maximum of the other dimension. Otherwise, we
   * report the sum of the second dimension.
   *
   * @param nMax the maximum size of the first dimension.
   *
   * @param r the other rectangle to compare with. We assume fixed
   * orientations.
   *
   * @param pDims1 the dimensions functor to use on the current class
   * object.
   *
   * @param pDims2 the dimensions functor to use on the given
   * rectangle.
   *
   * @return a minimum height.
   */

  URational minDim(const URational& nMax, const RDimensions& r,
		   const DimsFunctor* pDims1,
		   const DimsFunctor* pDims2) const;

  typedef enum {
    UNSPECIFIED,
    ORIENTED,
    UNORIENTED
  } Orientation;

  Orientation m_nOrientation;
  URational m_nWidth;
  URational m_nHeight;
  URational m_nMinDim;
  URational m_nArea;
};

std::ostream& operator<<(std::ostream& os, const RDimensions& d);

#endif // RDIMENSIONS_H
