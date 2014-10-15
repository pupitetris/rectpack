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

#ifndef METAFRAME_H
#define METAFRAME_H

#include <map>
#include <vector>
#include "APSP.h"
#include "MetaDomain.h"
#include "MetaVarDesc.h"
#include "MetaVariable.h"
#include "Solution.h"

class MetaFrame;

class MetaFrame {
 public:
  typedef std::map<const MetaVarDesc*, MetaVariable>::iterator VarIter;
  typedef std::map<const MetaVarDesc*, MetaVariable>::const_iterator ConstVarIter;

  MetaFrame();
  MetaFrame(const MetaFrame& src);
  virtual ~MetaFrame();
  void initialize(const std::vector<std::vector<MetaVarDesc> >& vVars);

  /**
   * Makes the requested assignment, moving the appropriate meta
   * variable into its set.
   */

  MetaVariable* assign(const MetaVarDesc* pDesc,
		       MetaDomain::ValueT nValue);

  /**
   * Performs forward checking on all unassigned variables, removing
   * invalid domain values.
   *
   * @return true if all tests passed and false if we found an empty
   * domain after all.
   */

  bool forwardChecking();
  bool forwardCheckBounds(const Solution& s);

  /**
   * Checks the APSP matrices for consistency with the current best
   * solution. Check to see if the APSP matrix requires a bounding box
   * of greater area than our best solution so far.
   */

  bool checkBoxBounds(const Solution& s) const;

  /**
   * Increases the minimum width and height, if necessary, to the
   * given tighter values, and given a maximum area.
   *
   * @return true if bounds are still valid, or false if the bounds
   * imply no solutions.
   */

  bool updateMinimumBounds(UInt nWidth, UInt nHeight, UInt nArea);

  /**
   * Make all assignments regarding subsumed variables. We go through
   * all unassigned variables and check to see if non-overlap is
   * already required per the APSP matrices. If so, we make immediate
   * assignments. This captures transitive assignments.
   */

  void subsumeVariables();

  /**
   * Provided for debugging purposes.
   */

  void print() const;

  /**
   * Represents the set of variables which have been assigned
   * particular values.
   */

  std::map<const MetaVarDesc*, MetaVariable> m_Assigned;

  /**
   * Represents the set of unassigned variables.
   */

  std::map<const MetaVarDesc*, MetaVariable> m_Unassigned;
  APSP m_vXMatrix;
  APSP m_vYMatrix;

  UInt m_nMinWidth;
  UInt m_nMinHeight;
  UInt m_nMaxWidth;
  UInt m_nMaxHeight;
};

#endif // METAFRAME_H
