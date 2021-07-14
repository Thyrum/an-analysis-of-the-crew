#include "common/util.h"

unsigned toInt(const char* begin, const char* end) {
	unsigned result = 0;
	while (begin != end) {
		if (*begin >= '0' && *begin <= '9')
			result = result * 10 + (*begin - '0');
		++begin;
	}
	return result;
}
