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

#ifndef PRINTER_H
#define PRINTER_H

#include "Dimensions.h"
#include "EmptyCorner.h"
#include "Integer.h"
#include <list>
#include <map>
#include <ostream>
#include <set>
#include <vector>

class Component;

class Printer {
 public:
  static void print(const std::vector<Int>& v);
  static void print(const std::vector<UInt>& v);
  static void print(const std::vector<bool>& v);
  static void print(const std::vector<std::list<UInt> >& v);
  static void printspace(int n);
  static void printspace(int n, std::ostream& os);
  static void print(const std::set<Int>& s);
  static void print(const std::set<Int>& s, std::ostream& os);
  static void print(const std::set<UInt>& s, std::ostream& os);
  static void print(const std::set<std::set<Int> >& s);
  static void print(const std::set<std::set<Int> >& s, std::ostream& os);
  static void print(const std::set<Component*>& s);
  static void print(const std::set<Component*>& s, std::ostream& os);
  static void print(const std::map<Dimensions, std::set<Int> >& s);
  static void print(const std::map<Dimensions, std::set<Int> >& s, std::ostream& os);
  static void print(const std::vector<EmptyCorner>& v, std::ostream& os);
};

#endif // PRINTER_H
