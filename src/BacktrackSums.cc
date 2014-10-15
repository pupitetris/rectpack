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

#include "BacktrackSums.h"
#include "Compulsory.h"

BacktrackSums::BacktrackSums() {
  m_vStackSize.push_back(0);
}

BacktrackSums::~BacktrackSums() {
}

void BacktrackSums::initialize(RectPtrArray::const_iterator iBegin,
			       RectPtrArray::const_iterator iEnd) {
  m_vUnplaced.clear();
  m_vUnplaced.resize(iEnd - iBegin);
  if(m_vUnplaced.empty()) return;

  --iEnd;
  m_vUnplaced.back().insert(0);
  m_vUnplaced.back().insert((*iEnd)->m_nWidth);
  while(iBegin != iEnd) {
    --iEnd;
    const Rectangle* r = *iEnd;
    m_vUnplaced[r->m_nID] = m_vUnplaced[r->m_nID + 1];
    m_vUnplaced[r->m_nID].add(r->m_nWidth);
  }
}

void BacktrackSums::push() {
  m_vStackSize.push_back(m_vStack.size());
}

void BacktrackSums::pop() {
  while(m_vStack.size() > m_vStackSize.back()) {
    erase(m_vStack.back());
    m_vStack.pop_back();
  }
  m_vStackSize.pop_back();
}

void BacktrackSums::add(const UInt& n, const UInt& nMax) {
  SimpleSums mNew;
  for(iterator i = begin(); i != end(); ++i) {
    UInt nNew = *i + n;
    if(nNew > nMax)
      break;
    if(find(nNew) == end())
      mNew.insert(mNew.end(), nNew);
  }
  for(SimpleSums::iterator i = mNew.begin(); i != mNew.end(); ++i)
    m_vStack.push_back(insert(*i).first);
}

void BacktrackSums::add(const Compulsory& nValue, const UInt& nMax) {
  iterator j(lower_bound(nValue.m_nStart.m_nLeft));
  while(j != end() && *j <= nValue.m_nStart.m_nRight) {
    UInt nSum = *j + nValue.m_pRect->m_nWidth;
    if(nSum > nMax)
      break;
    std::pair<iterator, bool> r = insert(nSum);
    if(r.second)
      m_vStack.push_back(r.first);
    ++j;
  }
}

void BacktrackSums::push(const UInt& s) {
  push();
  std::pair<iterator, bool> r = insert(s);
  if(r.second)
    m_vStack.push_back(r.first);
}

void BacktrackSums::clear() {
  SimpleSums::clear();
  m_vStack.clear();
  m_vStackSize.clear();
  m_vUnplaced.clear();
}
