#include <libstra/utility.hpp>
#include <libstra/static_vector.hpp>
#include <libstra/views.hpp>
#include <vector>
#include <list>

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

	static_assert(res == 2, "constexpr apply test failed");
	static_assert(libstra::is_swappable_v<int> &&
					  !libstra::is_swappable_v<void> &&
					  libstra::is_swappable_v<std::vector<int>::iterator>,
				  "is_swappable test failed");

	static_assert(libstra::is_nothrow_swappable_with<int, int>::value,
				  "nothrow_swappable_with test failed");

	static_assert(libstra::is_void_ptr_v<void *> &&
					  libstra::is_void_ptr_v<const void *> &&
					  !libstra::is_void_ptr_v<int *> &&
					  !libstra::is_void_ptr_v<void>,
				  "is_void_ptr test failed");

	struct Foo {
		int operator-(Foo) { return 0; }
		using difference_type = long long;
	};
	struct Bar {
		using difference_type = long long;
	};
	static_assert(
		std::is_same<libstra::difference_type<Foo>, int>::value &&
			std::is_same<libstra::difference_type<Bar>, long long>::value &&
			std::is_same<std::ptrdiff_t,
						 libstra::difference_type<int *>>::value,
		"difference_type test failed");
	static_assert(libstra::is_invocable_v<int(int), int> &&
					  libstra::is_invocable_v<int (*)()> &&
					  !libstra::is_invocable_v<int(), int> &&
					  !libstra::is_invocable_v<void(int)>,
				  "is_invocable test failed");

	static_assert(libstra::is_weakly_incrementable_v<int> &&
					  libstra::is_weakly_incrementable_v<int *> &&
					  !libstra::is_weakly_incrementable_v<void *> &&
					  !libstra::is_weakly_incrementable_v<double>,
				  "weakly incrementable test failed");
	{
		struct D {
			D(int){};
		};
		static_assert(libstra::is_semiregular_v<bool> &&
						  !libstra::is_semiregular_v<D>,
					  "semiregular test failed");
	}
	static_assert(
		libstra::_details::advanceable<std::vector<int>::iterator>::value &&
			libstra::_details::advanceable<const int *>::value,
		"advanceable trait test failed");
}

template <class T, class = void>
struct has_diff_t : std::false_type {};
template <class T>
struct has_diff_t<T, std::void_t<libstra::difference_type<T>>>
	: std::true_type {};

static_assert(has_diff_t<std::list<int>::iterator>::value,
			  "difference_type test failed");