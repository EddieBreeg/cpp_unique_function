#pragma once

#include <type_traits>
#include <cstddef>
#include <tuple>

namespace libstra {
	struct in_place_t {};
	struct nullopt_t {};

	template <class T>
	[[nodiscard]]
	constexpr T &&forward(std::remove_reference_t<T> &&t) noexcept {
		return (T &&)t;
	}
	template <class T>
	[[nodiscard]]
	constexpr T &&forward(std::remove_reference_t<T> &t) noexcept {
		return (T &&)t;
	}
	constexpr void forward() noexcept {}

	template <class Int, Int... Vals>
	struct integer_sequence {
		static_assert(std::is_integral<Int>::value, "Int must be integral");
	};

	template <class Int, Int N>
	struct integer_constant {
		static_assert(std::is_integral<Int>::value, "Int must be integral");
		static constexpr Int value = N;
	};
	template <bool B>
	using bool_constant = integer_constant<bool, B>;

	template <class Int, class N, Int... S>
	struct _make_int_sequence_helper {
		using type = typename _make_int_sequence_helper<
			Int, integer_constant<Int, N::value - 1>, N::value - 1, S...>::type;
	};
	template <class Int, Int... S>
	struct _make_int_sequence_helper<Int, integer_constant<Int, 0>, S...> {
		using type = integer_sequence<Int, S...>;
	};

	template <class Int, Int N>
	using make_integer_sequence =
		typename _make_int_sequence_helper<Int, integer_constant<Int, N>>::type;

	template <size_t... I>
	using index_sequence = integer_sequence<size_t, I...>;

	template <size_t N>
	using make_index_sequence = make_integer_sequence<size_t, N>;

	template <class F, class Tuple, size_t... I>
	constexpr decltype(auto) apply(index_sequence<I...>, F &&f, Tuple &&args) {
		return f(std::get<I>(std::forward<Tuple>(args))...);
	}
	template <class F, class Tuple>
	constexpr decltype(auto) apply(F &&f, Tuple &&args) {
		using Indices = make_index_sequence<std::tuple_size<Tuple>::value>;
		return apply(Indices{}, std::forward<F>(f), std::forward<Tuple>(args));
	}

	template <class T>
	struct extent {};

	template <class T, size_t N>
	struct extent<T[N]> : integer_constant<size_t, N> {};

	template <class T, size_t N>
	class static_vector;

	template <class T, size_t N>
	struct extent<static_vector<T, N>> : integer_constant<size_t, N> {};

	template <class T>
	static constexpr size_t extent_v = extent<T>::value;

	/**
	 * If T satisfies the requirements of LegacyIterator, declares a static
	 * boolean member equal to true. Otherwise, the same static member is equal
	 * to false. In other words, given an object x of type T, the following
	 * conditions must be met:
	 * - *x must be a valid expression and must not return void
	 * - ++x must be a valid expression which returns an object of type T&
	 * - T must be copy assignable and constructible
	 * - T must be destructible
	 * - T must be swappable
	 */
	template <class T, class = void>
	struct is_legacy_iterator : std::false_type {};

	/**
	 * Given x, an object of type T, dereference_t is an alias for the type of
	 * *x, if and only if *x is a valid expression
	 */
	template <class T>
	using dereference_t = decltype(*std::declval<T>());

	namespace _details {
		template <class T, class U>
		using swap_t =
			decltype(std::swap((T &)std::declval<T>(), (U &)std::declval<U>()));
	} // namespace _details

	template <class T, class U, class = void>
	struct is_swappable_with : std::false_type {};
	template <class T, class U>
	struct is_swappable_with<
		T, U, std::void_t<_details::swap_t<T, U>, _details::swap_t<U, T>>>
		: std::true_type {};

	template <class T, class U>
	static constexpr bool is_swappable_with_v = is_swappable_with<T, U>::value;

	template <class T>
	static constexpr bool is_swappable_v = is_swappable_with_v<T, T>;

	template <class T, class U, class = void>
	struct is_nothrow_swappable_with : std::false_type {};
	template <class T, class U>
	struct is_nothrow_swappable_with<
		T, U, std::enable_if_t<is_swappable_with_v<T, U>>>
		: std::bool_constant<noexcept(std::swap((T &)std::declval<T>(),
												(U &)std::declval<U>())) &&
							 noexcept(std::swap((U &)std::declval<U>(),
												(T &)std::declval<T>()))> {};

	template <class T>
	struct is_legacy_iterator<
		T,
		std::void_t<
			dereference_t<T>, decltype(++(T &)std::declval<T>()),
			std::enable_if_t<
				std::is_copy_constructible<T>::value &&
				std::is_copy_assignable<T>::value &&
				std::is_destructible<T>::value && is_swappable_v<T> &&
				!std::is_void<dereference_t<T>>::value &&
				std::is_same<T &, decltype(++(T &)std::declval<T>())>::value>>>
		: std::true_type {};

	template <class T>
	static constexpr bool is_legacy_iterator_v = is_legacy_iterator<T>::value;
} // namespace libstra
