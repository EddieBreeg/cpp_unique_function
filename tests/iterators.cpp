#include <libstra/iterators.hpp>
#include <iterator>
#include <memory>
#include <vector>
#include <list>
#include <libstra/static_vector.hpp>
#include <libstra/views.hpp>

int main(int argc, char const *argv[]) {
	{
		struct A {
			using value_type = double;
			using element_type = int;
		};
		static_assert(!libstra::_details::has_value_type<
						  libstra::indirectly_readable_traits<A>>::value,
					  "indirectly_readable_traits test failed");
		struct B {
			using value_type = int;
			constexpr int operator*() const { return 0; }
		};
		struct C {
			using element_type = float;
			constexpr float operator*() const { return 0.0f; }
		};
		struct D {
			using value_type = double;
			using element_type = double;
			constexpr double operator*() const { return 0.0; }
		};
		static_assert(
			std::is_same<libstra::indirectly_readable_traits<B>::value_type,
						 int>::value &&
				std::is_same<libstra::indirectly_readable_traits<C>::value_type,
							 float>::value &&
				std::is_same<libstra::indirectly_readable_traits<D>::value_type,
							 double>::value,
			"indirectly_readable_traits test failed");
	}
	struct A {
		int val = 0;
		int &operator*() { return val; }
		A &operator++() { return *this; }
	};
	static_assert(!libstra::is_iterator_v<void *> &&
					  libstra::is_iterator_v<int *> &&
					  libstra::is_iterator_v<std::vector<int>::iterator> &&
					  libstra::is_iterator_v<A>,
				  "is_iterator test failed");
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

	static_assert(
		std::is_same<int, libstra::iter_value_t<int *>>::value &&
			std::is_same<int, libstra::iter_value_t<const int *>>::value,
		"iter_value_t test failed");
}
