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

	/**
	 * If T satisfies the requirements of LegacyIterator, declares a static
	 * boolean constant equal to true. Otherwise, the same static
	 * member is equal to false. In other words, given an object x of type T,
	 * the following conditions must be met:
	 * - *x must be a valid expression and must not return void
	 * - ++x must be a valid expression which returns an object of type T&
	 * - T must be copy assignable and constructible
	 * - T must be destructible
	 * - T must be swappable
	 */
	template <class T, class = void>
	struct is_iterator : std::false_type {};

	/**
	 * If T satisfies the requirements of LegacyIterator, declares a static
	 * boolean constant equal to true. Otherwise, the same static
	 * member is equal to false. In other words, given an object x of type T,
	 * the following conditions must be met:
	 * - *x must be a valid expression and must not return void
	 * - ++x must be a valid expression which returns an object of type T&
	 * - T must be copy assignable and constructible
	 * - T must be destructible
	 * - T must be swappable
	 */
	template <class T>
	struct is_iterator<
		T,
		std::void_t<dereference_t<T>, _details::preinc_t<T>,
					std::enable_if_t<
						std::is_copy_constructible<T>::value &&
						std::is_copy_assignable<T>::value &&
						std::is_destructible<T>::value && is_swappable_v<T> &&
						std::is_same<T &, _details::preinc_t<T>>::value>>>
		: std::true_type {};

	template <class T>
	static constexpr bool is_iterator_v = is_iterator<T>::value;

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

	} // namespace _details

	template <class T, class = void>
	/**
	 * Let x and y be two objects of type T, and m a member of the object type
	 * pointed to by T. is_input_iterator declares a static boolean constant
	 * equal to true if and only if:
	 * - T is an iterator type
	 * - x == y and x != y are both valid expressions
	 * - x++ is a valid expression
	 * - x->m is a valid expression
	 */
	struct is_input_iterator : std::false_type {};

	template <class T>
	/**
	 * Let x and y be two objects of type T, and m a member of the object type
	 * pointed to by T. is_input_iterator declares a static boolean constant
	 * equal to true if and only if:
	 * - T is an iterator type
	 * - x == y and x != y are both valid expressions
	 * - x++ is a valid expression, which returns an object of type T&
	 * - x->m is a valid expression
	 */
	struct is_input_iterator<
		T, std::void_t<std::enable_if_t<
						   is_iterator_v<T> &&
						   std::is_same<T &, _details::preinc_t<T>>::value>,
					   _details::eq_t<T>, _details::neq_t<T>,
					   _details::member_access_t<T>>> : std::true_type {};

	template <class T>
	static constexpr bool is_input_iterator_v = is_input_iterator<T>::value;

	template <class T, class = void>
	/**
	 * Let x be an object of type T, and y an object of type value_type where
	 * value_type is dereference_t<T>. is_output_iterator declares a static
	 * boolean constant equal to true if and only if:
	 * - T is an iterator
	 * - *x = y is a valid expression. x doesn't have to be de referenceable
	 * after the write expression
	 * - ++x is a valid expression, which returns an object of type T&
	 * - x++ is a valid expression, the result of which is convertible to const
	 * T&
	 */
	struct is_output_iterator : std::false_type {};

	template <class T>
	/**
	 * Let x be an object of type T, and y an object of type value_type where
	 * value_type is dereference_t<T>. is_output_iterator declares a static
	 * boolean constant equal to true if and only if:
	 * - T is an iterator
	 * - *x = y is a valid expression. x doesn't have to be de referenceable
	 * after the write expression
	 * - ++x is a valid expression, which returns an object of type T&
	 * - x++ is a valid expression, the result of which is convertible to const
	 * T&
	 */
	struct is_output_iterator<
		T,
		std::void_t<
			std::enable_if_t<
				is_iterator_v<T> &&
				std::is_convertible<_details::postinc_t<T>, const T &>::value &&
				std::is_same<T &, _details::preinc_t<T>>::value>,
			decltype(*std::declval<T>() = std::declval<dereference_t<T>>())>>
		: std::true_type {};

	template <class T>
	static constexpr bool is_output_iterator_v = is_output_iterator<T>::value;

	template <class T>
	static constexpr bool is_input_output_iterator_v =
		is_input_iterator_v<T> && is_output_iterator_v<T>;

	namespace _details {
		template <class T>
		using predec_t =
			decltype(--std::declval<std::add_lvalue_reference_t<T>>());
		template <class T>
		using postdec_t =
			decltype((std::declval<std::add_lvalue_reference_t<T>>())--);
	} // namespace _details
	template <class T, class = void>

	/**
	 * Let x be an object of type T. is_forward_iterator declares a static
	 * boolean constant equal to true if and only if:
	 * - is_input_iterator_v<T> is true
	 * - T is default constructible
	 * - *x is a valid expression, and returns a reference
	 */
	struct is_forward_iterator : std::false_type {};

	template <class T>
	/**
	 * Let x be an object of type T. is_forward_iterator declares a static
	 * boolean constant equal to true if and only if:
	 * - is_input_iterator_v<T> is true
	 * - T is default constructible
	 * - *x is a valid expression, and returns a reference
	 */
	struct is_forward_iterator<
		T,
		std::void_t<std::enable_if_t<
			is_input_iterator_v<T> && std::is_default_constructible<T>::value &&
			std::is_reference<dereference_t<T>>::value>>> : std::true_type {};

	template <class T>
	static constexpr bool is_forward_iterator_v = is_forward_iterator<T>::value;

	template <class T, class = void>
	/**
	 * Let x be an object of type T. is_forward_iterator declares a static
	 * boolean constant equal to true if and only if:
	 * - is_forward_iterator_v<T> is true
	 * - ++x is a valid expression, which returns an object of type T&
	 * - x++ is a valid expression, the result of which is convertible to const
	 * T&
	 * - *x is a valid expression, and returns a reference
	 */
	struct is_bidirectional_iterator : std::false_type {};

	template <class T>
	/**
	 * Let x be an object of type T. is_forward_iterator declares a static
	 * boolean constant equal to true if and only if:
	 * - is_forward_iterator_v<T> is true
	 * - ++x is a valid expression, which returns an object of type T&
	 * - x++ is a valid expression, the result of which is convertible to const
	 * T&
	 * - *x is a valid expression, and returns a reference
	 */
	struct is_bidirectional_iterator<
		T, std::enable_if_t<
			   is_forward_iterator_v<T> &&
			   std::is_same<T &, _details::predec_t<T>>::value &&
			   std::is_convertible<const T &, _details::postdec_t<T>>::value>>
		: std::true_type {};

	template <class T>
	static constexpr bool is_bidirectional_iterator_v =
		is_bidirectional_iterator<T>::value;

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

	} // namespace _details

	template <class T>
	/**
	 * Let x and y be two objects of type T. If x - y is a valid expression
	 * which doesn't return void, then difference_type<T> = decltype(x - y).
	 * Else, if T declares a member type called difference_type, then
	 * difference_type<T> = typename T::difference_type. Otherwise,
	 * difference_type<T> does not represent a valid type
	 */
	using difference_type =
		typename _details::diff_t<T, _details::is_subtractible<T>::value>::type;

	template <class T, class = void>
	/**
	 * Let x and y be two objects of type T, and n an object of type
	 * difference_type<T>. is_random_access_iterator declares a static boolean
	 * constant equal to true if and only if:
	 * - is_bidirectional_iterator_v<T> is true
	 * - x += n is a valid expression, and returns an object of type T&
	 * - x -= n is a valid expression, and returns an object of type T&
	 * - x + n and n + x are valid expressions, and return an object of type T
	 * - x - n is a valid expression, which returns an object of type T
	 * - x - y is a valid expression, which returns an object of type
	 * difference_type<T>
	 * - dereference_t<T> is a reference type
	 * - x[n] is a valid expression, which returns a value of type
	 * dereference_t<T>
	 * - x < y is a valid expression, the result of which is convertible to bool
	 * - x > y is a valid expression, the result of which is convertible to bool
	 * - x <= y is a valid expression, the result of which is convertible to
	 * bool
	 * - x >= y is a valid expression, the result of which is convertible to
	 * bool
	 */
	struct is_random_access_iterator : std::false_type {};

	template <class T>
	/**
	 * Let x and y be two objects of type T, and n an object of type
	 * difference_type<T>. is_random_access_iterator declares a static boolean
	 * constant equal to true if and only if:
	 * - is_bidirectional_iterator_v<T> is true
	 * - x += n is a valid expression, and returns an object of type T&
	 * - x -= n is a valid expression, and returns an object of type T&
	 * - x + n and n + x are valid expressions, and return an object of type T
	 * - x - n is a valid expression, which returns an object of type T
	 * - x - y is a valid expression, which returns an object of type
	 * difference_type<T>
	 * - dereference_t<T> is a reference type
	 * - x[n] is a valid expression, which returns a value of type
	 * dereference_t<T>
	 * - x < y is a valid expression, the result of which is convertible to bool
	 * - x > y is a valid expression, the result of which is convertible to bool
	 * - x <= y is a valid expression, the result of which is convertible to
	 * bool
	 * - x >= y is a valid expression, the result of which is convertible to
	 * bool
	 */
	struct is_random_access_iterator<
		T,
		std::void_t<std::enable_if_t<
			is_bidirectional_iterator_v<T> &&
			std::is_same<
				T &, decltype(std::declval<std::add_lvalue_reference_t<T>>() +=
							  std::declval<difference_type<T>>())>::value &&
			std::is_same<
				T &, decltype(std::declval<std::add_lvalue_reference_t<T>>() -=
							  std::declval<difference_type<T>>())>::value &&
			std::is_same<T,
						 decltype(std::declval<T>() +
								  std::declval<difference_type<T>>())>::value &&
			std::is_same<T, decltype(std::declval<difference_type<T>>() +
									 std::declval<T>())>::value &&
			std::is_same<T,
						 decltype(std::declval<T>() -
								  std::declval<difference_type<T>>())>::value &&
			std::is_same<difference_type<T>,
						 decltype(std::declval<T>() -
								  std::declval<T>())>::value &&
			std::is_reference<dereference_t<T>>::value &&
			std::is_same<dereference_t<T>,
						 decltype((std::declval<T>())[std::declval<
							 difference_type<T>>()])>::value &&
			std::is_convertible<bool, decltype(std::declval<T>() <
											   std::declval<T>())>::value &&
			std::is_convertible<bool, decltype(std::declval<T>() >
											   std::declval<T>())>::value &&
			std::is_convertible<bool, decltype(std::declval<T>() <=
											   std::declval<T>())>::value &&
			std::is_convertible<bool, decltype(std::declval<T>() >=
											   std::declval<T>())>::value

			>>> : std::true_type {};

	template <class T>
	static constexpr bool is_random_access_iterator_v =
		is_random_access_iterator<T>::value;

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

	/**
	 * Let x be an object of T, and n an object of type difference_type<T>.
	 *  is_contiguous_iterator declares a static boolean constant equal to true
	 * if and only if:
	 * - is_random_access_iterator_v<T> is true
	 * - *(x + n) is a valid expression
	 */
	template <class T, class = void>
	struct is_contiguous_iterator : std::false_type {};

	template <class T>
	struct is_contiguous_iterator<
		T, std::void_t<decltype(*(std::declval<T>() +
								  std::declval<difference_type<T>>())),
					   std::enable_if_t<is_random_access_iterator_v<T>>>>
		: std::true_type {};

	template <class T>
	static constexpr bool is_contiguous_iterator_v =
		is_contiguous_iterator<T>::value;

	template <class T, class = void>
	struct is_iterable : std::false_type {};

	namespace _details {
		template <class T>
		using begin_t = decltype(std::begin(
			std::declval<std::add_lvalue_reference_t<T>>()));
		template <class T>
		using end_t =
			decltype(std::end(std::declval<std::add_lvalue_reference_t<T>>()));
	} // namespace _details

	template <class T>
	struct is_iterable<
		T, std::void_t<std::enable_if_t<
			   is_iterator_v<_details::begin_t<T>> &&
			   std::is_same<_details::begin_t<T>, _details::end_t<T>>::value &&
			   !std::is_void<dereference_t<_details::begin_t<T>>>::value>>>
		: std::true_type {};

	template <class T>
	static constexpr bool is_iterable_v = is_iterable<T>::value;

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

	template <class T, class... Args>
	struct is_invocable
		: _details::invokable_with<T, _details::Pack_t<Args...>> {};

	template <class T, class... Args>
	static constexpr bool is_invocable_v = is_invocable<T, Args...>::value;

	template <class T>
	struct type_identity {
		using type = T;
	};

	namespace details {
		template <size_t N, class First, class... Others>
		struct nth_type_impl {
			using type = typename nth_type_impl<N - 1, Others...>::type;
		};

		template <class First, class... O>
		struct nth_type_impl<0, First, O...> : type_identity<First> {};

	} // namespace details

	template <size_t N, class... T>
	struct nth_type : details::nth_type_impl<N, T...> {};

} // namespace libstra
