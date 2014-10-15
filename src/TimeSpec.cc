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

#include "TimeSpec.h"
#include <iomanip>
#include <sstream>
#include <math.h>

TimeSpec::TimeSpec() {
  tv_sec = 0;
  tv_nsec = 0;
}

TimeSpec::TimeSpec(const double& nTime) {
  tv_sec = (time_t) nTime;
  tv_nsec = (long) ((nTime - tv_sec) * 1000000000);
}

TimeSpec::TimeSpec(const TimeSpec& src) {
  tv_sec = src.tv_sec;
  tv_nsec = src.tv_nsec;
}

TimeSpec::TimeSpec(const long& nSec, const long& nNSec) {
  tv_sec = (int) nSec;
  tv_nsec = nNSec;
}

TimeSpec::~TimeSpec() {
}

void TimeSpec::set(const long& nSec, const long& nNSec) {
  tv_sec = (int) nSec;
  tv_nsec = nNSec;
}

void TimeSpec::set(const double& nTime) {
  tv_sec = (time_t) nTime;
  tv_nsec = (long) ((nTime - tv_sec) * 1000000000);
}

const TimeSpec& TimeSpec::operator=(const double& nTime) {
  tv_sec = (time_t) nTime;
  tv_nsec = (long) ((nTime - tv_sec) * 1000000000);
  return(*this);
}

const TimeSpec TimeSpec::operator-(const TimeSpec& rhs) const {
  TimeSpec ret(*this);
  ret -= rhs;
  return(ret);
}

const TimeSpec TimeSpec::operator+(const TimeSpec& rhs) const {
  TimeSpec ret(*this);
  ret += rhs;
  return(ret);
}

const TimeSpec TimeSpec::operator*(int n) const {
  TimeSpec ret(*this);
  ret *= n;
  return(ret);
}

const TimeSpec& TimeSpec::operator-=(const TimeSpec& rhs) {
  tv_sec -= rhs.tv_sec;
  tv_nsec -= rhs.tv_nsec;
  if(tv_nsec < 0) {
    tv_sec -= 1;
    tv_nsec += 1000000000;
  }
  return(*this);
}

const TimeSpec& TimeSpec::operator+=(const TimeSpec& rhs) {
  tv_sec += rhs.tv_sec;
  tv_nsec += rhs.tv_nsec;
  if(tv_nsec > 1000000000) {
    tv_sec += 1;
    tv_nsec -= 1000000000;
  }
  return(*this);
}

const TimeSpec& TimeSpec::operator*=(int n) {
  tv_sec *= n;
  unsigned long long t(tv_nsec);
  t *= n;
  tv_sec += t / 1000000000;
  tv_nsec = t % 1000000000;
  return(*this);
}

const TimeSpec& TimeSpec::operator*=(const double& n) {
  double d(toDouble() * n);
  tv_sec = (unsigned long) floor(d);
  tv_nsec = (unsigned long) floor((d - tv_sec) * 1000000000);
  return(*this);
}

void TimeSpec::print() const {
  std::cout << *this;
}

double TimeSpec::toDouble() const {
  return(tv_sec + tv_nsec / 1000000000.0);
}

void TimeSpec::tick() {
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, this);
}

void TimeSpec::tock() {
  TimeSpec t(*this);
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, this);
  operator-=(t);
}

std::ostream& operator<<(std::ostream& os, const TimeSpec& rhs) {
  return(os << (rhs.tv_sec + (rhs.tv_nsec >= 500000000 ? 1 : 0)));
}

std::string TimeSpec::toString() const {
  int nSeconds = tv_sec % 60 + (tv_nsec >= 500000000 ? 1 : 0);
  int nMinutes = tv_sec / 60 % 60;
  int nHours = tv_sec / 3600 % 24;
  int nDays = tv_sec / 86400;
  std::ostringstream ss;
  if(nDays)
    ss << nDays << ':' 
       << std::setfill('0') << std::setw(2) << nHours << ':'
       << std::setfill('0') << std::setw(2) << nMinutes << ':'
       << std::setfill('0') << std::setw(2) << nSeconds;
  else if(nHours)
    ss << nHours << ':'
       << std::setfill('0') << std::setw(2) << nMinutes << ':'
       << std::setfill('0') << std::setw(2) << nSeconds;
  else if(nMinutes)
    ss << nMinutes << ':' 
       << std::setfill('0') << std::setw(2) << nSeconds;
  else
    ss << ':'
       << std::setfill('0') << std::setw(2) << nSeconds;
  return(ss.str());
}

bool TimeSpec::operator<(const TimeSpec& rhs) const {
  if(tv_sec != rhs.tv_sec) return(tv_sec < rhs.tv_sec);
  return(tv_nsec < rhs.tv_nsec);
}

bool TimeSpec::operator==(const TimeSpec& rhs) const {
  return(tv_sec == rhs.tv_sec && tv_nsec == rhs.tv_nsec);
}

bool TimeSpec::operator!=(const TimeSpec& rhs) const {
  return(tv_sec != rhs.tv_sec || tv_nsec != rhs.tv_nsec);
}

void TimeSpec::clear() {
  tv_sec = tv_nsec = 0;
}
