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

#include "DimsGreater.h"
#include "Instance.h"
#include "GreaterHeight.h"
#include "GreaterMinDim.h"
#include "GreaterWidth.h"
#include "LessRatioHW.h"
#include "LessRatioWH.h"
#include "MpzWrapper.h"
#include "RDimensions.h"
#include "Rational.h"
#include <boost/bind.hpp>
#include <boost/math/common_factor.hpp> 
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <iostream>

Instance::Instance() {
}

Instance::Instance(const Instance& i) :
  std::deque<RDimensions>(i),
  m_nBenchmark(i.m_nBenchmark),
  m_bSequential(i.m_bSequential),
  m_bUnique(i.m_bUnique),
  m_bAllSquares(i.m_bAllSquares),
  m_bUnoriented(i.m_bUnoriented),
  m_bOrientedNonSquares(i.m_bOrientedNonSquares),
  m_bDiagonalSymmetry(i.m_bDiagonalSymmetry),
  m_bSortedDecreasingArea(i.m_bSortedDecreasingArea),
  m_bSortedDecreasingMinDim(i.m_bSortedDecreasingMinDim),
  m_bSortedDecreasingWidth(i.m_bSortedDecreasingWidth),
  m_bSortedDecreasingSize(i.m_bSortedDecreasingSize),
  m_nTotalArea(i.m_nTotalArea),
  m_nMinArea(i.m_nMinArea),
  m_nMaxArea(i.m_nMaxArea),
  m_nMax(i.m_nMax),
  m_nMin(i.m_nMin),
  m_nMaxMin(i.m_nMaxMin),
  m_nStacked(i.m_nStacked),
  m_nMaxDim(i.m_nMaxDim), 
  m_nMinDim(i.m_nMinDim),
  m_nScale(i.m_nScale) {
}

const Instance& Instance::operator=(const Instance& i) {
  std::deque<RDimensions>::operator=(i);
  m_nBenchmark = i.m_nBenchmark;
  m_bSequential = i.m_bSequential;
  m_bUnique = i.m_bUnique;
  m_bAllSquares = i.m_bAllSquares;
  m_bUnoriented = i.m_bUnoriented;
  m_bOrientedNonSquares = i.m_bOrientedNonSquares;
  m_bDiagonalSymmetry = i.m_bDiagonalSymmetry;
  m_bSortedDecreasingArea = i.m_bSortedDecreasingArea;
  m_bSortedDecreasingMinDim = i.m_bSortedDecreasingMinDim;
  m_bSortedDecreasingWidth = i.m_bSortedDecreasingWidth;
  m_bSortedDecreasingSize = i.m_bSortedDecreasingSize;
  m_nTotalArea = i.m_nTotalArea;
  m_nMinArea = i.m_nMinArea;
  m_nMaxArea = i.m_nMaxArea;
  m_nMax = i.m_nMax;
  m_nMin = i.m_nMin;
  m_nMaxMin = i.m_nMaxMin;
  m_nStacked = i.m_nStacked;
  m_nMaxDim = i.m_nMaxDim; 
  m_nMinDim = i.m_nMinDim;
  m_nScale = i.m_nScale;
  return(*this);
}

Instance::~Instance() {
}

void Instance::parseInstance(const std::string& s) {
  clear();
  m_nScale = (UInt) 1;
  bool bParsed(false);
  namespace qi = boost::spirit::qi;
  namespace phoenix = boost::phoenix;
  using boost::spirit::qi::uint_;
  using boost::spirit::qi::int_;
  using boost::spirit::qi::float_;
  using boost::spirit::qi::rule;
  using boost::spirit::qi::lit;
  using boost::spirit::qi::eps;
  using boost::phoenix::push_back;
  using boost::spirit::qi::parse;

  RDimensions d;
  URational ur;
  UInt p(0), q(1);

  rule<const char*> rat =
    ( uint_[phoenix::ref(p) = qi::_1] >>
      lit('/') >>
      uint_[phoenix::ref(q) = qi::_1] );

  rule<const char*> floatstr =
    ( uint_[phoenix::ref(p) = qi::_1] >>
      lit('.') >>
      uint_[phoenix::ref(q) = qi::_1] );

  rule<const char*> dim =
    ( rat[boost::bind(&URational::initialize<UInt, UInt>, &ur, boost::ref(p), boost::ref(q))] |
      floatstr[boost::bind(&URational::initializeFloat<UInt, UInt>, &ur, boost::ref(p), boost::ref(q))] |
      uint_[phoenix::ref(p) = qi::_1][boost::bind(&URational::initialize<UInt, UInt>, &ur, boost::ref(p), 1)]);

  rule<const char*> box =
    dim[phoenix::ref(d.m_nWidth) = phoenix::ref(ur)] >>
    ( lit('x') | lit('X') ) >>
    dim[phoenix::ref(d.m_nHeight) = phoenix::ref(ur)] >>
    ( lit('o')
      [phoenix::ref(d.m_nOrientation) = RDimensions::ORIENTED] |
      lit('u')
      [phoenix::ref(d.m_nOrientation) = RDimensions::UNORIENTED] |
      eps
      [phoenix::ref(d.m_nOrientation) = RDimensions::UNSPECIFIED] );

  rule<const char*> boxes =
    box[push_back(phoenix::ref(*this), phoenix::ref(d))] >>
    * ( lit(',') >> box[push_back(phoenix::ref(*this), phoenix::ref(d))]);

  rule<const char*> single = uint_[phoenix::ref(p) = qi::_1];

  const char* cs = s.c_str();
  if(m_nBenchmark == 0) {
    bParsed = parse(cs, cs + s.size(), boxes);
  }
  else if(m_nBenchmark >= 1 && m_nBenchmark <= 8) {

    /**
     * The alternative case is to simply take in a single number
     * describing the instance size.
     */

    bParsed = parse(cs, cs + s.size(), single);
    if(bParsed) {
      std::deque<UInt> v;
      v.push_back(p);
      parseInstance(v);
    }
  }
  if(!bParsed) {
    std::cout << "I don't understand your instance description: "
	      << s << std::endl;
    exit(0);
  }
  else
    for(iterator i = begin(); i != end(); ++i)
      i->setArea();
}

void Instance::parseInstance(std::deque<UInt>& v) {
  std::deque<UInt> d1, d2;
  parseInstance(v, d1, d2);
}

void Instance::parseInstance(std::deque<UInt>& v,
			     std::deque<UInt>& vLower,
			     std::deque<UInt>& vUpper) {
  /**
   * If we get only a single number, that means we want to signify the
   * range 1-N.
   */

  if(v.size() == 1 && vLower.empty() && vUpper.empty()) {
    vLower.push_back(1);
    vUpper.push_back(v.front());
    v.clear();
  }

  UInt n(0);
  if(!v.empty())
    n = *(std::max_element(v.begin(), v.end()));
  if(!vUpper.empty())
    n = std::max(n, *(std::max_element(vUpper.begin(), vUpper.end())));
  clear();
  switch(m_nBenchmark) {

  case 1:
    for(UInt i = 0; i < v.size(); ++i)
      push_back(RDimensions(v[i], v[i]));
    for(UInt i = 0; i < vLower.size(); ++i)
      for(UInt j = vLower[i]; j <= vUpper[i]; ++j)
	push_back(RDimensions(j, j));
    break;

  case 2:
    for(UInt i = 0; i < v.size(); ++i)
      push_back(RDimensions(v[i], v[i] + 1));
    for(UInt i = 0; i < vLower.size(); ++i)
      for(UInt j = vLower[i]; j <= vUpper[i]; ++j)
	push_back(RDimensions(j, j + 1));
    break;

  case 3:
    for(UInt i = 0; i < v.size(); ++i)
      push_back(RDimensions(v[i], n + 1 - v[i]));
    for(UInt i = 0; i < vLower.size(); ++i)
      for(UInt j = vLower[i]; j <= vUpper[i]; ++j)
	push_back(RDimensions(j, n + 1 - j));
    break;

  case 4:
    for(UInt i = 0; i < v.size(); ++i)
      push_back(RDimensions(v[i], 2 * n - v[i]));
    for(UInt i = 0; i < vLower.size(); ++i)
      for(UInt j = vLower[i]; j <= vUpper[i]; ++j)
	push_back(RDimensions(j, 2 * n - j));
    break;

  case 5:
    for(UInt i = 0; i < v.size(); ++i)
      push_back(RDimensions(URational((UInt) 1, v[i]),
			    URational((UInt) 1, v[i] + 1)));
    for(UInt i = 0; i < vLower.size(); ++i)
      for(UInt j = vLower[i]; j <= vUpper[i]; ++j)
	push_back(RDimensions(URational((UInt) 1, j),
			      URational((UInt) 1, j + 1)));
    break;

  case 6:
    for(UInt i = 0; i < v.size(); ++i)
      push_back(RDimensions(v[i] * n + 1, n * (n + 1 - v[i]) + 1));
    for(UInt i = 0; i < vLower.size(); ++i)
      for(UInt j = vLower[i]; j <= vUpper[i]; ++j)
	push_back(RDimensions(j * n + 1, n * (n + 1 - j) + 1)); 
    break;

  case 7:
    for(UInt i = 0; i < v.size(); ++i)
      push_back(RDimensions(2 * v[i], 2 * (v[i] + 1)));
    for(UInt i = 0; i < vLower.size(); ++i)
      for(UInt j = vLower[i]; j <= vUpper[i]; ++j)
	push_back(RDimensions(2 * j, 2 * (j + 1)));
    break;

  case 8:
    for(UInt i = 0; i < v.size(); ++i)
      push_back(RDimensions(2 * v[i] - 1, 2 * v[i]));
    for(UInt i = 0; i < vLower.size(); ++i)
      for(UInt j = vLower[i]; j <= vUpper[i]; ++j)
	push_back(RDimensions(2 * j - 1, 2 * j));
    break;

  default:
    std::cout << "I don't recognize your requested benchmark "
	      << m_nBenchmark << std::endl;
    exit(0);
  };
}

void Instance::inferInstanceOrientation() {
  m_bOrientedNonSquares = false;
  for(iterator i = begin(); i != end(); ++i)
    if(oriented(*i) && !square(*i)) {
      m_bOrientedNonSquares = true;
      break;
    }
}

void Instance::inferInstanceProperties() {
  inferInstanceOrientation();

  /**
   * Attempt to infer uniqueness properties.
   */

  std::multiset<RDimensions> s(begin(), end());
  inferInstanceUniqueness(s);

  /**
   * Attempt to infer sequence properties.
   */

  inferInstanceSequential(s);

  /**
   * Are they all squares?
   */

  m_bAllSquares = true;
  for(std::deque<RDimensions>::const_iterator k = begin();
      k != end(); ++k)
    if(k->m_nWidth != k->m_nHeight) {
      m_bAllSquares = false;
      break;
    }

  /**
   * Are the items such that their areas and sizes can be sorted in
   * non-increasing order all the time?
   */

  inferInstanceSorting(s);

  /**
   * Determine if we are able to force diagonal symmetry along the
   * bounding box due to our given instance.
   */

  inferInstanceSymmetry(s);
  inferInstanceExtrema();
}

void Instance::inferInstanceSorting(const std::multiset<RDimensions>& s) {
  std::multiset<RDimensions>::const_iterator i(s.begin()), previous(s.begin());

  URational nPrevious = std::numeric_limits<UInt>::max();
  m_bSortedDecreasingArea = true;
  for(const_iterator j = begin(); j != end(); ++j)
    if(j->m_nArea > nPrevious) {
      m_bSortedDecreasingArea = false;
      break;
    }
    else nPrevious = j->m_nArea;

  nPrevious = std::numeric_limits<UInt>::max();
  m_bSortedDecreasingMinDim = true;
  for(const_iterator j = begin(); j != end(); ++j)
    if(j->m_nMinDim > nPrevious) {
      m_bSortedDecreasingMinDim = false;
      break;
    }
    else nPrevious = j->m_nMinDim;

  nPrevious = std::numeric_limits<UInt>::max();
  m_bSortedDecreasingWidth = true;
  for(const_iterator j = begin(); j != end(); ++j)
    if(j->m_nWidth > nPrevious) {
      m_bSortedDecreasingWidth = false;
      break;
    }
    else nPrevious = j->m_nWidth;

  nPrevious = std::numeric_limits<UInt>::max();
  m_bSortedDecreasingSize = true;
  for(const_iterator j = begin(); j != end(); ++j)
    if(j->m_nWidth > nPrevious ||
       j->m_nHeight > nPrevious) {
      m_bSortedDecreasingSize = false;
      break;
    }
    else nPrevious = j->m_nMinDim;
}

void Instance::inferInstanceUniqueness(const std::multiset<RDimensions>& s) {
  m_bUnique = true;
  for(std::multiset<RDimensions>::const_iterator i = s.begin();
      i != s.end(); ++i)
    for(std::multiset<RDimensions>::const_iterator j = i;
	j != s.end(); ++j)
      if((i != j) && (*i == *j || i->canEqual(*j, m_bUnoriented))) {
	m_bUnique = false;
	return;
      }
}

void Instance::inferInstanceSequential(const std::multiset<RDimensions>& s) {
  if(!m_bUnique || m_nBenchmark >= 3)
    m_bSequential = false;
  else {
    m_bSequential = true;
    for(UInt n = 1; n <= size(); ++n)
      if((m_nBenchmark == 1 &&
	  s.find(RDimensions(n, n)) == s.end()) ||
	 (m_nBenchmark == 2 &&
	  s.find(RDimensions(n, n + 1)) == s.end())) {
	m_bSequential = false;
	return;
      }
  }
}

void Instance::inferInstanceSymmetry(const std::multiset<RDimensions>& s) {
  m_bDiagonalSymmetry = true;
  if(!m_bOrientedNonSquares || m_bAllSquares || s.empty())
    return;

  /**
   * Local copy of our set since we will be operating on it.
   */

  std::multiset<RDimensions> r(s);

  /**
   * Remove all squares and unoriented rectangles since they naturally
   * won't disqualify the symmetry property.
   */

  std::list<std::multiset<RDimensions>::iterator> lDelete;
  for(std::multiset<RDimensions>::iterator i = r.begin();
      i != r.end(); ++i)
    if(i->square() || i->rotatable(m_bUnoriented))
      lDelete.push_back(i);
  while(!lDelete.empty()) {
    r.erase(lDelete.back());
    lDelete.pop_back();
  }

  /**
   * Now the remaining elements are all oriented non-squares. We must
   * make sure there is a 1-to-1 pairing. Obviously, there cannot be a
   * 1-to-1 pairing if we have an odd number of elements.
   */

  if(r.size() % 2 != 0) {
    m_bDiagonalSymmetry = false;
    return;
  }

  /**
   * For each item, we'll remove it, and look for its partner to
   * remove as well.
   */

  while(!r.empty()) {

    /**
     * Examine the next rectangle in our set.
     */

    RDimensions d = *r.begin();
    r.erase(r.begin());

    /**
     * Find its partner.
     */

    std::multiset<RDimensions>::iterator i;
    for(i = r.begin(); i != r.end(); ++i)
      if(d.m_nHeight == i->m_nWidth &&
	 d.m_nWidth == i->m_nHeight)
	break;

    /**
     * Did we find its partner? If so, we'll delete its
     * partner. Otherwise, this guy has no partner and we turn off
     * diagonal symmetry.
     */

    if(i == r.end()) {
      m_bDiagonalSymmetry = false;
      break;
    } else r.erase(i);
  }
}

void Instance::sortDecreasingArea() {
  std::sort(begin(),end(), std::greater<RDimensions>());
}

void Instance::sortIncreasingArea() {
  std::sort(begin(), end());
}

void Instance::inferInstanceExtrema() {
  m_nTotalArea = (UInt) 0;
  m_nMinArea = std::numeric_limits<UInt>::max();
  m_nMaxArea = std::numeric_limits<UInt>::min();
  m_nMin.initMax();
  m_nMax.initMin();
  m_nStacked.initMin();
  m_nMaxMin.initMin();
  for(const_iterator i = begin(); i != end(); ++i) {
    m_nMin.m_nWidth = std::min(m_nMin.m_nWidth, i->m_nWidth);
    m_nMin.m_nHeight = std::min(m_nMin.m_nHeight, i->m_nHeight);
    m_nMax.m_nWidth = std::max(m_nMax.m_nWidth, i->m_nWidth);
    m_nMax.m_nHeight = std::max(m_nMax.m_nHeight, i->m_nHeight);
    m_nTotalArea += i->m_nArea;
    m_nMinArea = std::min(m_nMinArea, i->m_nArea);
    m_nMaxArea = std::max(m_nMaxArea, i->m_nArea);
    if(i->rotatable(m_bUnoriented)) {
      m_nMaxMin.m_nHeight = std::max(m_nMaxMin.m_nHeight, i->m_nMinDim);
      m_nMaxMin.m_nWidth = std::max(m_nMaxMin.m_nWidth, i->m_nMinDim);
      m_nStacked.m_nHeight += std::max(i->m_nHeight, i->m_nWidth);
      m_nStacked.m_nWidth += std::max(i->m_nHeight, i->m_nWidth);
    }
    else {
      m_nMaxMin.m_nHeight = std::max(m_nMaxMin.m_nHeight, i->m_nHeight);
      m_nMaxMin.m_nWidth = std::max(m_nMaxMin.m_nWidth, i->m_nWidth);
      m_nStacked.m_nHeight += i->m_nHeight;
      m_nStacked.m_nWidth += i->m_nWidth;
    }
  }
  m_nMaxMin.setArea();
  m_nMax.setArea();
  m_nMin.setArea();
  m_nMaxDim = std::max(m_nMax.m_nWidth, m_nMax.m_nHeight);
  m_nMinDim = std::min(m_nMin.m_nWidth, m_nMin.m_nHeight);
  m_nStacked.setArea();
}

void Instance::print() const {
  if(!empty())
    std::cout << front();
  for(UInt i = 1; i < size(); ++i)
    std::cout << "," << operator[](i);
  std::cout << std::flush;
}

void Instance::sort(int nOrdering) {
  sort(nOrdering, false);
}

void Instance::sort(int nOrdering, bool bYSmallerThanX) {
  if(nOrdering == 0 || nOrdering == 1)
    return;
  else if(nOrdering == 2)
    sortDecreasingArea();
  else if(nOrdering == 3)
    sortDecreasingMinDim();
  else if(nOrdering == 4)
    sortDecreasingHeight();
  else if(nOrdering == 5)
    sortDecreasingWidth();
  else if(nOrdering == 6)
    sortIncreasingWH();
  else if(nOrdering == 7)
    sortIncreasingHW();
  else if(nOrdering == 8) {
    if(m_bUnoriented)
      sortDecreasingArea();
    else
      sortDecreasingWidth();
  }
}

void Instance::sortDecreasingMinDim() {
  std::sort(begin(),end(), GreaterMinDim());
}

void Instance::sortDecreasingHeight() {
  std::sort(begin(), end(), GreaterHeight());
}

void Instance::sortDecreasingWidth() {
  std::sort(begin(), end(), GreaterWidth());
}

void Instance::sortIncreasingWH() {
  std::sort(begin(), end(), LessRatioWH());
}

void Instance::sortIncreasingHW() {
  std::sort(begin(), end(), LessRatioHW());
}

void Instance::rewriteJValue(float nJValue) {
  UInt n = size();
  for(UInt i = 0; i < size(); ++i) {
    operator[](i).m_nWidth = n - i;
    UInt k = n - i; // k=1..n
    operator[](i).m_nHeight =
      (UInt) (nJValue*(n+1)+(1-2*nJValue)*k + 0.5);
    operator[](i).setArea();
  }
  m_nBenchmark = 0;
  inferInstanceProperties();
}

void Instance::scaleDiscrete() {
  MpzWrapper m((UInt) 1);
  for(iterator i = begin(); i != end(); ++i) {
    m = m.lcm(i->m_nWidth.get_den_mpz());
    m = m.lcm(i->m_nHeight.get_den_mpz());
  }
  for(iterator i = begin(); i != end(); ++i) {
    (*i) *= m;
    i->setArea();
  }
  m_nScale /= m;
}

void Instance::scaleDivisor() {
  MpzWrapper m(begin()->m_nWidth.get_num_mpz());
  for(iterator i = begin(); i != end(); ++i) {
    m = m.gcd(i->m_nWidth.get_num_mpz());
    m = m.gcd(i->m_nHeight.get_num_mpz());
  }
  for(iterator i = begin(); i != end(); ++i) {
    (*i) /= m;
    i->setArea();
  }
  m_nScale *= m;
}

bool Instance::rotatable(const RDimensions& r) const {
  return(!oriented(r) && !square(r));
}

bool Instance::oriented(const RDimensions& r) const {
  return(r.m_nOrientation == RDimensions::ORIENTED ||
	 (!m_bUnoriented &&
	  r.m_nOrientation == RDimensions::UNSPECIFIED));
}

bool Instance::square(const RDimensions& r) const {
  return(r.m_nWidth == r.m_nHeight);
}

URational Instance::minDimPairs(const URational& nMax,
				const DimsFunctor* pDims) const {
  if(empty()) return(URational((UInt) 0));
  if(size() == 1)
    return(front().rotatable(m_bUnoriented) ?
	   front().m_nMinDim : pDims->d2(&(front())));

  URational nMin((UInt) 0);
  DimsFunctor* pRev = pDims->reverse();
  for(const_iterator i = begin(); i != end(); ++i) {

    /**
     * Consider i in its current rotation.
     */

    URational m(pDims->d2(m_nStacked));
    if(pDims->d1(*i) <= nMax) {
      URational n((UInt) 0);
      for(const_iterator j = i + 1; j != end(); ++j) {
	URational p(pDims->d2(m_nStacked));
	if(pDims->d1(*j) <= nMax)
	  p = std::min(p, i->minDim(nMax, *j, pDims, pDims));
	if(j->rotatable(m_bUnoriented) &&
	   pDims->d2(*j) <= nMax)
	  p = std::min(p, i->minDim(nMax, *j, pDims, pRev));
	n = std::max(n, p);
      }
      m = std::min(m, n);
    }

    /**
     * Consider i rotated, if possible.
     */

    if(i->rotatable(m_bUnoriented) &&
       pDims->d2(*i) <= nMax) {
      URational n((UInt) 0);
      for(const_iterator j = i + 1; j != end(); ++j) {
	URational p(pDims->d2(m_nStacked));
	if(pDims->d1(*j) <= nMax)
	  p = std::min(p, i->minDim(nMax, *j, pRev, pDims));
	if(j->rotatable(m_bUnoriented) &&
	   pDims->d2(*j) <= nMax)
	  p = std::min(p, i->minDim(nMax, *j, pRev, pRev));
	n = std::max(n, p);
      }
      m = std::min(m, n);
    }
    nMin = std::max(nMin, m);
  }
  return(nMin);
}

URational Instance::minDimStacked(const URational& nMax,
				  const DimsFunctor* pDims) const {

  /**
   * Sum up the total stack for every rectangle that exceeds the first
   * dimensional constraints.
   */

  URational nMin(0);
  URational nMaxDim(nMax / (URational) 2);
  for(const_iterator i = begin(); i != end(); ++i) {
    if(i->rotatable(m_bUnoriented)) {
      if(pDims->d2(*i) > nMax && pDims->d1(*i) > nMaxDim)
	nMin += pDims->d2(*i);
      else if(pDims->d1(*i) > nMax && pDims->d2(*i) > nMaxDim)
	nMin += pDims->d1(*i);
      else if(i->m_nMinDim > nMaxDim)
	nMin += i->m_nMinDim;
    }
    else if(pDims->d1(*i) > nMaxDim) nMin += pDims->d2(*i);
  }

  /**
   * Now find one rectangle whose first dimension equals exactly half
   * the dimensional constraint. Add in its other dimension.
   */

  for(const_iterator i = begin(); i != end(); ++i)
    if(i->rotatable(m_bUnoriented)) {
      if(pDims->d1(*i) > nMax && pDims->d2(*i) == nMaxDim)
	nMin += pDims->d1(*i);
      else if(pDims->d2(*i) > nMax && pDims->d1(*i) == nMaxDim)
	nMin += pDims->d2(*i);
      else if(i->m_nMinDim == nMaxDim)
	nMin += i->m_nMinDim;
    }
    else if(pDims->d1(*i) == nMaxDim) nMin += pDims->d2(*i);

  return(nMin);
}

URational Instance::minDimStacked2(const URational& nMax,
				   const DimsFunctor* pDims,
				   bool& bOpenInterval) const {
  std::vector<RDimensions> v(size());
  std::copy(begin(), end(), v.begin());
  DimsFunctor* pRev = pDims->reverse();
  
  /**
   * Orient the rectangles depending on the constraint (where
   * applicable), or relax the widths and heights by setting them to
   * the minimum dimension.
   */

  for(std::vector<RDimensions>::iterator i = v.begin();
      i != v.end(); ++i)
    if(i->rotatable(m_bUnoriented)) {
      if(pDims->d1(*i) > nMax)
	i->rotate(); // d1 and d2 can't both exceed nMax.
      else if(pDims->d2(*i) <= nMax)
	i->relax(); // d1 and d2 both fit within nMax.
    }

  /**
   * Now sort in decreasing 2nd dimension, breaking ties by decreasing
   * 1st dimension.
   */

  std::sort(v.begin(), v.end(), DimsGreater(pRev));
  
  URational d2((UInt) 0);
  for(std::vector<RDimensions>::iterator i = v.begin();
      i != v.end(); ++i) {
    d2 += pDims->d1(*i);

    /**
     * If the current width is now greater than our constraint, we
     * should then inspect the last few rectangles we added and derive
     * a second dimension constraint to exclude the current rectangle
     * from being stacked.
     */

    if(d2 > nMax) {
      
      /**
       * To exclude the current rectangle, we simply require the
       * second dimension to be greater than twice the second
       * dimension of this rectangle. There's a caveat here. If there
       * are several rectangles with the same height that are equal to
       * the midpoint, then the interval can be closed. Otherwise, it
       * must be open (so as to not include the last rectangle).
       */

      if(i != v.begin() && pDims->d2(*(i - 1)) != pDims->d2(*i))
	bOpenInterval = true;
      else
	bOpenInterval = false;
      delete pRev;
      return(URational(pDims->d2(*i) * (URational) 2));
    }
  }
  delete pRev;
  return(URational((UInt) 0));
}

void Instance::rotate() {
  for(iterator i = begin(); i != end(); ++i)
    i->rotate();
}

std::ostream& operator<<(std::ostream& os, const Instance& is) {
  if(!is.empty())
    os<< is.front();
  for(UInt i = 1; i < is.size(); ++i)
    os << "," << is[i];
  os << std::flush;
  return(os);
}

Instance& Instance::operator/=(const URational& n) {
  for(iterator i = begin(); i != end(); ++i)
    *i /= n;
  return(*this);
}

Instance& Instance::operator*=(const URational& n) {
  for(iterator i = begin(); i != end(); ++i)
    *i *= n;
  return(*this);
}

void Instance::roundUp() {
  for(iterator i = begin(); i != end(); ++i) {
    i->m_nWidth.ceil();
    i->m_nHeight.ceil();
    i->m_nMinDim = std::min(i->m_nWidth, i->m_nHeight);
    i->m_nArea = i->m_nWidth * i->m_nHeight;
  }
}

void Instance::roundDown() {
  for(iterator i = begin(); i != end(); ++i) {
    i->m_nWidth.floor();
    i->m_nHeight.floor();
    i->m_nMinDim = std::min(i->m_nWidth, i->m_nHeight);
    i->m_nArea = i->m_nWidth * i->m_nHeight;
  }
}
