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

#ifndef ERRORINFO_H
#define ERRORINFO_H

#include <iostream>
#include <deque>
#include "RDimensions.h"

class ErrorInfo;

class ErrorInfo {
  friend std::ostream& operator<<(std::ostream& os, const ErrorInfo& ei);

 public:
  ErrorInfo();
  ErrorInfo(const RDimensions& b, const std::deque<RDimensions>& v,
	    bool bKorf, bool bMine);
  ErrorInfo(const RDimensions& b, const std::deque<RDimensions>& v,
	    bool bKorf, bool bMine, bool bKorf2, bool bMine2);
  virtual ~ErrorInfo();
  RDimensions m_Box;
  std::deque<RDimensions> m_vInstance;
  bool m_bKorfFeasible;
  bool m_bMineFeasible;
  bool m_bKorfAttempted;
  bool m_bMineAttempted;
};

std::ostream& operator<<(std::ostream& os, const ErrorInfo& ei);

#endif // ERRORINFO_H
