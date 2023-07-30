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
#ifdef _HAS_CPP_17

	template <class E>
	class [[nodiscard]] unexpected {
		template <bool B>
		using enable_if = std::enable_if_t<B, int>;

	public:
		constexpr unexpected(const unexpected &) = default;
		constexpr unexpected(unexpected &&) = default;
		template <class Err,
				  typename = enable_if<!std::is_same_v<Err, unexpected> &&
									   !std::is_same_v<Err, std::in_place_t>>>
		constexpr unexpected(Err &&e) : _err(std::forward<Err>(e)) {}

		template <typename... Args>
		constexpr unexpected(std::in_place_t, Args &&...args) :
			_err(std::forward<Args>(args)...) {}

		template <class U, class... Args>
		constexpr explicit unexpected(std::in_place_t,
									  std::initializer_list<U> il,
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
		swap(unexpected &other) noexcept(std::is_nothrow_swappable_v<E>) {
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

#endif
} // namespace libstra
