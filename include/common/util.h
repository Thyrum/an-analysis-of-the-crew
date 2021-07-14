#pragma once

#include <algorithm>
#include <vector>
#include <functional>
#include <iostream>

unsigned toInt(const char* begin, const char* end);

namespace std {

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec) {
	if (vec.size() == 0)
		os << "[]";
	else {
		os << '[' << vec[0];
		for (typename std::vector<T>::size_type i = 1; i < vec.size(); ++i)
			os << ',' << vec[i];
		os << ']';
	}
	return os;
}

}
