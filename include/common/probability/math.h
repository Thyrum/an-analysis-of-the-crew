#pragma once

#include "common/util.h"
#include "common/card.h"

#ifdef HAS_BOOST
#	include <boost/math/special_functions/binomial.hpp>
#	include <boost/multiprecision/cpp_int.hpp>
#	include <boost/multiprecision/gmp.hpp>
#endif
#include <array>

#ifdef HAS_BOOST
using bigint = boost::multiprecision::cpp_int;
using bigfloat = boost::multiprecision::mpf_float;
#else
using bigint = long long;
using bigfloat = long double;
#endif


bigint fac(size_t n);

template<typename T>
T choose(int N, int k) {
	if (k > N || N < 0 || k < 0)
		return 0;
#ifdef HAS_BOOST
	return boost::math::binomial_coefficient<T>(N, k);
#else
	constexpr static std::array<std::array<unsigned int,10>,10> lookup = {{
		{1,    0,    0,    0,    0,    0,    0,    0,    0,    0},
		{1,    1,    0,    0,    0,    0,    0,    0,    0,    0},
		{1,    3,    3,    1,    0,    0,    0,    0,    0,    0},
		{1,    4,    6,    4,    1,    0,    0,    0,    0,    0},
		{1,    5,    10,   10,   5,    1,    0,    0,    0,    0},
		{1,    6,    15,   20,   15,   6,    1,    0,    0,    0},
		{1,    7,    21,   35,   35,   21,   7,    1,    0,    0},
		{1,    8,    28,   56,   70,   56,   28,   8,    1,    0},
		{1,    9,    36,   84,   126,  126,  84,   36,   9,    1},
	}};

	if (k > N || k < 0 || N < 0)
		return 0;
	if ((unsigned)N < lookup.size()) {
		return lookup[N][k];
	}
	return fac(N)/(fac(k)*fac(N-k));
#endif
}
