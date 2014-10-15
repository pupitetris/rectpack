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

#include "MpqWrapper.h"
#include <iostream>

MpqWrapper::MpqWrapper() {
}

MpqWrapper::MpqWrapper(const mpq_class& rhs) :
  mpq_class(rhs) {
}

MpqWrapper::MpqWrapper(const MpqWrapper& rhs) :
  mpq_class(rhs) {
}

MpqWrapper::MpqWrapper(const MpzWrapper& n) {
  mpq_class::get_num() = n.get_mpz_class();
  mpq_class::get_den() = 1;
}

MpqWrapper::MpqWrapper(int n) :
  mpq_class(n, 1) {
}

MpqWrapper::MpqWrapper(unsigned int n) :
  mpq_class(n, 1) {
}

MpqWrapper::MpqWrapper(const long long& n) {
  std::ostringstream oss;
  oss << n;
  mpq_class::set_str(oss.str(), 10);
}

MpqWrapper::MpqWrapper(const unsigned long long& n) {
  std::ostringstream oss;
  oss << n;
  mpq_class::set_str(oss.str(), 10);
}

MpqWrapper::MpqWrapper(int p, int q) {
  std::ostringstream oss;
  oss << p << '/' << q;
  mpq_class::set_str(oss.str(), 10);
  mpq_class::canonicalize();
}

MpqWrapper::MpqWrapper(unsigned int p, unsigned int q) {
  std::ostringstream oss;
  oss << p << '/' << q;
  mpq_class::set_str(oss.str(), 10);
  mpq_class::canonicalize();
}

MpqWrapper::MpqWrapper(const long long& p, const long long& q) {
  std::ostringstream oss;
  oss << p << '/' << q;
  mpq_class::set_str(oss.str(), 10);
  mpq_class::canonicalize();
}

MpqWrapper::MpqWrapper(const unsigned long long& p,
		       const unsigned long long& q) {
  std::ostringstream oss;
  oss << p << '/' << q;
  mpq_class::set_str(oss.str(), 10);
  mpq_class::canonicalize();
}

MpqWrapper::MpqWrapper(float f) :
  mpq_class(f) {
}

MpqWrapper::~MpqWrapper() {
}

void MpqWrapper::print() const {
  std::cout << get_num();
  if(get_den() > 1)
    std::cout << '/' << get_den();
  std::cout << std::flush;
}

void MpqWrapper::initialize(const std::string& s) {
  size_t n = s.find_first_of('.');
  if(n >= s.size()) {
    mpq_class::set_str(s, 10);
    canonicalize();
  }
  else {
    std::string sWhole(s.substr(0, n));
    std::string sDecimal(s.substr(n + 1));
    std::ostringstream oss;
    oss << sDecimal << "/1";
    for(size_t i = 0; i < sDecimal.size(); ++i) oss << '0';
    mpq_class::set_str(oss.str(), 10);
    canonicalize();
    MpqWrapper nWhole;
    nWhole.initialize(sWhole);
    operator+=(nWhole);
  }
}

void MpqWrapper::clear() {
  mpq_class::set_str("0", 10);
}

std::ostream& operator<<(std::ostream& os, const MpqWrapper& mpq) {
  if(mpq.get_den() == 1)
    return(os << mpq.get_str());
  else
    return(os << mpq.mpq_class::get_num().get_str() << '/'
	   << mpq.mpq_class::get_den().get_str());
}

bool MpqWrapper::integer() const {
  return(mpq_class::get_den() == 1);
}

UInt MpqWrapper::get_ui() const {
  return(((UInt) get_num()) / (UInt) get_den());
}

float MpqWrapper::get_f() const {
  return((float) mpq_class::get_d());
}

double MpqWrapper::get_d() const {
  return((float) mpq_class::get_d());
}

UInt MpqWrapper::get_num() const {
  return((UInt) mpq_class::get_num().get_ui());
}

UInt MpqWrapper::get_den() const {
  return((UInt) mpq_class::get_den().get_ui());
}

MpzWrapper MpqWrapper::get_num_mpz() const {
  return(MpzWrapper(mpq_class::get_num()));
}

MpzWrapper MpqWrapper::get_den_mpz() const {
  return(MpzWrapper(mpq_class::get_den()));
}

MpqWrapper& MpqWrapper::operator=(const MpqWrapper& n) {
  mpq_class::operator=(n);
  return(*this);
}

MpqWrapper MpqWrapper::operator+(const MpqWrapper& n) const {
  MpqWrapper m(*this);
  return(m += n);
}

MpqWrapper MpqWrapper::operator-(const MpqWrapper& n) const {
  MpqWrapper m(*this);
  return(m -= n);
}

MpqWrapper MpqWrapper::operator/(const MpqWrapper& n) const {
  MpqWrapper m(*this);
  return(m /= n);
}

MpqWrapper MpqWrapper::operator*(const MpqWrapper& n) const {
  MpqWrapper m(*this);
  return(m *= n);
}

MpqWrapper& MpqWrapper::operator+=(const MpqWrapper& n) {
  mpq_class::operator+=((mpq_class) n);
  return(*this);
}

MpqWrapper& MpqWrapper::operator-=(const MpqWrapper& n) {
  mpq_class::operator-=((mpq_class) n);
  return(*this);
}

MpqWrapper& MpqWrapper::operator/=(const MpqWrapper& n) {
  mpq_class::operator/=((mpq_class) n);
  return(*this);
}

MpqWrapper& MpqWrapper::operator*=(const MpqWrapper& n) {
  mpq_class::operator*=((mpq_class) n);
  return(*this);
}

bool MpqWrapper::operator!=(const MpqWrapper& n) const {
  return((mpq_class) (*this) != (mpq_class) n);
}

bool MpqWrapper::operator==(const MpqWrapper& n) const {
  return((mpq_class) (*this) == (mpq_class) n);
}

bool MpqWrapper::operator>(const MpqWrapper& n) const {
  return((mpq_class) (*this) > (mpq_class) n);
}

bool MpqWrapper::operator>=(const MpqWrapper& n) const {
  return((mpq_class) (*this) >= (mpq_class) n);
}

bool MpqWrapper::operator<(const MpqWrapper& n) const {
  return((mpq_class) (*this) < (mpq_class) n);
}

bool MpqWrapper::operator<=(const MpqWrapper& n) const {
  return((mpq_class) (*this) <= (mpq_class) n);
}

UInt MpqWrapper::roundUp() const {
  if(integer()) return(get_ui());
  else return(get_ui() + 1);
}

std::string MpqWrapper::get_str() const {
  return(mpq_class::get_str());
}

void MpqWrapper::ceil() {
  if(integer()) return;
  mpz_class n = mpq_class::get_num() / mpq_class::get_den();
  ++n;
  mpq_class::get_num() = n;
  mpq_class::get_den() = 1;
}

void MpqWrapper::floor() {
  if(integer()) return;
  mpz_class n = mpq_class::get_num() / mpq_class::get_den();
  mpq_class::get_num() = n;
  mpq_class::get_den() = 1;
}
