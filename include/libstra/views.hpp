#pragma once

#include <cstddef>
#include "utility.hpp"
#include <iterator>

namespace libstra {

	/**
	 * A lightweight object which provides a read-only interface for a
	 * contiguous array
	 * @tparam T: The type of elements contained in the referenced array
	 */
	template <class T>
	class array_view {
	private:
		const T *_start = nullptr, *_end = nullptr;

	public:
		using value_type = T;
		using reference = const T &;
		using const_reference = reference;
		using pointer = const T *;
		using iterator = pointer;
		using const_iterator = iterator;
		using difference_type =
			decltype(std::declval<pointer>() - std::declval<pointer>());
		using size_type = size_t;

		/**
		 * Constructs an empty view
		 */
		constexpr array_view() noexcept = default;
		/**
		 * Constructs a view from a data pointer and a size
		 * @param ptr: The pointer to the data. If the pointer is null or
		 * doesn't point to a valid data block, the behaviour of the program is
		 * undefined
		 * @param n: The value returned by size() after construction
		 */
		constexpr array_view(const T *ptr, size_t n) noexcept :
			_start(ptr), _end(ptr + n) {}
		/**
		 * Constructs a view from a pair of iterators
		 * @tparam Iter: The iterator type
		 * @param begin: An iterator to the first element in the data array
		 * @param end: An iterator to past the last element in the data array
		 * @warning If end is not reachable from begin, the behaviour is
		 * undefined
		 */
		template <class Iter,
				  class = std::enable_if_t<is_random_access_iterator_v<Iter>>>
		constexpr array_view(Iter begin, Iter end) :
			_start(&(*begin)), _end(&(*end)) {}
		template <class Iterable,
				  std::enable_if_t<
					  is_iterable_v<Iterable> &&
						  is_contiguous_iterator_v<_details::begin_t<Iterable>>,
					  int> = 0>
		/**
		 * Constructs a view from a generic iterable object. This constructor
		 * only participates in overload resolution if Iterable is a contiguous
		 * iterable object
		 */
		constexpr array_view(const Iterable &x) :
			_start(std::begin(x)), _end(std::end(x)) {}

		/**
		 * Accesses an element
		 */
		[[nodiscard]]
		constexpr const T &
		operator[](size_t i) const {
			return _start[i];
		}
		/**
		 * Returns a raw pointer to the underlying data. If the view was
		 * constructed with the default constructor, this function returns
		 * nullptr
		 */
		[[nodiscard]]
		constexpr const T *data() const noexcept {
			return _start;
		}
		/**
		 * Returns a reference to the first object in the array.
		 * Behaviour is undefined if the view is empty, or doesn't point to a
		 * valid data array
		 */
		[[nodiscard]]
		constexpr const T &front() const {
			return *_start;
		}
		/**
		 * Returns a reference to the last object in the array.
		 * Behaviour is undefined if the view is empty, or doesn't point to a
		 * valid data array
		 */
		[[nodiscard]]
		constexpr const T &back() const {
			return *(_end - 1);
		}
		/**
		 * Returns an iterator to the first element of the view
		 */
		[[nodiscard]]
		constexpr const T *begin() const noexcept {
			return _start;
		}
		/**
		 * Returns an iterator to past the last element of the view
		 */
		[[nodiscard]]
		constexpr const T *end() const noexcept {
			return _end;
		}
		[[nodiscard]]
		/**
		 * Returns end() - begin(), the number of elements in the view
		 */
		constexpr size_t size() const noexcept {
			return _end - _start;
		}
		/**
		 * Returns true if size() == 0, false otherwise
		 */
		[[nodiscard]]
		constexpr bool is_empty() const noexcept {
			return !size();
		}
		/**
		 * Returns a view of the subarray [data(), data() + rcount) where rcount
		 * is minimum between n and size() - start
		 * @param start: The index of the first element in the subarray
		 * @param n: The maximum number of elements in the subarray
		 */
		[[nodiscard]]
		constexpr array_view sub(size_t start,
								 size_t n = (size_t)-1) const noexcept {
			return array_view(_start + start,
							  n < (size() - start) ? _start + n : _end);
		}
	};
} // namespace libstra
