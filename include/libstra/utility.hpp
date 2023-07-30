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
		return (std::forward<F>(f))(std::get<I>(std::forward<Tuple>(args))...);
	}
	template <class F, class Tuple>
	constexpr decltype(auto) apply(F &&f, Tuple &&args) {
		using Indices = make_index_sequence<std::tuple_size<Tuple>::value>;
		return apply(Indices{}, std::forward<F>(f), std::forward<Tuple>(args));
	}

} // namespace libstra
