#include <libstra/views.hpp>
#include <cassert>
#include <iterator>

int main(int argc, char const *argv[]) {
	constexpr int X[] = { 0, 1, 2 };
	libstra::array_view<int> v(std::begin(X), std::end(X));
	assert(!v.is_empty());

	for (size_t i = 0; i < v.size(); i++) {
		assert(v[i] == X[i]);
	}
	auto s = v.sub(1);
	assert(s.size() == 2 && s[0] == 1);
	s = v.sub(0, 2);
	assert(s.size() == 2 && s[0] == 0);

	libstra::array_view<int> v2(X);
	assert(v2.size() == std::extent<decltype(X)>::value);

	return 0;
}
