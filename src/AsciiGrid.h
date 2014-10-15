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

#ifndef ASCIIGRID_H
#define ASCIIGRID_H

#include "Integer.h"
#include <string>
#include <vector>

class Label;
class Placement;
class Placements;
class Parameters;
class RatDims;

class AsciiGrid : public std::vector<std::vector<unsigned char> > {
 public:
  AsciiGrid();
  ~AsciiGrid();
  void draw(const Parameters* pParams, const Placements& v);
  void print() const;

 private:
  void clear();
  void resize(const RatDims& b);
  void draw(const Placement* p, const Label* l);
  void draw(const Placement* p);
  void drawH(UInt y, UInt i, UInt j);
  void drawV(UInt x, UInt i, UInt j);
  void draw(UInt x, UInt y, const std::string& s);
  void clearH(UInt y, UInt i, UInt j);
  void drawCharacter(UInt x, UInt y, unsigned char c);
  void getScreen(UInt& nWidth, UInt& nHeight) const;
};

#endif // ASCIIGRID_H
