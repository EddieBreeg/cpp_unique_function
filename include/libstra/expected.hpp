#pragma once

#if __cplusplus >= 201703
#define _HAS_CPP_17
#endif

#include "storage.hpp"
#include <utility>
#include <initializer_list>

#if __cplusplus >= 201703
#define _HAS_CPP_17
#endif

namespace libstra {
#ifndef _HAS_CPP_17
	struct nullopt_t {};
	struct in_place_t {};

	template <class T, class U>
	struct is_nothrow_swappable {
		using l_T = std::add_lvalue_reference_t<T>;
		using l_U = std::add_lvalue_reference_t<U>;
		using type = typename std::bool_constant<
			noexcept(std::swap(std::declval<l_T>(), std::declval<l_U>())) &&
			noexcept(std::swap(std::declval<l_U>(), std::declval<l_T>()))>;
		static constexpr bool value = type::value;
	};
#else
#include <optional>
	using nullopt_t = std::nullopt_t;
	using in_place_t = std::in_place_t;
	template <class T, class U>
	using is_nothrow_swappable = std::is_nothrow_swappable<T, U>;
#endif
	template <class T, class U>
	static constexpr bool is_nothrow_swappable_v =
		is_nothrow_swappable<T, U>::value;

	template <class E>
	class [[nodiscard]] unexpected {
		template <bool B>
		using enable_if = std::enable_if_t<B, int>;

	public:
		constexpr unexpected(const unexpected &) = default;
		constexpr unexpected(unexpected &&) = default;
		template <class Err,
				  typename = enable_if<!std::is_same<Err, unexpected>::value &&
									   !std::is_same<Err, in_place_t>::value>>
		constexpr unexpected(Err &&e) : _err(std::forward<Err>(e)) {}
		template <typename... Args>
		constexpr unexpected(in_place_t, Args &&...args) :
			_err(std::forward<Args>(args)...) {}
		template <class U, class... Args>
		constexpr explicit unexpected(in_place_t, std::initializer_list<U> il,
									  Args &&...args) :
			_err(il, std::forward<Args>(args)...) {}

		[[nodiscard]]
		constexpr const E &error() const & noexcept {
			return _err;
		}
		[[nodiscard]]
		constexpr E &error() & noexcept {
			return _err;
		}
		[[nodiscard]]
		constexpr const E &&error() const && noexcept {
			return _err;
		}
		[[nodiscard]]
		constexpr E &&error() && noexcept {
			return _err;
		}
		constexpr void
		swap(unexpected &other) noexcept(is_nothrow_swappable_v<E, E>) {
			std::swap(_err, other._err);
		}
		template <class E2>
		[[nodiscard]]
		friend constexpr bool
		operator==(const unexpected &l, const unexpected<E2> &r) {
			return l._err == r._err;
		}

	private:
		E _err;
	};

	template <class T, class E>
	class [[nodiscard]] expected {
	private:
		union {
			storage<T> _val;
			storage<E> _err;
		};
		enum class Type : unsigned char { None, Value, Error } _type;
		static constexpr bool trivially_destructible =
			std::is_trivially_destructible<T>::value &&
			std::is_trivially_destructible<E>::value;

	public:
		using value_type = T;
		using error_type = E;
		using unexpected_type = unexpected<error_type>;
		template <class U>
		using rebind = expected<U, error_type>;
		constexpr expected() : _val{}, _type{ Type::Value } {}
		constexpr expected(const expected &other) : _type(other._type) {}
		[[nodiscard]]
		constexpr bool has_value() const noexcept {
			return _type == Type::Value;
		}
	};
} // namespace libstra
