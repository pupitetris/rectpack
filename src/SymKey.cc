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

#include "SymKey.h"

#include <limits>

SymKey::SymKey() :
  m_nMinCoordinate(0),
  m_nID(std::numeric_limits<UInt>::max()),
  m_pTable(NULL),
  m_nGap(std::numeric_limits<UInt>::max()) {
}

SymKey::SymKey(const SymKey& rhs) :
  m_nMinCoordinate(rhs.m_nMinCoordinate),
  m_nID(rhs.m_nID),
  m_pTable(rhs.m_pTable),
  m_nGap(rhs.m_nGap) {
}

SymKey::SymKey(UInt nMinCoordinate, UInt nID,
	       std::vector<std::vector<UInt> >* pTable, UInt nGap) :
  m_nMinCoordinate(nMinCoordinate),
  m_nID(nID),
  m_pTable(pTable),
  m_nGap(nGap) {
}

SymKey::~SymKey() {
}

const SymKey& SymKey::operator=(const SymKey& rhs) {
  m_nMinCoordinate = rhs.m_nMinCoordinate;
  m_nID = rhs.m_nID;
  m_pTable = rhs.m_pTable;
  m_nGap = rhs.m_nGap;
  return(*this);
}

bool SymKey::operator==(const SymKey& rhs) const {
  return(m_nMinCoordinate == rhs.m_nMinCoordinate &&
	 m_nID == rhs.m_nID &&
	 m_pTable == rhs.m_pTable &&
	 m_nGap == rhs.m_nGap);
}

bool SymKey::operator<(const SymKey& rhs) const {
  if(m_nMinCoordinate != rhs.m_nMinCoordinate)
    return(m_nMinCoordinate < rhs.m_nMinCoordinate);
  else if(m_nID != rhs.m_nID)
    return(m_nID < rhs.m_nID);
  else if(m_pTable != rhs.m_pTable)
    return(m_pTable < rhs.m_pTable);
  else
    return(m_nGap < rhs.m_nGap);
}
