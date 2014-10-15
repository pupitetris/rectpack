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

#ifndef DOMAINS_H
#define DOMAINS_H

#include "Compulsory.h"
#include <vector>

/**
 * Represents the set of interval value assignments possible for a
 * given rectangle, given orientation, and given conditions of which
 * dominated positions are unavailable.
 */

typedef std::vector<Compulsory> DomConfig;

/**
 * Represents the different conditions of which dominated positions
 * are unavailable (there are four total). This data structure
 * corresponds to a single oriented rectangle.
 */

typedef std::vector<DomConfig> Oriented;

/**
 * Corresponds to a single rectangle.
 */

typedef std::vector<Oriented> Domain;

class Packer;
class Parameters;
class BoxDimensions;

/**
 * Represents the domains of interval assignments over all rectangles,
 * for all orientations, for all combinations of dominated positions
 * being unavailable.
 */

class Domains : public std::vector<Domain> {
 public:
  Domains();
  virtual ~Domains();

  /**
   * Resizes all of our data structures to allocate memory. The only
   * thing we can't fully allocate is innermost sizes which describes
   * the different interval values.
   */

  void initialize(const Parameters* pParams);
  void initialize(Packer* pPacker);
  void initialize(const BoxDimensions* pBox);
  const Oriented& get(const Rectangle* r) const;
  void print() const;
  void initialize(const Rectangle* r, const BoxDimensions* pBox,
		  DomConfig& v);
  void initialize(const Rectangle* r, const UInt& nWidth,
		  DomConfig& v);
  UInt intervalSize(const Rectangle* r, const UInt& nWidth);

 private:
  Packer* m_pPacker;
  bool m_bBreakTopSymmetry;
};

#endif // DOMAINS_H
