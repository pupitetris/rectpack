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

#include "Label.h"
#include "RatDims.h"
#include "RDimensions.h"
#include <sstream>

Label::Label() {
}

Label::~Label() {
}

void Label::initialize(const RDimensions* pActual,
		       const RatDims* pScaled) {
  clear();
  
  /**
   * If there isn't any room for a single character, then we don't
   * have a label.
   */

  if(pScaled->m_nHeight < (URational) 2 ||
     pScaled->m_nWidth < (URational) 2) return;

  /**
   * Parse the width and height.
   */

  std::ostringstream ossw, ossh;
  if((pScaled->m_nWidth > pScaled->m_nHeight &&
      pActual->m_nWidth > pActual->m_nHeight) ||
     (pScaled->m_nWidth < pScaled->m_nHeight &&
      pActual->m_nWidth < pActual->m_nHeight)) {
    ossw << pActual->m_nWidth << std::flush;
    ossh << pActual->m_nHeight << std::flush;
  }
  else {
    ossw << pActual->m_nHeight << std::flush;
    ossh << pActual->m_nWidth << std::flush;
  }
  std::string w(ossw.str());
  std::string h(ossh.str());
  size_t wChars = (pScaled->m_nWidth * (URational) 2).get_ui();

  /**
   * Try WxH and see if that fits.
   */

  if(wChars > (w.size() + 1 + h.size())) {
    resize(1);
    operator[](0) = w + "x" + h;
    center(pScaled);
    return;
  }

  /**
   * Try W \n x \n H to see if it fits.
   */

  if(pScaled->m_nHeight >= (URational) 4 &&
     wChars > w.size() && wChars > h.size()) {
    resize(3);
    operator[](2) = w;
    operator[](1) = "x";
    operator[](0) = h;
    center(pScaled);
    return;
  }

  /**
   * Pick the better of Wx \n H or W \n xH.
   */

  if(pScaled->m_nHeight >= (URational) 3 &&
     (w.size() == h.size() && wChars > (w.size() + 1)) ||
     (w.size() != h.size() && wChars > std::max(w.size(), h.size()))) {
    resize(2);
    if(w.size() < h.size()) {
      operator[](1) = w + "x";
      operator[](0) = h;
    }
    else {
      operator[](1) = w;
      operator[](0) = "x" + h;
    }
    center(pScaled);
    return;
  }
}

size_t Label::width() const {
  if(empty()) return(0);
  else return(front().size());
}

size_t Label::height() const {
  return(size());
}

bool Label::empty() const {
  return(std::vector<std::string>::empty());
}

void Label::center(const RatDims* pScaled) {
  for(iterator i = begin(); i != end(); ++i) {
    int nPad =
      ((pScaled->m_nWidth * (URational) 2 -
	(URational) 1 - (URational) (int) i->size()) /
       (URational) 2).get_ui();
    std::ostringstream oss;
    for(int j = 0; j < nPad; ++j) oss << " ";
    oss << *i << std::flush;
    *i = oss.str();
  }
  m_nYOffset =
    ((pScaled->m_nHeight + (URational) 2 - (URational) (int) size()) /
     (URational) 2).get_ui();
}

size_t Label::yOffset() const {
  return(m_nYOffset);
}
