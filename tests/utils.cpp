#include <libstra/utility.hpp>
#include <vector>

using IntSeq = libstra::make_integer_sequence<int, 3>;
static_assert(
	std::is_same<IntSeq, libstra::integer_sequence<int, 0, 1, 2>>::value,
	"Integer sequence test failed");

static_assert(std::is_same<libstra::make_index_sequence<3>,
						   libstra::index_sequence<0, 1, 2>>::value,
			  "Index sequence test failed");

constexpr int foo(int a, int b) {
	return a + b;
}

int main(int argc, char const *argv[]) {
	constexpr int res = libstra::apply(foo, std::make_tuple(1, 1));

	struct A {
		int val = 0;
		int &operator*() { return val; }
		A &operator++() { return *this; }
	};
	struct B {
		void operator*() {}
		B &operator++() { return *this; }
	};

	static_assert(res == 2, "constexpr apply test failed");
	static_assert(libstra::is_swappable_v<int> &&
					  !libstra::is_swappable_v<void>,
				  "is_swappable test failed");
	static_assert(libstra::is_nothrow_swappable_with<int, int>::value,
				  "nothrow_swappable_with test failed");
	static_assert(
		!libstra::is_legacy_iterator_v<void *> &&
			libstra::is_legacy_iterator_v<int *> &&
			libstra::is_legacy_iterator_v<std::vector<int>::iterator> &&
			libstra::is_legacy_iterator_v<A> &&
			!libstra::is_legacy_iterator_v<B>,
		"is_iterator test failed");
}
