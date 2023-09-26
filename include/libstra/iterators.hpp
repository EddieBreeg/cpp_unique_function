#pragma once

#include "utility.hpp"

namespace libstra {

	namespace _details {
		template <class T, class = void>
		struct dereferenceable : std::false_type {};
		template <class T>
		struct dereferenceable<T, std::void_t<dereference_t<T>>>
			: std::true_type {};

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

		template <class T, bool = false, class = void>
		struct indirectly_readable_traits {};
		template <class T>
		struct indirectly_readable_traits<T *, false> {
			using value_type = std::remove_cv_t<T>;
		};
		template <class T>
		struct indirectly_readable_traits<const T, false>
			: indirectly_readable_traits<T> {};
		template <class T>
		struct indirectly_readable_traits<
			T, false, std::enable_if_t<std::is_array<T>::value>> {
			using value_type = std::remove_cv_t<std::remove_extent_t<T>>;
		};
		template <class T>
		struct indirectly_readable_traits<
			T, false,
			std::enable_if_t<dereferenceable<T>::value &&
							 has_value_type<T>::value>> {
			using value_type = std::remove_cv_t<typename T::value_type>;
		};
		template <class T>
		struct indirectly_readable_traits<
			T, false,
			std::void_t<dereference_t<T>,
						std::enable_if_t<has_element_type<T>::value, int>>> {
			using value_type = std::remove_cv_t<typename T::element_type>;
		};
		template <class T>
		struct indirectly_readable_traits<
			T, true,
			std::void_t<dereference_t<T>,
						std::enable_if_t<
							std::is_same<typename T::value_type,
										 typename T::element_type>::value>>> {
			using value_type = std::remove_cv_t<typename T::value_type>;
		};

	} // namespace _details

	template <class T>
	struct indirectly_readable_traits
		: _details::indirectly_readable_traits<
			  T, _details::has_element_type<T>::value &&
					 _details::has_value_type<T>::value> {};

	template <class T>
	using iter_value_t = std::conditional_t<
		_details::has_value_type<
			std::iterator_traits<remove_cv_ref_t<T>>>::value,
		typename std::iterator_traits<remove_cv_ref_t<T>>::value_type,
		typename indirectly_readable_traits<remove_cv_ref_t<T>>::value_type>;

	template <class T>
	using iter_reference_t = dereference_t<std::add_lvalue_reference_t<T>>;

	namespace _details {
		template <class T, class = void>
		struct has_diff_t : std::false_type {};
		template <class T>
		struct has_diff_t<T, typename T::difference_type> : std::true_type {};
	} // namespace _details

	template <class T>
	using iter_difference_t = std::conditional_t<
		_details::has_diff_t<std::iterator_traits<remove_cv_ref_t<T>>>::value,
		typename std::iterator_traits<remove_cv_ref_t<T>>::difference_type,
		difference_type<remove_cv_ref_t<T>>>;

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
		template <class Iter, bool = true>
		struct const_iter_base {
		public:
			using iterator_category =
				typename std::iterator_traits<Iter>::iterator_category;
		};
		template <class Iter>
		struct const_iter_base<Iter, false> {
		public:
			static_assert(is_input_iterator_v<Iter>,
						  "Iter must be an input iterator type");
		};
		template <class T>
		struct make_const : type_identity<const T> {};
		template <class T>
		struct make_const<T *> : type_identity<const T *> {};
		template <class T>
		struct make_const<T &> : type_identity<const T &> {};
		template <class T>
		struct make_const<T &&> : type_identity<const T &&> {};

		template <class T>
		struct make_const<const T> : make_const<T> {};
		template <class T>
		using make_const_t = typename make_const<T>::type;

		template <class Iter, class = void>
		struct has_member_access_method : std::false_type {};

		template <class Iter>
		struct has_member_access_method<
			Iter, std::void_t<
					  decltype(std::declval<std::add_lvalue_reference_t<Iter>>()
								   .
								   operator->())>> : std::true_type {};
	} // namespace _details

	template <class Iter>
	class basic_const_iterator
		: public _details::const_iter_base<Iter, is_forward_iterator_v<Iter>> {
		Iter _i{};

		using reference =
			typename _details::make_const_t<iter_reference_t<Iter>>;

	public:
		using value_type = iter_value_t<Iter>;
		using difference_type = iter_difference_t<Iter>;

		constexpr basic_const_iterator() = default;

		template <class I = Iter, class = std::enable_if_t<
									  std::is_constructible<Iter, I &&>::value>>
		constexpr basic_const_iterator(I &&x) : _i{ std::forward<I>(x) } {}

		constexpr basic_const_iterator &operator++() {
			++_i;
			return *this;
		}
		constexpr basic_const_iterator operator++(int) {
			return basic_const_iterator(_i++);
		}
		template <class I = Iter,
				  class = std::enable_if_t<is_bidirectional_iterator_v<I>>>
		constexpr basic_const_iterator &operator--() {
			--_i;
			return *this;
		}
		template <class I = Iter,
				  class = std::enable_if_t<is_bidirectional_iterator_v<I>>>
		constexpr basic_const_iterator operator--(int) {
			return basic_const_iterator(_i--);
		}

		[[nodiscard]]
		constexpr bool
		operator==(const basic_const_iterator &other) const {
			return _i == other._i;
		}
		[[nodiscard]]
		constexpr bool
		operator!=(const basic_const_iterator &other) const {
			return _i != other._i;
		}
		template <class I = Iter,
				  class = std::enable_if_t<is_totally_ordered_v<I>>>
		[[nodiscard]]
		constexpr bool
		operator<(const basic_const_iterator &other) {
			return _i < other._i;
		}
		template <class I = Iter,
				  class = std::enable_if_t<is_totally_ordered_v<I>>>
		[[nodiscard]]
		constexpr bool
		operator>(const basic_const_iterator &other) {
			return _i > other._i;
		}
		template <class I = Iter,
				  class = std::enable_if_t<is_totally_ordered_v<I>>>
		[[nodiscard]]
		constexpr bool
		operator<=(const basic_const_iterator &other) {
			return _i <= other._i;
		}
		template <class I = Iter,
				  class = std::enable_if_t<is_totally_ordered_v<I>>>
		[[nodiscard]]
		constexpr bool
		operator>=(const basic_const_iterator &other) {
			return _i >= other._i;
		}

		[[nodiscard]]
		constexpr basic_const_iterator::reference
		operator*() const {
			return *_i;
		}
		template <class I = Iter,
				  std::enable_if_t<_details::has_member_access_method<I>::value,
								   int> = 0>
		[[nodiscard]]
		constexpr _details::make_const_t<_details::member_access_t<Iter>>
		operator->() const {
			return _i.operator->();
		}
		template <class I = Iter,
				  std::enable_if_t<std::is_pointer<I>::value, int> = 0>
		[[nodiscard]]
		constexpr _details::make_const_t<Iter>
		operator->() const {
			return _i;
		}

		template <class I = Iter,
				  class = std::enable_if_t<_details::advanceable<I>::value>>
		constexpr basic_const_iterator
		operator+(iter_difference_t<Iter> n) const {
			return _i + n;
		}
		template <class I = Iter,
				  class = std::enable_if_t<_details::advanceable<I>::value>>
		constexpr basic_const_iterator
		operator-(iter_difference_t<Iter> n) const {
			return _i - n;
		}
		template <class I = Iter,
				  class = std::enable_if_t<_details::advanceable<I>::value>>
		constexpr basic_const_iterator &operator-=(iter_difference_t<Iter> n) {
			_i -= n;
			return *this;
		}
		template <class I = Iter,
				  class = std::enable_if_t<_details::advanceable<I>::value>>
		constexpr basic_const_iterator &operator+=(iter_difference_t<Iter> n) {
			_i += n;
			return *this;
		}

		template <class I = Iter,
				  class = std::enable_if_t<_details::advanceable<I>::value>>
		constexpr iter_difference_t<Iter>
		operator-(const basic_const_iterator &other) const {
			return _i - other._i;
		}

		template <class I = Iter,
				  class = std::enable_if_t<is_random_access_iterator_v<I>>>
		[[nodiscard]]
		constexpr basic_const_iterator::reference
		operator[](iter_difference_t<Iter> n) const {
			return _i[n];
		}
	};

	namespace _details {
		template <class Iter, class = void>
		struct reverse_iter_base {
			static_assert(is_iterator_v<Iter> &&
							  is_weakly_decrementable_v<Iter>,
						  "Iter must be a weakly decrementable iterator type");
		};
		template <class Iter>
		struct reverse_iter_base<
			Iter,
			std::void_t<typename std::iterator_traits<Iter>::iterator_category>>
			: std::iterator_traits<Iter> {
			static_assert(is_iterator_v<Iter> &&
							  is_weakly_decrementable_v<Iter>,
						  "Iter must be a weakly decrementable iterator type");
		};
	} // namespace _details

	template <class Iter>
	class basic_reverse_iterator : _details::reverse_iter_base<Iter> {
		Iter _i{};
		using const_reference = _details::make_const_t<iter_reference_t<Iter>>;

	public:
		constexpr operator Iter() const { return _i; }
		constexpr basic_reverse_iterator() = default;
		template <class I = Iter, class = std::enable_if_t<
									  std::is_constructible<Iter, I>::value>>
		constexpr basic_reverse_iterator(I &&it) : _i{ it } {}

		constexpr iter_reference_t<Iter> operator*() { return *_i; }
		constexpr basic_reverse_iterator::const_reference operator*() const {
			return *_i;
		}

		template <class I = Iter,
				  std::enable_if_t<_details::has_member_access_method<I>::value,
								   int> = 0>
		constexpr _details::member_access_t<Iter> operator->() {
			return _i.operator->();
		}
		template <class I = Iter,
				  std::enable_if_t<std::is_pointer<I>::value, int> = 0>
		constexpr Iter operator->() {
			return _i;
		}
		template <class I = Iter,
				  std::enable_if_t<_details::has_member_access_method<I>::value,
								   int> = 0>
		constexpr _details::make_const_t<_details::member_access_t<Iter>>
		operator->() const {
			return _i.operator->();
		}
		template <class I = Iter,
				  std::enable_if_t<std::is_pointer<I>::value, int> = 0>
		constexpr _details::make_const_t<Iter> operator->() const {
			return _i;
		}

		[[nodiscard]]
		constexpr bool
		operator==(const basic_reverse_iterator &other) const {
			return _i == other._i;
		}
		[[nodiscard]]
		constexpr bool
		operator==(const Iter &other) const {
			return _i == other;
		}
		[[nodiscard]]
		constexpr bool
		operator!=(const basic_reverse_iterator &other) const {
			return _i != other._i;
		}
		[[nodiscard]]
		constexpr bool
		operator!=(const Iter &other) const {
			return _i != _i;
		}
		template <class I = Iter,
				  class = std::enable_if_t<is_totally_ordered_v<I>>>
		[[nodiscard]]
		constexpr bool
		operator<(const basic_reverse_iterator &other) {
			return _i > other._i;
		}
		template <class I = Iter,
				  class = std::enable_if_t<is_totally_ordered_v<I>>>
		[[nodiscard]]
		constexpr bool
		operator<(const Iter &other) {
			return _i > other;
		}
		template <class I = Iter,
				  class = std::enable_if_t<is_totally_ordered_v<I>>>
		[[nodiscard]]
		constexpr bool
		operator>(const basic_reverse_iterator &other) {
			return _i < other._i;
		}
		template <class I = Iter,
				  class = std::enable_if_t<is_totally_ordered_v<I>>>
		[[nodiscard]]
		constexpr bool
		operator<=(const basic_reverse_iterator &other) {
			return _i >= other._i;
		}
		template <class I = Iter,
				  class = std::enable_if_t<is_totally_ordered_v<I>>>
		[[nodiscard]]
		constexpr bool
		operator>=(const basic_reverse_iterator &other) {
			return _i <= other._i;
		}

		template <class I = Iter,
				  class = std::enable_if_t<is_totally_ordered_v<I>>>
		[[nodiscard]]
		constexpr bool
		operator>(const Iter &other) {
			return _i < other;
		}
		template <class I = Iter,
				  class = std::enable_if_t<is_totally_ordered_v<I>>>
		[[nodiscard]]
		constexpr bool
		operator<=(const Iter &other) {
			return _i >= other;
		}
		template <class I = Iter,
				  class = std::enable_if_t<is_totally_ordered_v<I>>>
		[[nodiscard]]
		constexpr bool
		operator>=(const Iter &other) {
			return _i <= other;
		}

		constexpr basic_reverse_iterator &operator++() {
			--_i;
			return *this;
		}
		constexpr basic_reverse_iterator &operator--() {
			++_i;
			return *this;
		}
		constexpr basic_reverse_iterator operator++(int) { return --_i; }
		constexpr basic_reverse_iterator operator--(int) { return ++_i; }

		template <class I = Iter,
				  class = std::enable_if_t<_details::advanceable<I>::value>>
		constexpr basic_reverse_iterator
		operator+(iter_difference_t<Iter> n) const {
			return _i - n;
		}
		template <class I = Iter,
				  class = std::enable_if_t<_details::advanceable<I>::value>>
		constexpr basic_reverse_iterator
		operator-(iter_difference_t<Iter> n) const {
			return _i + n;
		}
		template <class I = Iter,
				  class = std::enable_if_t<_details::advanceable<I>::value>>
		constexpr basic_reverse_iterator &
		operator+=(iter_difference_t<Iter> n) {
			_i -= n;
			return *this;
		}
		template <class I = Iter,
				  class = std::enable_if_t<_details::advanceable<I>::value>>
		constexpr basic_reverse_iterator &
		operator-=(iter_difference_t<Iter> n) {
			_i += n;
			return *this;
		}
		template <class I = Iter,
				  class = std::enable_if_t<_details::advanceable<I>::value>>
		constexpr iter_difference_t<Iter>
		operator-(const basic_reverse_iterator &other) {
			return other._i - _i;
		}

		template <class I = Iter,
				  std::enable_if_t<is_random_access_iterator_v<I>, int> = 0>
		constexpr iter_reference_t<Iter> operator[](iter_difference_t<Iter> n) {
			return *(_i - n);
		}
		template <class I = Iter,
				  std::enable_if_t<is_random_access_iterator_v<I>, int> = 0>
		constexpr basic_reverse_iterator::const_reference
		operator[](iter_difference_t<Iter> n) const {
			return *(_i - n);
		}
	};

} // namespace libstra
