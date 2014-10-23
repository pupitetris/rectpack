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

#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "Color.h"
#include "Integer.h"
#include "Interval.h"
#include "Rational.h"

#define NUM_ROTATIONS 6 // Number of possible orientations of an orthogonal figure of N dims is N!

class BoxDimensions;
class Coordinates;
class DimsFunctor;
class RDimensions;
class Fixed;
class Rectangle;
class Grid;

class Rectangle {
  friend std::ostream& operator<<(std::ostream& os,
				  const Rectangle& r);

 public:
  Rectangle();
  virtual ~Rectangle();
  const Rectangle& operator=(const Rectangle& src);
  void initialize(const UInt& nSize);
  void initialize(const UInt& nWidth,
		  const UInt& nHeight,
		  const UInt& nLength);
  void initialize(const RDimensions& d);

  /**
   * Reports whether or not the two rectangles are equivalent,
   * assuming that orientation is important.
   */

  bool equal(const Rectangle& r) const;
  bool equal(const Rectangle* r) const;

  /**
   * Reports whether or not the two rectangles are equivalent,
   * assuming that the rectangles are unoriented.
   */

  bool uequal(const Rectangle& r) const;
  bool uequal(const Rectangle* r) const;
  bool rotatable() const;
  void rotate();
  void rotateTall();
  void rotateWide();
  void rotateLong();
  void resetRotation();
  bool overlaps(const Rectangle* r) const;

  bool fixed() const;
  void fix(const Coordinates& c);
  void fix(const Fixed& f);

  void setColor(float h);
  int nextX() const;
  int nextY() const;
  int nextZ() const;
  bool skippingX(const Grid* pGrid);
  bool skippingY(const Grid* pGrid);
  bool skippingZ(const Grid* pGrid);
  void print() const;
  void printNoLocation() const;
  void printTop() const;
  void constrain(const BoxDimensions& b);
  void constrainHeight(size_t nHeight);
  void constrainLength(size_t nLength);
  void gutsyScale(const BoxDimensions& b);
  void rescale(const URational& ur);
  void unscale(const URational& ur);
  bool unit() const;
  const URational& scale() const;
  URational& scale();
  
  /**
   * Given a fixed dimension, a dimension functor, and another
   * rectangle, assume the fixed dimension and compute a minimum bound
   * on the other dimension.
   *
   * @param nMax a constraint forced upon the first dimension.
   *
   * @param pDims a dimensional functor that binds a dimension to the
   * first one as well as to the second one.
   *
   * @param pRect the rectangle to compare against.
   *
   * @return a lower bound on the second dimension that is induced,
   * including consideration of all rotations of the other rectangle.
   */

  // Port where the "other" dimension is represented by both 2 and 3
  UInt minDim23(const UInt& nMax,
		const DimsFunctor* pDims,
		const Rectangle* pRect) const;

  // Port where we define which dimension is going to be the "other" (either 2 or 3)
  UInt minDim2(const UInt& nMax,
	       const DimsFunctor* pDims,
	       const Rectangle* pRect) const;

  UInt minDim3(const UInt& nMax,
	       const DimsFunctor* pDims,
	       const Rectangle* pRect) const;

  /**
   * Represents a unique ID for referring to this square. The space of
   * these ID numbers can be assumed to start at 0 and enumerate all
   * squares.
   */

  UInt m_nID;

  /**
   * The x-coordinate of this square.
   */

  UInt x;
  Interval xi;

  /**
   * The y-coordinate of this square.
   */

  UInt y;
  Interval yi;

  /**
   * The z-coordinate of this square.
   */

  UInt z;
  Interval zi;

  UInt& width();
  const UInt& width() const;
  UInt& height();
  const UInt& height() const;
  UInt& length();
  const UInt& length() const;

  UInt m_nWidth;
  UInt m_nHeight;
  UInt m_nLength;
  UInt m_nMinDim;
  UInt m_nMaxDim;
  UInt m_nArea;

  /**
   * Color assigned for this square for purposes of visualization.
   */
  
  Color m_Color;

  /**
   * Notes whether or not this rectangle has been rotated from the
   * original specifications.
   */

  bool m_bRotated;
  bool m_bRotatable;
  bool m_bSquare;

  DimsFunctor *m_pRotation; // Represents the current rotation of the rectangle.

  /**
   * Set to non-negative if we're going to be fixing the placement of
   * this square on our grid.
   */

  bool m_bFixed;
  UInt m_nFixX;
  UInt m_nFixY;
  UInt m_nFixZ;

  URational m_nScale;
  URational m_nRScale;
};

std::ostream& operator<<(std::ostream& os, const Rectangle& r);

#endif // RECTANGLE_H
