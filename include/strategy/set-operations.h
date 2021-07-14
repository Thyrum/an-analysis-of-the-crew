#include <vector>

template<typename T>
std::vector<T> inBoth(const std::vector<T>& v1, const std::vector<T>& v2) {
	std::vector<T> result;
	for (const auto& item : v1) {
		if (std::find(v2.begin(), v2.end(), item) != v2.end())
			result.push_back(item);
	}
	return result;
}

template<typename T>
std::vector<T> inEither(const std::vector<T>& v1, const std::vector<T>& v2) {
	std::vector<T> result = v1;
	for (const auto& item : v2) {
		if (std::find(v1.begin(), v1.end(), item) == v1.end())
			result.push_back(item);
	}
	return result;
}

template<typename T>
std::vector<T> notIn(const std::vector<T>& v1, const std::vector<T>& v2) {
	std::vector<T> result;
	for (const auto& item : v1) {
		if (std::find(v2.begin(), v2.end(), item) == v2.end())
			result.push_back(item);
	}
	return result;
}

template<typename T>
void removeElement(std::vector<T>& v, const T& element) {
	auto it = std::find(v.begin(), v.end(), element);
	if (it != v.end())
		v.erase(it);
}
