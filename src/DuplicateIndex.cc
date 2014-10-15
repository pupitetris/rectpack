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

#include "DuplicateIndex.h"
#include "Parameters.h"
#include "Rectangle.h"

DuplicateIndex::DuplicateIndex() {
}

DuplicateIndex::~DuplicateIndex() {
}

void DuplicateIndex::initialize(const Parameters* p,
				RectPtrArray& r) {
  m_vNext.clear();
  m_vPrev.clear();
  m_vNext.resize(r.size(), NULL);
  m_vPrev.resize(r.size(), NULL);
  if(p->m_vInstance.m_bUnique)
    return;

  for(size_t i = 0; i < m_vNext.size(); ++i) {
    m_vNext[i] = NULL;
    for(size_t j = i + 1; j < r.size(); ++j)
      if(r[i]->rotatable() || r[j]->rotatable()) {
	if(r[i]->uequal(*r[j])) {
	  m_vNext[i] = r[j];
	  break;
	}
      }
      else {
	if(r[i]->equal(*r[j])) {
	  m_vNext[i] = r[j];
	  break;
	}
      }
  }

  for(int i = m_vPrev.size() - 1; i >= 0; --i) {
    m_vPrev[i] = NULL;
    for(int j = i - 1; j >= 0; --j)
      if(r[i]->rotatable() || r[j]->rotatable()) {
	if(r[i]->uequal(*r[j])) {
	  m_vPrev[i] = r[j];
	  break;
	}
      }
      else {
	if(r[i]->equal(*r[j])) {
	  m_vPrev[i] = r[j];
	  break;
	}
      }
  }
}

Rectangle* DuplicateIndex::next(const Rectangle* r) const {
  return(m_vNext[r->m_nID]);
}

Rectangle* DuplicateIndex::prev(const Rectangle* r) const {
  return(m_vPrev[r->m_nID]);
}

bool DuplicateIndex::first(const Rectangle* r) const {
  return(prev(r) == NULL);
}

bool DuplicateIndex::last(const Rectangle* r) const {
  return(next(r) == NULL);
}
