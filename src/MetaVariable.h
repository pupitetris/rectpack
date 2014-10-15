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

#ifndef METAVARIABLE_H
#define METAVARIABLE_H

#include "Integer.h"
#include "MetaDomain.h"

class MetaVarDesc;
class MetaVariable;

class MetaVariable {
  friend std::ostream& operator<<(std::ostream& os,
				  const MetaVariable& rhs);

 public:
  MetaVariable();
  virtual ~MetaVariable();
  void initialize(const MetaVarDesc* pDesc);
  void assign(MetaDomain::ValueT nValue);

  /**
   * Point to the description for this variable.
   */

  const MetaVarDesc* m_pDesc;

  /**
   * We also capture its current domain and assignment.
   */

  MetaDomain m_Domain;
  MetaDomain::ValueT m_nValue;
};

std::ostream& operator<<(std::ostream& os, const MetaVariable& rhs);

#endif // METAVARIABLE_H
