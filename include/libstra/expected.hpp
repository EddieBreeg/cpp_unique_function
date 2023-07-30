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
	template <class T, class E, bool TriviallyDestructible>
	struct _expected_base;

	template <class T, class E>
	struct _expected_base<T, E, false> {
		union {
			T _val{};
			E _err;
		};
		bool _has_val = true;
		~_expected_base() {
			if (_has_val) _val.~T();
			else _err.~E();
		}
	};
	template <class T, class E>
	struct _expected_base<T, E, true> {
		union {
			T _val{};
			E _err;
		};
		bool _has_val = true;
	};
	template <class E>
	struct _expected_base<void, E, true> {
		union {
			char _val{};
			E _err;
		};
		bool _has_val = true;
	};
	template <class E>
	struct _expected_base<void, E, false> {
		union {
			char _val{};
			E _err;
		};
		bool _has_val = true;
		~_expected_base() {
			if (!_has_val) _err.~E();
		}
	};

	template <class T, class E>
	class [[nodiscard]] expected {
	private:
		using base_type =
			_expected_base<T, E,
						   (std::is_trivially_destructible<T>::value ||
							std::is_void<T>::value) &&
							   (std::is_trivially_destructible<E>::value)>;
		base_type _b;

	public:
		constexpr expected() : _b{ {} } {}

		[[nodiscard]]
		constexpr bool has_value() const noexcept {
			return _b._has_val;
		}
	};

} // namespace libstra
