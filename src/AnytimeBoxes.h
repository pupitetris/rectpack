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

#ifndef ANYTIMEBOXES_H
#define ANYTIMEBOXES_H

#include "BoundingBoxes.h"
#include "NodeCount.h"
#include "Rational.h"
#include "SubsetSums.h"
#include "SubsetSumsSet.h"
#include "TimeSpec.h"
#include <deque>
#include <set>
#include <string>
#include <vector>
#include <queue>

class BoxDimensions;
class Packer;
class Parameters;

class AnytimeBoxes : public BoundingBoxes {
 public:
  AnytimeBoxes();
  virtual ~AnytimeBoxes();
  virtual void initialize(const Parameters* pParams);
  virtual void initialize(Packer* pPacker);
  virtual void initialize(const std::deque<Packer*>& vPackers);
  virtual bool run();
  virtual std::string solutions() const;

 protected:

  /**
   * Performs a single packing attempt on a bounding box as specified
   * in the command line parameters.
   */

  virtual bool singleRun();
};

#endif // ANYTIMEBOXES_H
