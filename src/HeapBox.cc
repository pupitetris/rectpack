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

#include "HeapBox.h"

HeapBox::HeapBox() :
  m_bRotated(false),
  m_nConflictLearningIndex(0) {
}

HeapBox::~HeapBox() {
}

void HeapBox::initialize(SubsetSums::const_iterator& w,
			 SubsetSums::const_iterator& h) {
  m_Box.initialize(w->first, h->first);
  m_iHeight = h;
}

void HeapBox::initializeH(SubsetSums::const_iterator& h) {
  m_Box.initializeH(h->first);
  m_iHeight = h;
}

bool HeapBox::intersect(SubsetSums::const_iterator& w) {
  m_Rects.clear();
  for(SubsetSums::data_type::const_iterator i = w->second.begin();
      i != w->second.end(); ++i) {
    SubsetSums::data_type::const_iterator j =
      m_iHeight->second.find(i->first);
    if(j != m_iHeight->second.end()) {

      /**
       * We found the same rectangle in both sets. Now we must
       * intersect their orientations. If we already have one
       * rectangle in this set, then immediately we can fail. We can
       * only allow at most one rectangle in the intersection!
       */
      
      if(!m_Rects.empty())
	return(false);

      if(i->second == 0) {
	if(j->second == 1)
	  return(false);
	else
	  m_Rects.insert(*i);
      }
      else if(i->second == 1) {
	if(j->second == 0)
	  return(false);
	else
	  m_Rects.insert(*i);
      }
      else
	m_Rects.insert(*j);
    }
  }
  return(true);
}

bool HeapBox::operator==(const HeapBox& hb) const {
  return(m_Box == hb.m_Box);
}

bool HeapBox::operator>(const HeapBox& hb) const {
  return(m_Box > hb.m_Box);
}

bool HeapBox::operator<(const HeapBox& hb) const {
  return(m_Box < hb.m_Box);
}

void HeapBox::rotate() {
  m_Box.rotate();
  m_bRotated = !m_bRotated;
}

void HeapBox::print() const {
  std::cout << "Box: " << m_Box << std::endl;
  std::cout << "Height mutex set: " << std::endl;
  m_iHeight->second.print();
  std::cout << "Intersected mutex set: " << std::endl;
  m_Rects.print();
  std::cout << "This box is " << (m_bRotated ? "" : "not ")
	    << "rotated" << std::endl
	    << "Conflict learning index is "
	    << m_nConflictLearningIndex << std::endl;
}
