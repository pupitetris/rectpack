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

#ifndef MPQWRAPPER_H
#define MPQWRAPPER_H

#include "MpzWrapper.h"
#include "Integer.h"
#include <gmpxx.h>
#include <sstream>

class MpqWrapper;

class MpqWrapper : private mpq_class {
 public:
  friend std::ostream& operator<<(std::ostream& os, const MpqWrapper& mpq);

  MpqWrapper();
  MpqWrapper(const mpq_class& rhs);
  MpqWrapper(const MpqWrapper& rhs);

  MpqWrapper(float f);
  MpqWrapper(int n);
  MpqWrapper(unsigned int n);
  MpqWrapper(const long long& n);
  MpqWrapper(const unsigned long long& n);
  MpqWrapper(const MpzWrapper& n);

  MpqWrapper(int p, int q);
  MpqWrapper(unsigned int p, unsigned int q);
  MpqWrapper(const long long& p, const long long& q);
  MpqWrapper(const unsigned long long& p, const unsigned long long& q);

  ~MpqWrapper();

  void initialize(const std::string& s);

  template<class T> void initialize(const T& p) {
    std::ostringstream oss;
    oss << p;
    mpq_class::set_str(oss.str(), 10);
  };

  template<class T1, class T2> void initialize(const T1& p,
					       const T2& q) {
    std::ostringstream oss;
    oss << p << '/' << q;
    mpq_class::set_str(oss.str(), 10);
    mpq_class::canonicalize();
  };
  
  template<class T1, class T2> void initializeFloat(const T1& nWhole,
						    const T2& nDecimal) {
    initialize(nWhole);
    std::ostringstream oss;
    oss << nDecimal;
    size_t nDigits = oss.str().size();
    oss << nDecimal << "/1";
    for(size_t i = 0; i <= nDigits; ++i) oss << '0';
    mpq_class::set_str(oss.str(), 10);
    mpq_class::canonicalize();
    operator+=(nWhole);
  };

  void clear();

  MpqWrapper& operator=(const MpqWrapper& n);
  MpqWrapper operator+(const MpqWrapper& n) const;
  MpqWrapper operator-(const MpqWrapper& n) const;
  MpqWrapper operator/(const MpqWrapper& n) const;
  MpqWrapper operator*(const MpqWrapper& n) const;

  MpqWrapper& operator+=(const MpqWrapper& n);
  MpqWrapper& operator-=(const MpqWrapper& n);
  MpqWrapper& operator/=(const MpqWrapper& n);
  MpqWrapper& operator*=(const MpqWrapper& n);

  bool operator!=(const MpqWrapper& n) const;
  bool operator==(const MpqWrapper& n) const;
  bool operator>(const MpqWrapper& n) const;
  bool operator>=(const MpqWrapper& n) const;
  bool operator<(const MpqWrapper& n) const;
  bool operator<=(const MpqWrapper& n) const;

  void ceil();
  void floor();
  bool integer() const;
  UInt roundUp() const;
  UInt get_ui() const;
  float get_f() const;
  double get_d() const;
  UInt get_num() const;
  UInt get_den() const;
  std::string get_str() const;
  MpzWrapper get_num_mpz() const;
  MpzWrapper get_den_mpz() const;
  void print() const;
};

std::ostream& operator<<(std::ostream& os, const MpqWrapper& mpq);

#endif // MPQWRAPPER_H
