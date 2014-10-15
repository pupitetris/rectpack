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

#include "AsciiGrid.h"
#include "Label.h"
#include "Parameters.h"
#include "Placements.h"
#include "RatDims.h"
#include "Rectangle.h"
#include "RectPtrArray.h"
#include <stdlib.h>
#include <sstream>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define DEFAULT_SCREEN_WIDTH 50;
#define DEFAULT_SCREEN_HEIGHT 100;

AsciiGrid::AsciiGrid() {
}

AsciiGrid::~AsciiGrid() {
}

void AsciiGrid::resize(const RatDims& r) {
  std::vector<std::vector<unsigned char> >::
    resize(r.m_nWidth.get_ui() * 2 + 1,
	   std::vector<unsigned char>(r.m_nHeight.get_ui() + 1, ':'));
}

void AsciiGrid::clear() {
  std::vector<std::vector<unsigned char> >::clear();
}

void AsciiGrid::draw(const Parameters* pParams, const Placements& v) {

  /**
   * Retrieve some information about the size of the screen.
   */

  UInt nWidth, nHeight;
  getScreen(nWidth, nHeight);

  /**
   * Rescale the current instance so that it's not larger than our
   * width, if applicable.
   */

  Placements vScaled(v);
  if(!pParams->m_bFitScreen)
    vScaled *= v.minLabelScale();

  if(vScaled.m_Box.m_nWidth > (URational) nWidth) {
    vScaled *= ((URational) nWidth / vScaled.m_Box.m_nWidth);
    if(vScaled.m_Box.m_nHeight > (URational) nHeight)
      vScaled *= ((URational) nHeight / vScaled.m_Box.m_nHeight);
  }

  /**
   * Resize the grid.
   */

  clear();
  resize(vScaled.m_Box);

  /**
   * Generate the labels.
   */

  std::vector<Label> vLabels(vScaled.size());
  for(UInt i = 0; i < vLabels.size(); ++i)
    vLabels[i].initialize(&pParams->m_vUnscaled[i],
			  &vScaled[i].m_Dims);

  /**
   * Finally draw the results.
   */
  
  for(UInt i = 0; i < v.size(); ++i)
    draw(&vScaled[i], &vLabels[i]);
}

void AsciiGrid::draw(const Placement* p, const Label* l) {
  draw(p);
  for(UInt i = 0; i < l->size(); ++i)
    draw(p->ix1() * 2 + 1, p->iy1() + l->yOffset() + i,
	 l->operator[](i));
}

void AsciiGrid::draw(const Placement* p) {
  drawH(p->iy1(), p->ix1(), p->ix2());
  for(UInt y = p->iy1() + 1; y < p->iy2(); ++y)
    clearH(y, p->ix1(), p->ix2());
  drawH(p->iy2(), p->ix1(), p->ix2());
  drawV(p->ix1(), p->iy1(), p->iy2());
  drawV(p->ix2(), p->iy1(), p->iy2());
}

void AsciiGrid::draw(UInt x, UInt y, const std::string& s) {
  for(UInt i = 0; i < s.size(); ++i)
    (*this)[x + i][y] = (unsigned char) s[i];
}

void AsciiGrid::drawH(UInt y, UInt i, UInt j) {
  j *= 2;
  i *= 2;
  for(; i <= j; ++i)
    drawCharacter(i, y, '-');
}

void AsciiGrid::clearH(UInt y, UInt i, UInt j) {
  j *= 2;
  i *= 2;
  for(; i <= j; ++i)
    if((*this)[i][y] == ':')
      (*this)[i][y] = ' ';
}

void AsciiGrid::drawV(UInt x, UInt i, UInt j) {
  x *= 2;
  for(; i <= j; ++i)
    drawCharacter(x, i, '|');
}

void AsciiGrid::drawCharacter(UInt x, UInt y, unsigned char c) {
  if(c == '+')
    (*this)[x][y] = c;
  else if(c == '-') {
    if((*this)[x][y] == '+' || (*this)[x][y] == '|')
      (*this)[x][y] = '+';
    else
      (*this)[x][y] = '-';
  }
  else if(c == '|') {
    if((*this)[x][y] == '+' || (*this)[x][y] == '-')
      (*this)[x][y] = '+';
    else
      (*this)[x][y] = '|';
  }
}

void AsciiGrid::print() const {
  if(empty())
    std::cout << "Width exceeds 300." << std::endl;
  else
    for(int y = front().size() - 1; y >= 0; --y) {
      for(UInt x = 0; x < size(); ++x)
	std::cout << (*this)[x][y];
      std::cout << std::endl;
    }
}

void AsciiGrid::getScreen(UInt& nWidth, UInt& nHeight) const {
  nWidth = DEFAULT_SCREEN_WIDTH;
  nHeight = DEFAULT_SCREEN_HEIGHT;
  int w, h;

  struct winsize ts;
  ioctl(STDIN_FILENO, TIOCGWINSZ, &ts);
  w = ts.ws_col;
  h = ts.ws_row;

  if(w > 0 && h > 0) {
    nWidth = (UInt) w / 2;
    nHeight = (UInt) h;
  }
}
