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

#include "MpzWrapper.h"
#include <iostream>
#include <sstream>

MpzWrapper::MpzWrapper() {
}

MpzWrapper::MpzWrapper(const mpz_class& n) :
  mpz_class(n) {
}

MpzWrapper::MpzWrapper(int n) :
  mpz_class(n) {
}

MpzWrapper::MpzWrapper(unsigned int n) :
  mpz_class(n) {
}

MpzWrapper::MpzWrapper(const long long& n) {
  std::ostringstream oss;
  oss << n;
  mpz_class::set_str(oss.str(), 10);
}

MpzWrapper::MpzWrapper(const unsigned long long& n) {
  std::ostringstream oss;
  oss << n;
  mpz_class::set_str(oss.str(), 10);
}

MpzWrapper::~MpzWrapper() {
}

void MpzWrapper::print() const {
  std::cout << *this << std::flush;
}

MpzWrapper& MpzWrapper::operator*=(const MpzWrapper& n) {
  mpz_class::operator*=(n);
  return(*this);
}

MpzWrapper MpzWrapper::lcm(const MpzWrapper& n) const {
  MpzWrapper m;
  mpz_lcm(m.get_mpz_t(), get_mpz_t(), n.get_mpz_t());
  return(m);
}

MpzWrapper MpzWrapper::gcd(const MpzWrapper& n) const {
  MpzWrapper m;
  mpz_gcd(m.get_mpz_t(), get_mpz_t(), n.get_mpz_t());
  return(m);
}

const mpz_class& MpzWrapper::get_mpz_class() const {
  return(*this);
}

mpz_class& MpzWrapper::get_mpz_class() {
  return(*this);
}
