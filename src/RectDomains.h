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

#ifndef RECTDOMAINS_H
#define RECTDOMAINS_H

#include "Interval.h"
#include <vector>

class BranchingFactor;
class CoordinateRanges;
class Parameters;
class IntPack;

typedef std::vector<Interval> IntDomains;
typedef std::vector<IntDomains> IntConfig;
typedef std::vector<IntConfig> RectDomain;

class RectDomains : public std::vector<RectDomain> {
 public:
  RectDomains();
  ~RectDomains();
  void initialize(const Parameters* pParams);
  void initialize(IntPack* pPacker, const CoordinateRanges* pRanges,
		  const BranchingFactor* pBranches);
  std::vector<Interval>& get(const Rectangle* r, size_t nConfig = 0);

 private:
  void initialize(IntPack* pPacker, Rectangle* r,
		  const CoordinateRanges* pRanges,
		  const BranchingFactor* pBranches, IntConfig* ic);
  void print() const;
  void print(const IntConfig& ic) const;
  void print(const IntDomains& id) const;
  const IntPack* m_pPacker;
};

#endif // RECTDOMAINS_H
