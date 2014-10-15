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

#include "Component.h"
#include "Printer.h"
#include <iostream>
#include <iomanip>

void Printer::print(const std::vector<Int>& v) {
  for(UInt i = 0; i < v.size(); ++i)
    std::cout << std::setw(3) << i << ": " << v[i] << std::endl;
}

void Printer::print(const std::vector<UInt>& v) {
  for(UInt i = 0; i < v.size(); ++i)
    std::cout << std::setw(3) << i << ": " << v[i] << std::endl;
}

void Printer::print(const std::vector<bool>& v) {
  for(UInt i = 0; i < v.size(); ++i)
    std::cout << std::setw(3) << i << ": " << (v[i] ? "1" : "0") << std::endl;
}

void Printer::print(const std::vector<std::list<UInt> >& v) {
  for(UInt i = 0; i < v.size(); ++i) {
    std::cout << std::setw(3) << i << ":";
    for(std::list<UInt>::const_iterator j = v[i].begin();
	j != v[i].end(); ++j)
      std::cout << " " << *j;
    std::cout << std::endl;
  }
}

void Printer::print(const std::set<Int>& s) {
  print(s, std::cout);
}

void Printer::print(const std::set<std::set<Int> >& s) {
  print(s, std::cout);
}

void Printer::print(const std::set<Component*>& s) {
  print(s, std::cout);
}

void Printer::print(const std::set<Int>& s, std::ostream& os) {
  os << "{";
  if(!s.empty()) {
    std::set<Int>::const_iterator i = s.begin();
    os << *i;
    for(++i; i != s.end(); ++i)
      os << "," << *i;
  }
  os << "}" << std::flush;
}

void Printer::print(const std::set<std::set<Int> >& s, std::ostream& os) {
  os << "{";
  if(!s.empty()) {
    std::set<std::set<Int> >::const_iterator i = s.begin();
    print(*i, os);
    for(++i; i != s.end(); ++i) {
      os << ",";
      print(*i, os);
    }
  }
  os << "}" << std::flush;
}

void Printer::print(const std::set<UInt>& s, std::ostream& os) {
  os << "{";
  if(!s.empty()) {
    std::set<UInt>::const_iterator i = s.begin();
    os << *i;
    for(++i; i != s.end(); ++i)
      os << "," << *i;
  }
  os << "}" << std::flush;
}

void Printer::print(const std::set<Component*>& s, std::ostream& os) {
  os << "{";
  if(!s.empty()) {
    std::set<Component*>::const_iterator i = s.begin();
    os << (*i)->m_nID;
    for(++i; i != s.end(); ++i)
      os << "," << (*i)->m_nID;
  }
  os << "}" << std::flush;
}

void Printer::printspace(int n) {
  printspace(n, std::cout);
}

void Printer::printspace(int n, std::ostream& os) {
  for(int i = 0; i < n; ++i)
    os << " ";
}

void Printer::print(const std::vector<EmptyCorner>& v,
		    std::ostream& os) {
  os << '{';
  std::vector<EmptyCorner>::const_iterator i = v.begin();
  if(!v.empty()) {
    os << *i;
    for(++i; i != v.end(); ++i)
      os << ',' << *i;
  }
  os << '}';
}
