#include <libstra/range.hpp>
#include <vector>
#include <cassert>

template <class T>
class array_view : public libstra::view_interface<array_view<T>> {
	T *_start = nullptr, *_end = nullptr;

public:
	constexpr array_view(T *start, T *end) : _start(start), _end(end) {}
	constexpr T *begin() { return _start; }
	constexpr T *end() { return _end; }
	constexpr const T *begin() const { return _start; }
	constexpr const T *end() const { return _end; }
};

int main(int argc, char const *argv[]) {
	constexpr libstra::empty_view<int> v;
	static_assert(libstra::is_forward_range_v<decltype(v)>,
				  "Range test failed");
	static_assert(!libstra::is_range_v<nullptr_t>, "Range test failed");
	static_assert(v.size() == 0, "Range test failed");
	static_assert(v.begin() == nullptr && v.end() == nullptr,
				  "Range test failed");
	static_assert(v.data() == nullptr && v.end() == nullptr,
				  "Range test failed");
	static_assert(libstra::is_range_v<std::vector<int>>, "Range test failed");

	static_assert(std::is_same<libstra::iterator_t<std::vector<int>>,
							   std::vector<int>::iterator>::value,
				  "Range test failed");

	static_assert(std::is_same<int &, decltype(v.front())>::value,
				  "Range test failed");
	int a[] = { 0, 1, 2 };
	array_view<int> av(a + 0, std::end(a));
	static_assert(libstra::is_random_access_range_v<decltype(av)>,
				  "Range test failed");
	assert(!av.is_empty());
	assert(av.front() == 0);
	assert(av.back() == 2);
	for (size_t i = 0; i < std::extent<decltype(a)>::value; i++) {
		assert(a[i] == av[i]);
	}

	return 0;
}
