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

#include "Parameters.h"
#include "Rectangle.h"
#include "ValueStack.h"
#include <iomanip>

ValueStack::ValueStack() {
}

ValueStack::~ValueStack() {
}

void ValueStack::initialize(const Parameters* pParams) {
  m_pParams = pParams;
  m_vStack.reserve(pParams->m_vInstance.size() *
		   pParams->m_vInstance.size() * 2);
  m_vInserted.reserve(pParams->m_vInstance.size() * 2 + 1);
  m_vSize.reserve(pParams->m_vInstance.size() * 2 + 1);
}

void ValueStack::initialize(const BoxDimensions* pBox) {
  m_vStack.clear();
  m_vInserted.clear();
  m_vInserted.push_back(boost::dynamic_bitset<>(m_pParams->m_vInstance.size(), false));
  m_vSize.clear();
  m_vSize.push_back(0);
}

void ValueStack::save() {
  m_vInserted.push_back(boost::dynamic_bitset<>(m_pParams->m_vInstance.size(), false));
  m_vSize.push_back(m_vStack.size());
}

void ValueStack::restore() {
  m_vInserted.pop_back();
  m_vSize.pop_back();
}

const Compulsory& ValueStack::top() const {
  return(m_vStack.back());
}

bool ValueStack::empty() const {
  return(m_vSize.back() == m_vStack.size());
}

void ValueStack::push(const Compulsory& c) {
  if(m_vInserted.back().test(c.m_pRect->m_nID)) return;
  m_vInserted.back().set(c.m_pRect->m_nID);
  m_vStack.push_back(c);
}

void ValueStack::pop() {
  m_vStack.pop_back();
}

void ValueStack::print() const {
  size_t i(0);
  for(size_t j = 1; j < m_vSize.size(); ++j) {
    std::cout << "Frame " << std::setw(2) << j
	      << ": " << std::flush;
    if(i < m_vSize[j]) {
      std::cout << m_vStack[i];
      for(++i; i < m_vSize[j]; ++i)
	std::cout << "          " << m_vStack[i];
    }
    else std::cout << "empty." << std::endl;
  }

  std::cout << "Frame " << std::setw(2) << m_vSize.size()
	    << ": " << std::flush;
  if(i < m_vStack.size()) {
    std::cout << m_vStack[i];
    for(++i; i < m_vStack.size(); ++i)
      std::cout << "          " << m_vStack[i];
  }
  else std::cout << "empty." << std::endl;
}
