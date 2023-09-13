#pragma once

#include "utility.hpp"

namespace libstra {
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
	 * Let x be an object of type T. is_birectional_iterator declares a static
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
		template <class T, class = void>
		struct has_value_type : std::false_type {};
		template <class T>
		struct has_value_type<T, std::void_t<typename T::value_type>>
			: std::true_type {};

		template <class T, class = void>
		struct has_element_type : std::false_type {};
		template <class T>
		struct has_element_type<T, std::void_t<typename T::element_type>>
			: std::true_type {};

		template <class T, class V = typename std::iterator_traits<
							   remove_cv_ref_t<T>>::value_type>
		struct iter_value : type_identity<V> {};

	} // namespace _details

	template <class T>
	using iter_value_t = typename _details::iter_value<T>::type;

} // namespace libstra
