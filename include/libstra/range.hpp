#pragma once

#include "utility.hpp"

namespace libstra {

	namespace _details {
		template <class T, class = void>
		struct is_range : std::false_type {};

		template <class T>
		struct is_range<T,
						std::enable_if_t<is_iterator_v<_details::begin_t<T>>>>
			: std::true_type {};

	} // namespace _details

	template <class T, class = std::enable_if_t<is_iterable_v<T>>>
	using iterator_t = _details::begin_t<T>;
	template <class T>
	static constexpr bool is_range_v = _details::is_range<T>::value;

	namespace _details {
		template <class T, class = void>
		struct is_forward_range : std::false_type {};
		template <class T>
		struct is_forward_range<
			T, std::enable_if_t<is_range_v<T> &&
								is_forward_iterator_v<iterator_t<T>>>>
			: std::true_type {};

		template <class T, class = void>
		struct is_bidirectional_range : std::false_type {};
		template <class T>
		struct is_bidirectional_range<
			T, std::enable_if_t<is_range_v<T> &&
								is_bidirectional_iterator_v<iterator_t<T>>>>
			: std::true_type {};

		template <class T, class = void>
		struct is_randaccess_range : std::false_type {};
		template <class T>
		struct is_randaccess_range<
			T, std::enable_if_t<is_range_v<T> &&
								is_random_access_iterator_v<iterator_t<T>>>>
			: std::true_type {};

		template <class T, class = void>
		struct is_sized_range : std::false_type {};

		template <class T>
		struct is_sized_range<T,
							  std::void_t<decltype(std::declval<T>().size())>>
			: std::true_type {};

	} // namespace _details

	template <class T>
	static constexpr bool is_forward_range_v =
		_details::is_forward_range<T>::value;

	template <class T>
	static constexpr bool is_bidirectional_range_v =
		_details::is_bidirectional_range<T>::value;

	template <class T>
	static constexpr bool is_random_access_range_v =
		_details::is_randaccess_range<T>::value;

	template <class D>
	struct view_interface {
		static_assert(std::is_object<D>::value && !is_cv_qualified_v<D>,
					  "D must be a non-cv-qualified object type");
		template <class R = D, class = std::enable_if_t<is_forward_range_v<R>>>
		constexpr decltype(auto) front() {
			return *std::begin(static_cast<D &>(*this));
		}
		template <class R = D, class = std::enable_if_t<is_forward_range_v<R>>>
		constexpr decltype(auto) front() const {
			return *std::begin(static_cast<const D &>(*this));
		}

		template <class R = D,
				  class = std::enable_if_t<is_bidirectional_range_v<R>>>
		constexpr decltype(auto) back() {
			auto it = std::end(static_cast<D &>(*this));
			return *(--it);
		}
		template <class R = D,
				  class = std::enable_if_t<is_bidirectional_range_v<R>>>
		constexpr decltype(auto) back() const {
			auto it = std::end(static_cast<D &>(*this));
			return *(--it);
		}

		template <class R = D,
				  class = std::enable_if_t<is_random_access_range_v<R>>>
		constexpr auto data() {
			return std::addressof(front());
		}
		template <class R = D,
				  class = std::enable_if_t<is_random_access_range_v<R>>>
		constexpr auto data() const {
			return std::addressof(front());
		}

		template <class R = D,
				  class = std::enable_if_t<is_random_access_range_v<R>>>
		constexpr decltype(auto) operator[](size_t i) {
			return std::begin(static_cast<D &>(*this))[i];
		}
		template <class R = D,
				  class = std::enable_if_t<is_random_access_range_v<R>>>
		constexpr decltype(auto) operator[](size_t i) const {
			return std::begin(static_cast<const D &>(*this))[i];
		}
		template <class R = D, class = std::enable_if_t<is_forward_range_v<R>>>
		[[nodiscard]]
		constexpr bool is_empty() const {
			auto it = std::begin(static_cast<const D &>(*this));
			return ++it == std::end(static_cast<const D &>(*this));
		}
	};

	template <class T>
	class empty_view : public view_interface<empty_view<T>> {
	public:
		static_assert(std::is_object<T>::value, "T must be an object type");
		constexpr empty_view() noexcept = default;
		static constexpr T *begin() noexcept { return nullptr; }
		static constexpr T *end() noexcept { return nullptr; }
		[[nodiscard]]
		static constexpr size_t size() noexcept {
			return 0;
		}
		constexpr T *data() const noexcept { return nullptr; }
	};

} // namespace libstra
