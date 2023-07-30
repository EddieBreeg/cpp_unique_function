#pragma once

#if __cplusplus >= 201703
#define _HAS_CPP_17
#endif

#include <utility>
#include <initializer_list>
#include <libstra/utility.hpp>

namespace libstra {
	template <class E>
	class unexpected {
	private:
		E _err;

	public:
		constexpr unexpected(const unexpected &) = default;
		constexpr unexpected(unexpected &&) = default;

		template <class Err = E,
				  typename = std::enable_if_t<!std::is_same<
					  std::remove_reference_t<Err>, unexpected>::value>>
		constexpr explicit unexpected(Err &&e) : _err(forward<Err>(e)) {}

		template <class... Args>
		constexpr explicit unexpected(libstra::in_place_t, Args &&...args) :
			_err(forward<Args>(args)...) {}

		template <class U, class... Args>
		constexpr explicit unexpected(in_place_t, std::initializer_list<U> il,
									  Args &&...args) :
			_err(il, forward<Args>(args)...) {}

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

#ifdef _CPP_17
		constexpr void
		swap(unexpected &other) noexcept(std::is_nothrow_swappable_v<E>) {
			std::swap(_err, other._err);
		}
#else
		constexpr void swap(unexpected &other) { std::swap(_err, other._err); }
#endif
		template <class E2>
		[[nodiscard]]
		constexpr bool
		operator==(const unexpected<E2> &other) const noexcept {
			return _err == other._err;
		}
	};

} // namespace libstra
