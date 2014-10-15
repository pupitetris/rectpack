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

#ifndef TIMESPEC_H
#define TIMESPEC_H

#include <time.h>
#include <iostream>

class TimeSpec;

class TimeSpec : public timespec {
 public:
  TimeSpec();
  TimeSpec(const double& nTime);
  TimeSpec(const long& nSec, const long& nNSec);
  TimeSpec(const TimeSpec& src);
  virtual ~TimeSpec();
  void set(const long& nSec, const long& nNSec);
  void set(const double& nTime);
  bool operator==(const TimeSpec& rhs) const;
  bool operator!=(const TimeSpec& rhs) const;
  const TimeSpec& operator=(const double& nTime);
  const TimeSpec operator-(const TimeSpec& rhs) const;
  const TimeSpec operator+(const TimeSpec& rhs) const;
  const TimeSpec operator*(int n) const;
  const TimeSpec& operator-=(const TimeSpec& rhs);
  const TimeSpec& operator+=(const TimeSpec& rhs);
  const TimeSpec& operator*=(int n);
  const TimeSpec& operator*=(const double& n);
  bool operator<(const TimeSpec& rhs) const;
  void print() const;
  void clear();
  double toDouble() const;
  void tick();
  void tock();
  std::string toString() const;
};

std::ostream& operator<<(std::ostream& os, const TimeSpec& rhs);

#endif // TIMESPEC_H
