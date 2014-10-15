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

#ifndef MPZWRAPPER_H
#define MPZWRAPPER_H

#include <gmpxx.h>

class MpzWrapper;

class MpzWrapper : protected mpz_class {
 public:
  MpzWrapper();
  MpzWrapper(const mpz_class& n);

  MpzWrapper(int n);
  MpzWrapper(unsigned int n);
  MpzWrapper(const long long& n);
  MpzWrapper(const unsigned long long& n);
  ~MpzWrapper();

  MpzWrapper& operator*=(const MpzWrapper& n);
  MpzWrapper lcm(const MpzWrapper& n) const;
  MpzWrapper gcd(const MpzWrapper& n) const;

  /**
   * Internal accessor functions.
   */

  const mpz_class& get_mpz_class() const;
  mpz_class& get_mpz_class();
  
  void print() const;
};

#endif // MPZWRAPPER_H
