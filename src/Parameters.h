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

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "BoxSpec.h"
#include "Dimensions.h"
#include "Fixed.h"
#include "Instance.h"
#include "Integer.h"
#include "LogState.h"
#include "Rational.h"
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <deque>
#include <string>
#include <vector>

/**
 * This class represents the set of parameters that are parsed in from
 * the command line.
 */

class Parameters;

class Parameters {
 public:
  Parameters();
  Parameters(const Parameters& p);
  virtual ~Parameters();
  int parseCommand(int argc, char** argv);
  int parseCommand(std::vector<std::string>& vc);
  int parseIJCAI(int argc, char** argv);
  void saveCommandLine(int argc, char** argv);
  void rescaleInstance();
  UInt scale(const URational& ur) const;
  UInt scale2(const URational& ur) const;
  URational unscale(const UInt& ui) const;
  URational unscale2(const UInt& ui) const;
  URational unscale2(const URational& ur) const;

  /**
   * Random seed value.
   */

  int m_nRandom;

  /**
   * Limits the testing to the [nth, mth] (index starts at 0)
   * boxes. Furthermore, for each of these boxes, we only test the box
   * if the dimensions fit within the given box bounds. Naturally
   * these variables are initialized to allow all boxes to be
   * tested. In the case of an anytime algorithm, the box filter
   * specifies the initial bounding box and the [nth, mth] box ranges
   * will be ignored.
   */

  void parseBox(const std::string& s);
  void initBox();
  bool breakVerticalSymmetry() const;
  void getDescription(boost::program_options::options_description& desc);
  void parseAux(boost::program_options::options_description& desc,
		boost::program_options::variables_map& vm);
  const Parameters& operator=(const Parameters& p);
  std::string m_sBox;
  BoxSpec m_Box;
  size_t m_nNthBox;
  size_t m_nMthBox;
  bool m_bSingleRun;

  /**
   * Algorithm description.
   */

  bool m_bKorf;
  bool m_bScheduling;
  int m_nWeakening;
  int m_nThreads;
  std::string m_sCra;

  /**
   * Attempts to resume a previous run.
   */

  void parseExtend(const std::string& s);
  std::string m_sExtend;
  LogState m_Log;

  /**
   * Parameters related to regression testing.
   */

  void parseRegression(const std::string& s);
  void rerollRegression();
  std::string m_sRegression;
  size_t m_nTrials;
  size_t m_nSubsetSize;
  size_t m_nRangeSize;

  /**
   * The set of rectangles that will be packed. This is the exact
   * specification of width and heights.
   */

  Instance m_vInstance;
  Instance m_vUnscaled;

  /**
   * Whether or not we will attempt to scale the result to fit the
   * screen.
   */

  bool m_bFitScreen;

  /**
   * Represents the set of items that are fixed.
   */

  void parseFix(const std::string& s);
  std::string m_sFix;
  std::deque<Fixed> m_vFixed;

  /**
   * Additional parameters.
   */

  std::string m_sCommand;
  bool m_bVerbose;
  int m_nOrdering;
  std::string m_s0;
  std::string m_s1;
  std::string m_s2;
  int m_nArgc;
  char** m_pArgv;

  /**
   * Option added to facilitate generation of instances based on our
   * formula: W=i, H=j(N+1) + (1-2j)i.
   */

  float m_nJValue;

  /**
   * Specifies the initial window's geometry in pixels.
   */

  void parseGeometry(const std::string& s);
  std::string m_sGeometry;
  size_t m_nGeometryWidth;
  size_t m_nGeometryHeight;

  /**
   * Specifies the size of the interval in terms of a multiplicative
   * factor of the rectangle height: 0.0 for single values, or 1.0 for
   * the height of the entire rectangle (not useful since it doesn't
   * draw any constraints, but I'm just giving you an example).
   */

  std::string m_sYScale;
  URational m_nYScale;
  bool m_bQuiet;

  /**
   * Printing functions.
   */

  void print() const;

 private:

  /**
   * Moves unit rectangles to the end of the sequence. This function
   * modifies both the scaled and unscaled instances in tandem.
   */

  void moveUnitRectangles();
}; // class Parameters

#endif // PARAMETERS_H
