#pragma once

#include <type_traits>
#include <iterator>
#include <cstddef>
#include <tuple>

namespace libstra {
	struct in_place_t {};
	struct nullopt_t {};

	/**
	 * Perfectly forwards an object
	 * @tparam T: the type of object to forward
	 * @param t: The object to forward. If T is a (possibly cv-qualified) rvalue
	 * reference, t will be moved. Otherwise, t will be copied
	 * @note If T is void, then forward is a no-op
	 */
	template <class T>
	[[nodiscard]]
	constexpr T &&forward(std::remove_reference_t<T> &&t) noexcept {
		return (T &&)t;
	}
	/**
	 * Perfectly forwards an object
	 * @tparam T: the type of object to forward
	 * @param t: The object to forward. If T is a (possibly cv-qualified) rvalue
	 * reference, t will be moved. Otherwise, t will be copied
	 * @note If T is void, then forward is a no-op
	 */
	template <class T>
	[[nodiscard]]
	constexpr T &&forward(std::remove_reference_t<T> &t) noexcept {
		return (T &&)t;
	}
	/**
	 * Perfectly forwards an object
	 * @tparam T: the type of object to forward
	 * @param t: The object to forward. If T is a (possibly cv-qualified) rvalue
	 * reference, t will be moved. Otherwise, t will be copied
	 * @note If T is void, then forward is a no-op
	 */
	constexpr void forward() noexcept {}

	/**
	 * Represents a sequence of values of type Int. Int is required to be an
	 * integral type
	 */
	template <class Int, Int... Vals>
	struct integer_sequence {
		static_assert(std::is_integral<Int>::value, "Int must be integral");
	};

	/**
	 * Represents an integer constant of type Int. Int must be an integral type
	 */
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

	/**
	 * Helper structure which automatically constructs an integer_sequence with
	 * type Int, made with all values from 0 to N-1
	 */
	template <class Int, Int N>
	using make_integer_sequence =
		typename _make_int_sequence_helper<Int, integer_constant<Int, N>>::type;

	/**
	 * Specialization of integer_sequence with Int = size_t
	 */
	template <size_t... I>
	using index_sequence = integer_sequence<size_t, I...>;

	/**
	 * Specialization of make_integer_sequence with Int = size_t
	 */
	template <size_t N>
	using make_index_sequence = make_integer_sequence<size_t, N>;

	template <class F, class Tuple, size_t... I>
	constexpr decltype(auto) apply(index_sequence<I...>, F &&f, Tuple &&args) {
		return f(std::get<I>(std::forward<Tuple>(args))...);
	}
	/**
	 * Invokes an object using arguments stored in a tuple
	 * @tparam F: The type of invokable object
	 * @tparam Tuple: The type of container which holds the arguments. Any type
	 * for which std::get is implemented is valid, for example: std::tuple,
	 * std::pair, std::array
	 * @param f: The callable object
	 * @param args: The tuple containing the arguments. Said arguments will be
	 * passed using perfect forwarding
	 */
	template <class F, class Tuple>
	constexpr decltype(auto) apply(F &&f, Tuple &&args) {
		using Indices = make_index_sequence<std::tuple_size<Tuple>::value>;
		return apply(Indices{}, std::forward<F>(f), std::forward<Tuple>(args));
	}

	/**
	 * Given x, an object of type T, dereference_t is an alias for the type of
	 * *x, if and only if *x is a valid expression
	 */
	template <class T>
	using dereference_t = decltype(*std::declval<T>());

	/**
	 * true if T is a (possibly cv-qualified) void pointer type
	 */
	template <class T>
	static constexpr bool is_void_ptr_v =
		std::is_pointer<T>::value &&
		std::is_void<std::remove_pointer_t<T>>::value;

	namespace _details {
		template <class T, class U>
		using swap_t =
			decltype(std::swap(std::declval<std::add_lvalue_reference_t<T>>(),
							   std::declval<std::add_lvalue_reference_t<U>>()));
		template <class T>
		using preinc_t =
			decltype(++std::declval<std::add_lvalue_reference_t<T>>());
		template <class T>
		using postinc_t =
			decltype((std::declval<std::add_lvalue_reference_t<T>>())++);
	} // namespace _details

	/**
	 * If given x and y, two objects of type T and U respecitively,
	 * std::swap(x, y) and std::swap(y, x) are valid expressions, declares
	 * a static boolean constant equal to true. Otherwise, this constant is
	 * equal to false
	 */
	template <class T, class U, class = void>
	struct is_swappable_with : std::false_type {};

	/**
	 * If given x and y, two objects of type T and U respecitively,
	 * std::swap(x, y) and std::swap(y, x) are valid expressions, declares
	 * a static boolean constant equal to true. Otherwise, this constant is
	 * equal to false
	 */
	template <class T, class U>
	struct is_swappable_with<
		T, U, std::void_t<_details::swap_t<T, U>, _details::swap_t<U, T>>>
		: std::true_type {};

	template <class T, class U>
	static constexpr bool is_swappable_with_v = is_swappable_with<T, U>::value;

	template <class T>
	static constexpr bool is_swappable_v = is_swappable_with_v<T, T>;

	/** Given x and y, two objects of type T and U respectively, declares a
	 * static boolean constant equal to true if and only if:
	 * - std::swap(x, y) and std::swap(y, x) are both valid expressions
	 * - Neither std::swap(x, y) or std::swap(y, x) throws an exception
	 */
	template <class T, class U, class = void>
	struct is_nothrow_swappable_with : std::false_type {};
	/** Given x and y, two objects of type T and U respectively, declares a
	 * static boolean constant equal to true if and only if:
	 * - std::swap(x, y) and std::swap(y, x) are both valid expressions
	 * - Neither std::swap(x, y) or std::swap(y, x) throws an exception
	 */
	template <class T, class U>
	struct is_nothrow_swappable_with<
		T, U, std::enable_if_t<is_swappable_with_v<T, U>>>
		: bool_constant<noexcept(std::swap(
							std::declval<std::add_lvalue_reference_t<T>>(),
							std::declval<std::add_lvalue_reference_t<U>>())) &&
						noexcept(std::swap(
							std::declval<std::add_lvalue_reference_t<U>>(),
							std::declval<std::add_lvalue_reference_t<T>>()))> {
	};

	namespace _details {
		template <class T>
		using eq_t = decltype(std::declval<T>() == std::declval<T>());
		template <class T>
		using neq_t = decltype(std::declval<T>() != std::declval<T>());

		template <class T, class = void>
		struct member_access {
			using type = decltype(std::declval<T>().operator->());
		};
		template <class T>
		struct member_access<T, std::enable_if_t<std::is_pointer<T>::value &&
												 !is_void_ptr_v<T>>> {
			using type = T;
		};
		template <class T>
		using member_access_t = typename member_access<T>::type;

		template <class T, class U, class = void>
		struct is_eq_comp_t : std::false_type {};
		template <class T, class U>
		struct is_eq_comp_t<
			T, U, std::void_t<decltype(std::declval<T>() == std::declval<U>())>>
			: std::true_type {};

		template <class T>
		using predec_t =
			decltype(--std::declval<std::add_lvalue_reference_t<T>>());
		template <class T>
		using postdec_t =
			decltype((std::declval<std::add_lvalue_reference_t<T>>())--);

	} // namespace _details

	/**
	 * If the type T is a reference type, remove_cv_ref_t is a typedef
	 * which is the type referred to by T with its topmost cv-qualifiers
	 * removed. Otherwise type is T with its topmost cv-qualifiers removed.
	 */
	template <class T>
	using remove_cv_ref_t = std::remove_cv_t<std::remove_reference_t<T>>;

	namespace _details {
		template <class T>
		using sub_t = decltype(std::declval<T>() - std::declval<T>());

		template <class T, class = void>
		struct is_subtractible : std::false_type {};

		template <class T>
		struct is_subtractible<T,
							   std::enable_if_t<!std::is_void<sub_t<T>>::value>>
			: std::true_type {};

		template <class T, bool = false, class = void>
		struct diff_t {};

		template <class T>
		struct diff_t<T, false, std::void_t<typename T::difference_type>> {
			using type = typename T::difference_type;
		};
		template <class T>
		struct diff_t<T, true, std::void_t<sub_t<T>>> {
			using type = sub_t<T>;
		};

		template <class T>
		using difference_type =
			typename diff_t<T, is_subtractible<T>::value>::type;

		template <class T, class = void>
		struct inc_traits {};

		template <class T>
		struct inc_traits<
			T, std::enable_if_t<std::is_integral<difference_type<T>>::value>> {
			using difference_type = difference_type<T>;
		};

	} // namespace _details

	/**
	 * If T has an integral member type called difference_type,
	 * incrementable_traits also declares such a member.
	 * Otherwise, given x and y two objects of type T, incrementable_traits
	 * declares the difference_type member type if and only if x - y is a valid
	 * expression which returns an integer.
	 *
	 * In all other cases, incrementable_traits is an empty struct.
	 */
	template <class T>
	struct incrementable_traits : _details::inc_traits<T> {};

	/**
	 * If T has an integral member type called difference_type,
	 * incrementable_traits also declares such a member.
	 * Otherwise, given x and y two objects of type T, incrementable_traits
	 * declares the difference_type member type if and only if x - y is a valid
	 * expression which returns an integer.
	 *
	 * In all other cases, incrementable_traits is an empty struct.
	 */
	template <class T>
	struct incrementable_traits<const T> : incrementable_traits<T> {};

	template <class T>
	/**
	 * Type alias for incrementable_traits<remove_cv_ref_t<T>>::difference_type
	 */
	using difference_type =
		typename incrementable_traits<remove_cv_ref_t<T>>::difference_type;

	namespace _details {

		template <class T, class = void>
		struct totally_ordered : std::false_type {};
		template <class T>
		struct totally_ordered<
			T, std::void_t<eq_t<T>, neq_t<T>,
						   decltype(std::declval<T>() < std::declval<T>()),
						   decltype(std::declval<T>() <= std::declval<T>()),
						   decltype(std::declval<T>() >= std::declval<T>()),
						   decltype(std::declval<T>() > std::declval<T>())>>
			: std::true_type {};

		template <class T, class = void>
		struct is_weakly_inc : std::false_type {};
		template <class T>
		struct is_weakly_inc<
			T, std::void_t<
				   postinc_t<T>,
				   std::enable_if_t<
					   std::is_same<preinc_t<T>, T &>::value &&
					   std::is_signed<libstra::difference_type<T>>::value>>>
			: std::true_type {};
		template <class T, class = void>
		struct is_weakly_dec : std::false_type {};
		template <class T>
		struct is_weakly_dec<
			T, std::void_t<
				   postdec_t<T>,
				   std::enable_if_t<
					   std::is_same<predec_t<T>, T &>::value &&
					   std::is_signed<libstra::difference_type<T>>::value>>>
			: std::true_type {};

		template <class T, class = void>
		struct advanceable : std::false_type {};
		template <class T>
		struct advanceable<
			T,
			std::void_t<
				decltype(T(std::declval<T>() +
						   std::declval<libstra::difference_type<T>>())),
				decltype(T(std::declval<libstra::difference_type<T>>() +
						   std::declval<T>())),
				decltype(T(std::declval<T>() -
						   std::declval<libstra::difference_type<T>>())),
				std::enable_if_t<
					totally_ordered<T>::value && is_weakly_inc<T>::value &&
					is_weakly_dec<T>::value &&
					std::is_same<
						T &,
						decltype(std::declval<
									 std::add_lvalue_reference_t<T>>() +=
								 std::declval<difference_type<T>>())>::value &&
					std::is_same<
						T &,
						decltype(std::declval<
									 std::add_lvalue_reference_t<T>>() -=
								 std::declval<difference_type<T>>())>::value>>>
			: std::true_type {};
	} // namespace _details

	template <class T>
	static constexpr bool is_totally_ordered_v =
		_details::totally_ordered<T>::value;

#if defined(_MSC_VER) || defined(__GNUC__) || defined(__clang__)
	template <class T>
	// Obtains the actual address of the object or function arg, even in
	// presence of overloaded operator&
	constexpr T *addressof(T &arg) noexcept {
		return __builtin_addressof(arg);
	}
#else
	// Obtains the actual address of the object or function arg, even in
	// presence of overloaded operator&
	template <class T>
	typename std::enable_if<std::is_object<T>::value, T *>::type
	addressof(T &arg) noexcept {
		return reinterpret_cast<T *>(
			&const_cast<char &>(reinterpret_cast<const volatile char &>(arg)));
	}

	// Obtains the actual address of the object or function arg, even in
	// presence of overloaded operator&
	template <class T>
	typename std::enable_if<!std::is_object<T>::value, T *>::type
	addressof(T &arg) noexcept {
		return &arg;
	}
#endif

	namespace _details {
		template <class F, class... Args>
		using invoke_t = decltype(std::declval<F>()(std::declval<Args>()...));

		template <class... Args>
		struct Pack_t {};

		template <class F, class Args, class = void>
		struct invokable_with : std::false_type {};

		template <class F, class... Args>
		struct invokable_with<F, Pack_t<Args...>,
							  std::void_t<invoke_t<F, Args...>>>
			: std::true_type {};

	} // namespace _details

	template <class T, class U>
	static constexpr bool is_equality_comparable_v =
		_details::is_eq_comp_t<T, U>::value;

	template <class T, class... Args>
	struct is_invocable
		: _details::invokable_with<T, _details::Pack_t<Args...>> {};

	template <class T, class... Args>
	static constexpr bool is_invocable_v = is_invocable<T, Args...>::value;

	template <class T>
	struct is_cv_qualified : std::false_type {};

	template <class T>
	struct is_cv_qualified<const T> : std::true_type {};
	template <class T>
	struct is_cv_qualified<volatile T> : std::true_type {};
	template <class T>
	struct is_cv_qualified<const volatile T> : std::true_type {};

	template <class T>
	static constexpr bool is_cv_qualified_v = is_cv_qualified<T>::value;

	template <class T>
	static constexpr bool is_weakly_incrementable_v =
		_details::is_weakly_inc<T>::value;
	template <class T>
	static constexpr bool is_weakly_decrementable_v =
		_details::is_weakly_dec<T>::value;

	template <class T>
	static constexpr bool is_copyable_v =
		std::is_copy_assignable<T>::value &&
		std::is_move_assignable<T>::value &&
		std::is_copy_constructible<T>::value &&
		std::is_move_constructible<T>::value;

	template <class T>
	static constexpr bool is_semiregular_v =
		std::is_default_constructible<T>::value && is_copyable_v<T>;
	template <class T>
	static constexpr bool is_regular_v =
		is_semiregular_v<T> && is_equality_comparable_v<T, T>;

	namespace _details {
		template <class T, class = void>
		struct incrementable : std::false_type {};
		template <class T>
		struct incrementable<
			T, std::enable_if_t<
				   is_regular_v<T> && is_weakly_incrementable_v<T> &&
				   std::is_same<decltype(std::declval<T>()++), T>::value>>
			: std::true_type {};

		template <class T, class = void>
		struct decrementable : std::false_type {};
		template <class T>
		struct decrementable<
			T, std::enable_if_t<
				   incrementable<T>::value && is_weakly_incrementable_v<T> &&
				   std::is_same<decltype(std::declval<T>()--), T>::value>>
			: std::true_type {};

	} // namespace _details

	template <class T>
	static constexpr bool is_incrementable_v =
		_details::incrementable<T>::value;
	template <class T>
	static constexpr bool is_decrementable_v =
		_details::decrementable<T>::value;

	template <class T>
	struct type_identity {
		using type = T;
	};
} // namespace libstra
