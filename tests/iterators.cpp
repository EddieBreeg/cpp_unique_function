#include <libstra/iterators.hpp>
#include <iterator>
#include <memory>
#include <vector>
#include <list>
#include <libstra/static_vector.hpp>
#include <libstra/views.hpp>
#include <cassert>

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
	static_assert(
		std::is_same<int &, libstra::iter_reference_t<int *>>::value &&
			std::is_same<const int &,
						 libstra::iter_reference_t<const int *>>::value,
		"iter_reference_t test failed");
	static_assert(
		std::is_same<libstra::iter_difference_t<int *>, std::ptrdiff_t>::value,
		"iter_difference_t test failed");

	{
		using const_iter_t = libstra::basic_const_iterator<int *>;
		static_assert(std::is_same<std::random_access_iterator_tag,
								   const_iter_t::iterator_category>::value,
					  "basic_const_iterator test failed");
		constexpr const_iter_t it;
		static_assert(std::is_same<decltype(*it), const int &>::value,
					  "basic_const_iterator test failed");
	}
	{
		struct Foo {
			int x = 0;
		};
		using const_iter_t = libstra::basic_const_iterator<Foo *>;
		static_assert(
			libstra::_details::has_member_access_method<const_iter_t>::value,
			"basic_const_iterator test failed");
		constexpr const_iter_t iter;
		static_assert(iter == nullptr, "basic_const_iterator test failed");
		static_assert(
			std::is_same<const Foo *, decltype(iter.operator->())>::value,
			"basic_const_iterator test failed");
		static_assert(
			std::is_same<const_iter_t, decltype(iter + 0)>::value &&
				std::is_same<const_iter_t, decltype(iter - 0)>::value &&
				std::is_same<const Foo &, decltype(iter[0])>::value,
			"basic_const_iterator test failed");
	}
	{
		struct Foo : std::iterator_traits<int *> {
			int *_ptr;
			Foo() = delete;
			constexpr Foo(int *ptr) : _ptr(ptr) {}
			constexpr reference operator*() { return *_ptr; }
			constexpr pointer operator->() { return _ptr; }
			constexpr Foo &operator++() {
				++_ptr;
				return *this;
			}
			constexpr Foo operator++(int) { return _ptr++; }
			constexpr bool operator==(const Foo &other) const {
				return _ptr == other._ptr;
			}
			constexpr bool operator!=(const Foo &other) const {
				return _ptr != other._ptr;
			}
		};
		using const_iter_t = libstra::basic_const_iterator<Foo>;
		constexpr const_iter_t it = nullptr;
	}
	{
		constexpr int X[] = { 0, 1, 12 };
		using RIter = libstra::basic_reverse_iterator<decltype(std::end(X))>;
		RIter it = std::end(X);
		assert(it == std::end(X));
		assert((it + 1) == (std::end(X) - 1));
		++it;
		assert(it[2] == 0);
		assert(it < RIter(std::begin(X)));
	}
}
