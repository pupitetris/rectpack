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

#ifndef DOMINATION_H
#define DOMINATION_H

#include "Dimensions.h"
#include "Integer.h"
#include "Rectangle.h"
#include "SymKey.h"
#include "TimeSpec.h"

#include <boost/function.hpp>
#include <deque>
#include <list>
#include <map>
#include <set>
#include <vector>

class BoxDimensions;
class DomDecorator;
class PackingCache;
class Parameters;
class Rectangle;
class RectDecArray;
class RectDecDeq;
class RectDecorator;
class RectPtrArray;
class DuplicateIndex;

/**
 * Represents a domination table that is aware of unoriented
 * benchmarks. For the unoriented case it makes use of
 * Rectangle::m_bRotated to determine which table to look up when the
 * width of the current rectangle (as it is oriented). Consequently,
 * call the appropriate width and height reference functions for the
 * current orientation of the rectangle.
 */

class Domination {
 public:
  virtual ~Domination();
  Domination();
  Domination(const Domination& d);
  static Domination* create(const Parameters* pParams,
			    const RectPtrArray* pRects,
			    const DuplicateIndex* pDuplicates,
			    bool bSupportsDomination);
  virtual Domination* clone() const;
  void initialize(const Parameters* pParams,
		  const RectPtrArray* pRects,
		  const DuplicateIndex* pDuplicates);
  bool dominated(const Rectangle* r, UInt nGap) const;
  bool dominatedw(const Rectangle* r, UInt nGap) const;
  bool dominatedh(const Rectangle* r, UInt nGap) const;
  virtual bool dominatedw1(const Rectangle* r, UInt nGap) const;
  virtual bool dominatedw2(const Rectangle* r, UInt nGap) const;
  virtual bool dominatedh1(const Rectangle* r, UInt nGap) const;
  virtual bool dominatedh2(const Rectangle* r, UInt nGap) const;
  UInt entries(const Rectangle* r) const;
  UInt entriesw(const Rectangle* r) const;
  UInt entriesh(const Rectangle* r) const;
  virtual UInt entriesw1(const Rectangle* r) const;
  virtual UInt entriesw2(const Rectangle* r) const;
  virtual UInt entriesh1(const Rectangle* r) const;
  virtual UInt entriesh2(const Rectangle* r) const;
  UInt gap(const Rectangle* r) const;
  UInt gapw(const Rectangle* r) const;
  UInt gaph(const Rectangle* r) const;
  virtual UInt gapw1(const Rectangle* r) const;
  virtual UInt gapw2(const Rectangle* r) const;
  virtual UInt gaph1(const Rectangle* r) const;
  virtual UInt gaph2(const Rectangle* r) const;
  virtual void buildTable();
  virtual void buildTableStage1();

  /**
   * Note that we make reference in this function to the original
   * rectangle pointer array. Therefore, when this is called, we also
   * assume that the rectangles have all been reset and rotated to
   * their original orientations.
   */

  virtual void buildTableStage2();
  bool enabled() const;

  /**
   * Attempts to remove domination values which move the root
   * rectangle into the interior in a way that violates the symmetry
   * breaking rule of confining the top rectangle to the upper left
   * quadrant.
   *
   * @param nMax the maximum coordinate that the top level rectangle
   * is allowed to assume.
   */

  void preserveTopSymmetry(UInt nMax);
  void swap();
  TimeSpec m_Time;

 protected:

  /**
   * Assumes that the first dimension d1 is the wall, and dimension d2
   * is the other side.
   */

  void buildTable(const RectDecArray& rda, PackingCache& c,
		  std::vector<std::vector<UInt> >& vTable);
  void buildGaps(const std::vector<std::vector<UInt> >& vTable,
		 std::vector<UInt>& vGap);
  void buildEntries(std::vector<std::vector<UInt> >& vTable);
  void printTable(const std::vector<std::vector<UInt> >& v) const;
  void printMeta() const;
  bool pack(Parameters* pParams, const BoxDimensions& b);
  UInt totalArea(const RectDecDeq& d) const;
  bool sticksOut(UInt g, UInt l,
		 const RectDecDeq& d) const;
  void deadlocks(const RectDecArray& rda, DomDecorator& t) const;
  bool deadlocks(const RectDecorator* r, const RectDecDeq& d,
		 DomDecorator& t) const;
  void getEligible(const RectDecorator* rd, const RectDecArray& rda,
		   UInt nGap, RectDecDeq& d) const;

  bool dominated(const Rectangle* r, UInt nGap,
		 const std::vector<std::vector<UInt> >& v1,
		 const std::vector<std::vector<UInt> >& v2) const {
    if(r->m_bRotated) {
      if(nGap < v2[r->m_nID].size())
	return(v2[r->m_nID][nGap] == 1);
    }
    else if(nGap < v1[r->m_nID].size())
      return(v1[r->m_nID][nGap] == 1);
    return(false);
  }

  UInt gap(const Rectangle* r,
	   const std::vector<UInt>& v1,
	   const std::vector<UInt>& v2) const {
    if(r->m_bRotated)
      return(v2[r->m_nID]);
    else
      return(v1[r->m_nID]);
  }

  UInt entries(const Rectangle* r,
	       const std::vector<std::vector<UInt> >& v1,
	       const std::vector<std::vector<UInt> >& v2) const {
    if(r->m_bRotated)
      return(v2[r->m_nID].size());
    else
      return(v1[r->m_nID].size());
  }

  void print() const;

  /**
   * These functions handle duplicates. The problem is that when we
   * have duplicates and we enforce the subsequent duplicate to assume
   * a coordinate greater than or equal to the previous duplicate,
   * then sometimes we do not explore the proper dominated
   * positions. Therefore we must eliminate certain location entries
   * (since their respective dominated positions aren't explored).
   */

  typedef std::map<const Rectangle*, std::list<const Rectangle*> > DuplicateMap;
  void constrainDuplicatesW(const DuplicateMap& m);
  void constrainDuplicatesH(const DuplicateMap& m);
  void getDuplicates(DuplicateMap& m) const;


  const Parameters* m_pParams;
  const RectPtrArray* m_pRects;
  const DuplicateIndex* m_pDuplicates;

 private:

  /**
   * The domination tables that are derived directly from the
   * instance. The major index is the rectangle ID while the minor
   * index is the gap size. m_vTableH1 assumes that the rectangle's
   * vertical side is being used as the wall and the other wall is the
   * first box edge. m_vTableH2 assumes the second box edge is a wall.
   */

  std::vector<std::vector<UInt> > m_vTableH1;
  std::vector<std::vector<UInt> > m_vTableH2;

  /**
   * The domination tables that are derived directly from the
   * instance. The major index is the rectangle ID while the minor
   * index is the gap size. m_vTableW1 assumes that the rectangle's
   * horizontal side is being used as the wall, and the other wall is
   * the first box edge. m_vTableW2 assumes the second box edge is a
   * wall.
   */

  std::vector<std::vector<UInt> > m_vTableW1;
  std::vector<std::vector<UInt> > m_vTableW2;
  std::vector<UInt> m_vGapH1;
  std::vector<UInt> m_vGapH2;
  std::vector<UInt> m_vGapW1;
  std::vector<UInt> m_vGapW2;

  /**
   * Set of domination table entries that involve moving the root
   * rectangle into the interior. This tells us which table entries we
   * will have to modify (and unmodify) with each containment
   * attempt. It's sorted by a "min coordinate" defined in SymKey,
   * which is essentially the coordinate that the root rectangle is
   * moved to when the domination condition is applied.
   */

  std::set<SymKey> m_MovesRootY;

 protected:

  /**
   * Specifies whether or not this is usable.
   */

  bool m_bEnabled;
};

#endif // DOMINATION_H
