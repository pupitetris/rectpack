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

#ifndef METACSP_H
#define METACSP_H

#include <map>
#include <set>
#include <vector>
#include "HeightFirst.h"
#include "MetaDomain.h"
#include "MetaFrame.h"
#include "MetaVarDesc.h"
#include "Packer.h"
#include "Placements.h"
#include "Solution.h"
#include "TimeSpec.h"
#include "WidthFirst.h"

class Rectangle;

class MetaCSP : public Packer {
 public:
  MetaCSP();
  virtual ~MetaCSP();
  virtual bool pack();

 private:

  class Comparator : public std::less<std::pair<int, Int> > {
  public:
    bool operator()(const std::pair<int, Int>& n1,
		    const std::pair<int, Int>& n2) const {
      if(n1.first == n2.first)
	return(false);

      bool bVertical1 = 
	(n1.first == MetaDomain::ABOVE ||
	 n1.first == MetaDomain::BELOW);
      bool bVertical2 =
	(n2.first == MetaDomain::ABOVE ||
	 n2.first == MetaDomain::BELOW);

      /**
       * If one is vertical and one is horizontal, then we don't need
       * to evaluate them -- simply return the disjunct we prefer.
       */

      if(bVertical1 != bVertical2)
	if(m_bPreferVertical)
	  return(bVertical1);
	else
	  return(!bVertical1);
      else {

	/**
	 * They are the same type of disjunct but they are different
	 * disjuncts. We now base our evaluation on their slack.
	 */
	
	return(n1.second < n2.second);
      }
    };

    bool m_bPreferVertical;
  };

  TimeSpec& timeDomination();
  void placeUnitRectangles();
  void initVariableDescriptions();
  virtual void packAux();
  virtual void orientation();
  virtual void orientationAux(size_t nRectangle);

  /**
   * Extracts the x- and y-coordinates of the rectangles from the APSP
   * matrix.
   */

  virtual void get(Placements& v) const;
  void get2(Placements& v) const;
  bool allAssigned() const;

  /**
   * Prints to the screen the most recently acquired best solution.
   */
  
  void printRecentBest() const;
  void branchOn(MetaFrame::VarIter& i);

  /**
   * If the instance were symmetric, one quick way to break the
   * symmetry is to automatically assign a relationship between the
   * top two rectangles.
   */

  void breakInstanceSymmetry();

  /**
   * If we're about to branch on the horizontal disjunct assignment,
   * and in our stack we've so far only made a bunch of vertical
   * disjunct assignments, we do not have to explore the complementary
   * horizontal disjunct (because its symmetric). The same is true
   * about a series of horizontal disjunct assignments and our first
   * attempt at branching on a vertical disjunct. The boolean flag
   * tells us whether or not we should continue to consider this
   * symmetry breaking opportunity.
   */

  void dynamicSymmetry(MetaFrame::VarIter& v);

  /**
   * @TODO This function can be made more efficient by precomputing
   * the static variable order, and simply looking up the appropriate
   * next variable.
   */
  
  MetaFrame::VarIter nextVariable();

  /**
   * Performs variable ordering on the given variable's domain. We
   * first decide whether we want to pick from between the vertical or
   * horizontal disjuncts. Then for the two remaining values, we can
   * assign additional tie-breaking scores.
   */

  void valueOrdering(MetaFrame::VarIter& i, std::vector<int>& v) const;
  bool computeAPSP();
  void semanticBranching(MetaFrame::VarIter& v, int n);

  /**
   * Performs clique detection to derive a minimum width.
   */

  UInt cliqueWidth() const;
  UInt cliqueWidthAux(const std::vector<WidthFirst>& v) const;

  /**
   * Performs clique detection to derive a minimum height.
   */

  UInt cliqueHeight() const;
  UInt cliqueHeightAux(const std::vector<HeightFirst>& v) const;

  /**
   * Returns whether or not the given two rectangles are forced to be
   * horizontal to one another. Note that the negative doesn't imply
   * that they are vertically non-overlapping.
   */

  bool horizontal(const Rectangle* r1, const Rectangle* r2) const;

  /**
   * Returns whether or not the given two rectangles are forced to be
   * vertical to one another. Note that the negative doesn't imply
   * that they are horizontally non-overlapping.
   */

  bool vertical(const Rectangle* r1, const Rectangle* r2) const;

  /**
   * Retrieves the previously created meta-variable description based
   * on the pointers to two rectangle objects.
   */

  const MetaVarDesc* getDesc(const Rectangle* r1,
			     const Rectangle* r2) const;
  bool updateSolution();

  std::vector<MetaFrame> m_vStack;

  /**
   * The current global set of variable descriptions. This is
   * allocated only once and never changed throughout the entire
   * search. The major index is the first rectangle ID and the minor
   * index is the second rectangle ID. Although we have n^2 entries in
   * this table, we only use a half matrix where a rectangle with
   * index i is less than j.
   */

  std::vector<std::vector<MetaVarDesc> > m_vVariableDescs;

 public:
  Solution m_Best;

  /**
   * Tracks the number of times that the best solution has been
   * replaced. This is distinct from the number of times that we've
   * updated the solution (with possibly an additional bounding box of
   * equal area but of different dimensions).
   */

 private:
  UInt m_nBestReplacements;
  TimeSpec m_nTimeDomination;
};

#endif // METACSP_H
