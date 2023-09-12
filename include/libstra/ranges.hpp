#pragma once

#include "utility.hpp"

namespace libstra {
	namespace _details {
		template <class T, class = void>
		struct has_begin_method : std::false_type {};
		template <class T>
		struct has_begin_method<
			T, std::void_t<decltype(std::declval<T>().begin())>>
			: std::true_type {
			using begin_type = decltype(std::declval<T>().begin());
		};

		template <class T, class = void>
		struct has_end_method : std::false_type {};

		template <class T>
		struct has_end_method<T, std::void_t<decltype(std::declval<T>().end())>>
			: std::true_type {
			using end_type = decltype(std::declval<T>().end());
		};

	} // namespace _details

	namespace ranges {
		template <
			class T,
			std::enable_if_t<std::is_array<remove_cv_ref_t<T>>::value, int> = 0>
		constexpr auto begin(T &&x) {
			return x + 0;
		}
		template <class T, std::enable_if_t<
							   _details::has_begin_method<T>::value, int> = 0>
		constexpr auto begin(T &&x) {
			using R = typename _details::has_begin_method<T>::begin_type;
			return std::forward<std::decay_t<R>>(x.begin());
		}
		template <
			class T,
			std::enable_if_t<std::is_array<remove_cv_ref_t<T>>::value, int> = 0>
		constexpr auto end(T &&x) {
			return x + std::extent<remove_cv_ref_t<T>>::value;
		}
		template <class T,
				  std::enable_if_t<_details::has_end_method<T>::value, int> = 0>
		constexpr auto end(T &&x) {
			using R = typename _details::has_end_method<T>::end_type;
			return std::forward<std::decay_t<R>>(x.end());
		}

		template <class T>
		using iterator_t =
			decltype(begin(std::declval<std::add_lvalue_reference_t<T>>()));

		template <class T>
		using const_iterator_t = iterator_t<std::add_const_t<T>>;

		template <class T>
		using sentinel_t =
			decltype(end(std::declval<std::add_lvalue_reference_t<T>>()));

	} // namespace ranges

	namespace _details {
		namespace ranges = ::libstra::ranges;

		template <class T, class = void>
		struct is_range : std::false_type {};

		template <class T>
		struct is_range<
			T, std::void_t<ranges::iterator_t<T>, ranges::sentinel_t<T>>>
			: std::true_type {};
	} // namespace _details

	namespace ranges {
		template <class T>
		static constexpr bool is_range_v = _details::is_range<T>::value;
	} // namespace ranges

} // namespace libstra
