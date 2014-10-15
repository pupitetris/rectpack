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

#include "MetaVarDesc.h"
#include "MetaVariable.h"

MetaVariable::MetaVariable() :
  m_pDesc(NULL),
  m_nValue(MetaDomain::UNASSIGNED) {
}

MetaVariable::~MetaVariable() {
}

void MetaVariable::initialize(const MetaVarDesc* pDesc) {
  m_pDesc = pDesc;
  m_Domain.initialize();
  m_nValue = MetaDomain::UNASSIGNED;
}

void MetaVariable::assign(MetaDomain::ValueT nValue) {
  m_nValue = nValue;
  m_Domain.clear();
  m_Domain.insert(nValue);
}

std::ostream& operator<<(std::ostream& os, const MetaVariable& rhs) {
  return(os << *rhs.m_pDesc << '=' << rhs.m_Domain);
}
