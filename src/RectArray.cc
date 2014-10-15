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

#include "BoxDimensions.h"
#include "DimsFunctor.h"
#include "HeapBox.h"
#include "Instance.h"
#include "Parameters.h"
#include "RectArray.h"
#include <iomanip>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

RectArray::RectArray() {
}

RectArray::~RectArray() {
}

void RectArray::loady(const std::vector<int>& v) {
  for(size_t i = 0; i < std::min(v.size(), size()); ++i)
    operator[](i).y = v[i];
}

void RectArray::savey(std::vector<int>& v) const {
  v.clear();
  for(const_iterator i = begin(); i != end(); ++i)
    v.push_back(i->y);
}

void RectArray::str2vec(const std::string& s, std::vector<int>& v) const {
  v.clear();
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
  boost::char_separator<char> sep(", ");
  tokenizer tokens(s, sep);
  for(tokenizer::iterator i = tokens.begin(); i != tokens.end(); ++i)
    v.push_back(boost::lexical_cast<int>(*i));
}

void RectArray::vec2str(const std::vector<int>& v, std::string& s) const {
  std::ostringstream os;
  if(!v.empty())
    os << v.front();
  for(size_t i = 1; i < v.size(); ++i)
    os << ',' << v[i];
  s = os.str();
}

void RectArray::color() {
  std::vector<float> vColors(size(), 0);
  for(size_t i = 0; i < size(); ++i)
    vColors[i] = i / (float) size() * 360;
  std::random_shuffle(vColors.begin(), vColors.end());
  for(size_t i = 0; i < size(); ++i)
    operator[](i).m_Color.set(vColors[i]);
}

void RectArray::initialize(const Parameters* pParams) {
  reset(pParams->m_vInstance);
  inferProperties();
  for(size_t i = 0; i < pParams->m_vInstance.size(); ++i) {
    if(pParams->m_nYScale >= (URational) 0) {
      operator[](i).m_nScale = pParams->m_nYScale;
      operator[](i).m_nRScale = pParams->m_nYScale;
    }
  }
  color();

  /**
   * Copy over constraints on the square placements (if any).
   */

  for(std::deque<Fixed>::const_iterator i = pParams->m_vFixed.begin();
      i != pParams->m_vFixed.end(); ++i)
    for(iterator j = begin(); j != end(); ++j)
      if(!j->fixed() &&
	 j->m_nWidth == i->m_nWidth &&
	 j->m_nHeight == i->m_nHeight) {
	j->fix(*i);
	break;
      }
}

void RectArray::reset(const Instance& i) {
  resize(i.size());
  for(UInt j = 0; j < size(); ++j) {
    operator[](j).m_nID = j;
    operator[](j).m_bRotatable = i[j].rotatable(i.m_bUnoriented);
    operator[](j).initialize(i[j]);
  }
}

void RectArray::constrain(const HeapBox& b) {
  constrain(b.m_Box);

  /**
   * The HeapBox contains a mutex set which constrains the set of
   * rectangles. This box would not be possible unless the rectangles
   * were oriented in a particular way to achieve the necessary widths
   * and heights. Therefore, we must enforce any constraints requested
   * by the mutex set onto our set of rectangles.
   */

  for(SubsetSums::data_type::const_iterator i = b.m_Rects.begin();
      i != b.m_Rects.end(); ++i) {

    /**
     * Rotate the rectangle to the proper orientation.
     */
    
    Rectangle* pRect = &operator[](i->first->m_nID);
    if(b.m_bRotated) {
      if(i->second == 0 && pRect->m_bRotatable && !pRect->m_bRotated)
	pRect->rotate();
      else if(i->second == 1 && pRect->m_bRotatable && pRect->m_bRotated)
	pRect->rotate();
    }
    else {
      if(i->second == 0 && pRect->m_bRotatable && pRect->m_bRotated)
	pRect->rotate();
      else if(i->second == 1 && pRect->m_bRotatable && !pRect->m_bRotated)
	pRect->rotate();
    }

    /**
     * Then fix that orientation.
     */

    if(i->second < 2)
      pRect->m_bRotatable = false;
  }
}

void RectArray::constrain(const BoxDimensions& b) {
  for(iterator i = begin(); i != end(); ++i)
    i->constrain(b);
  if(b.square())
    front().m_bRotatable = false;
}

void RectArray::constrainHeight(size_t nHeight) {
  for(iterator i = begin(); i != end(); ++i)
    i->constrainHeight(nHeight);
}

void RectArray::inferProperties() {
  if(empty()) return;

  m_bNonRotatables = m_bRotatables = false;
  m_bUnorientedNonSquares = m_bOrientedNonSquares = false;
  for(iterator i = begin(); i != end(); ++i) {
    if(i->m_bRotatable)  {
      m_bRotatables = true;
      if(!i->m_bSquare) m_bUnorientedNonSquares = true;
    }
    else {
      m_bNonRotatables = true;
      if(!i->m_bSquare) m_bOrientedNonSquares = true;
    }
  }
}

const RectArray& RectArray::operator*=(const URational& ur) {
  for(iterator i = begin(); i != end(); ++i)
    i->rescale(ur);
  return(*this);
}

void RectArray::print() const {
  for(const_iterator i = begin(); i != end(); ++i)
    std::cout << std::setw(2) << i->m_nID << ": "
	      << i->width() << "x" << i->height() << " @ "
	      << "(" << i->x << ", " << i->y << ")" << std::endl;
}

void RectArray::rotate() {
  for(iterator i = begin(); i != end(); ++i)
    if(!i->m_bSquare)
      i->rotate();
}

void RectArray::swap() {
  for(iterator i = begin(); i != end(); ++i)
    if(!i->m_bSquare)
      i->swap();
}
