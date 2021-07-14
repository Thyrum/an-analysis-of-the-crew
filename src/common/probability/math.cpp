#include "common/probability/math.h"

bigint fac(size_t n) {
	constexpr static std::array<unsigned long,21> lookup = {{
		1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880, 3628800, 39916800,
		479001600, 6227020800,87178291200, 1307674368000,20922789888000,
		355687428096000,6402373705728000,121645100408832000,2432902008176640000
	}};

	if (n < 0)
		return 0;
	if ((unsigned)n < lookup.size())
		return lookup[n];

	bigint result = lookup[lookup.size()-1];
	for (size_t i = lookup.size(); i <= n; ++i) {
		result *= i;
	}
	return result;
}
