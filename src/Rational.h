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

#ifndef RATIONAL_H
#define RATIONAL_H

#include "MpqWrapper.h"
#include <gmpxx.h>
#include <ostream>

//#include <boost/rational.hpp>
//typedef boost::rational<size_t> URational;
//typedef boost::rational<int> Rational;
//typedef mpq_class Rational;
//typedef mpq_class URational;

typedef MpqWrapper Rational;
typedef MpqWrapper URational;

#endif // RATIONAL_H
