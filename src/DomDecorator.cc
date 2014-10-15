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

#include "DomDecorator.h"

DomDecorator::DomDecorator() :
  m_pT1(NULL),
  m_pT2(NULL) {
}

DomDecorator::DomDecorator(DomTable& t1, DomTable& t2) :
  m_pT1(&t1),
  m_pT2(&t2) {
}

void DomDecorator::initialize(DomTable& t1, DomTable& t2) {
  m_pT1 = &t1;
  m_pT2 = &t2;
}

DomDecorator::DomTable& DomDecorator::t1() {
  return(*m_pT1);
}

DomDecorator::DomTable& DomDecorator::t2() {
  return(*m_pT2);
}
