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

#include "BinomialCache.h"

BinomialCache::BinomialCache() {
}

BinomialCache::~BinomialCache() {
}

size_t BinomialCache::choose(size_t n, size_t k) {
  iterator i = find(key_type(n, k));
  if(i == end())
    i = insert(value_type(key_type(n, k), chooseAux(n, k))).first;
  return(i->second);
}

size_t BinomialCache::chooseAux(size_t n, size_t k) {
  if(k > n) return 0;
  if(k > n / 2) k = n - k; // faster
  long double nAccumulator(1);
  for(size_t i = 1; i <= k; ++i)
    nAccumulator *= (n - k + i) / i;
  return((size_t) (nAccumulator + 0.5)); // avoid rounding error
}
