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
	static_assert(!libstra::is_iterator_v<void *> &&
					  libstra::is_iterator_v<int *> &&
					  libstra::is_iterator_v<std::vector<int>::iterator> &&
					  libstra::is_iterator_v<A>,
				  "is_iterator test failed");
	static_assert(libstra::is_void_ptr_v<void *> &&
					  libstra::is_void_ptr_v<const void *> &&
					  !libstra::is_void_ptr_v<int *> &&
					  !libstra::is_void_ptr_v<void>,
				  "is_void_ptr test failed");
	using iter = std::vector<int>::iterator;
	using const_iter = std::vector<int>::const_iterator;
	struct B {
		B &operator++() { return *this; }
		constexpr bool operator==(const B &) const noexcept { return true; }
		constexpr bool operator!=(const B &) const noexcept { return false; }
		B operator++(int) { return *this; }
		int *operator->() { return nullptr; }
		int operator*() { return 0; }
	};
	struct C {
		int v = 0;
		C &operator++() { return *this; }
		constexpr bool operator==(const C &) const noexcept { return true; }
		constexpr bool operator!=(const C &) const noexcept { return false; }
		C operator++(int) { return *this; }
		int *operator->() { return &v; }
		int &operator*() { return v; }
	};

	static_assert(libstra::is_input_iterator_v<int *> &&
					  libstra::is_input_iterator_v<const int *> &&
					  libstra::is_input_iterator_v<iter> &&
					  libstra::is_input_iterator_v<const_iter> &&
					  libstra::is_input_iterator_v<C> &&
					  libstra::is_input_iterator_v<B>,
				  "is_input_iterator test failed");
	static_assert(libstra::is_output_iterator_v<int *> &&
					  libstra::is_output_iterator_v<iter> &&
					  libstra::is_output_iterator_v<C> &&
					  !libstra::is_output_iterator_v<const int *> &&
					  !libstra::is_output_iterator_v<const_iter>,
				  "is_output_iterator test failed");
	static_assert(libstra::is_forward_iterator_v<int *> &&
					  libstra::is_forward_iterator_v<const int *> &&
					  libstra::is_forward_iterator_v<iter> &&
					  libstra::is_forward_iterator_v<const_iter> &&
					  libstra::is_forward_iterator_v<C> &&
					  !libstra::is_forward_iterator_v<B>,
				  "is_forward_iterator test failed");
	using list_iter_t = std::list<int>::iterator;

	static_assert(libstra::is_bidirectional_iterator_v<int *> &&
					  libstra::is_bidirectional_iterator_v<const int *> &&
					  libstra::is_bidirectional_iterator_v<iter> &&
					  libstra::is_bidirectional_iterator_v<const_iter> &&
					  libstra::is_bidirectional_iterator_v<list_iter_t> &&
					  !libstra::is_bidirectional_iterator_v<B> &&
					  !libstra::is_bidirectional_iterator_v<C>,
				  "is_bidirectional_iterator test failed");

	static_assert(libstra::is_random_access_iterator_v<int *> &&
					  libstra::is_random_access_iterator_v<iter> &&
					  libstra::is_random_access_iterator_v<const_iter> &&
					  !libstra::is_random_access_iterator_v<list_iter_t> &&
					  !libstra::is_random_access_iterator_v<C>,
				  "is_random_access_iterator test failed");

	static_assert(libstra::is_contiguous_iterator_v<const int *>,
				  "is_contiguous_iterator test failed");

	static_assert(libstra::is_iterable_v<int[2]> &&
					  libstra::is_iterable_v<libstra::static_vector<A, 3>> &&
					  libstra::is_iterable_v<libstra::array_view<void *>>,
				  "is_iterable test failed");
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
}

template <class T, class = void>
struct has_diff_t : std::false_type {};
template <class T>
struct has_diff_t<T, std::void_t<libstra::difference_type<T>>>
	: std::true_type {};