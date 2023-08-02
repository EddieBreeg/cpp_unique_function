#pragma once

#include <type_traits>
#include <utility>
#include <memory>
#include "internal/config.h"

namespace libstra {

	struct null_ptr_error {
		[[nodiscard]]
		constexpr const char *what() const noexcept {
			return "Attempted to assign null to a not_null_ptr";
		}
	};

	template <class T>
	struct is_pointer : public std::is_pointer<T> {};
	template <class T>
	class not_null_ptr;

	template <class T, class D>
	struct is_pointer<std::unique_ptr<T, D>> : public std::true_type {};
	template <class T>
	struct is_pointer<std::shared_ptr<T>> : public std::true_type {};
	template <class T>
	struct is_pointer<std::weak_ptr<T>> : public std::true_type {};

	template <class T>
	struct is_pointer<not_null_ptr<T>> : public is_pointer<T> {};

	template <class T>
	static constexpr bool is_pointer_v = is_pointer<T>::value;

	template <class Ptr>
	class not_null_ptr {
	public:
		static_assert(is_pointer_v<Ptr>, "Ptr must be a pointer type");

		constexpr not_null_ptr(decltype(nullptr)) = delete;
		template <class U,
				  std::enable_if_t<std::is_convertible<U, Ptr>::value &&
									   !std::is_same<U, not_null_ptr>::value,
								   int> = 0>
		constexpr not_null_ptr(U &&p) : _p(std::forward<U>(p)) {
			if (!_p) _unlikely throw null_ptr_error{};
		}
		constexpr not_null_ptr(Ptr &&p) : _p(std::move(p)) {
			if (!_p) _unlikely throw null_ptr_error{};
		}
		constexpr not_null_ptr(const Ptr &p) : _p(p) {
			if (!_p) _unlikely throw null_ptr_error{};
		}
		template <class U,
				  std::enable_if_t<std::is_convertible<U, Ptr>::value, int> = 0>
		constexpr not_null_ptr(const not_null_ptr<U> &other) : _p(other._p) {}
		template <class U,
				  std::enable_if_t<std::is_convertible<U, Ptr>::value, int> = 0>
		constexpr not_null_ptr(not_null_ptr<U> &&other) noexcept :
			_p(std::move(other._p)) {}

		template <class U,
				  std::enable_if_t<std::is_convertible<U, Ptr>::value &&
									   !std::is_same<U, not_null_ptr>::value,
								   int> = 0>
		constexpr not_null_ptr &operator=(U &&p) {
			if (!p) _unlikely throw null_ptr_error{};
			_p = std::forward<U>(p);
			return *this;
		}
		template <class U,
				  std::enable_if_t<std::is_convertible<U, Ptr>::value, int> = 0>
		constexpr not_null_ptr &operator=(const not_null_ptr &other) {
			_p = other._p;
			return *this;
		}
		template <class U,
				  std::enable_if_t<std::is_convertible<U, Ptr>::value, int> = 0>
		constexpr not_null_ptr &operator=(not_null_ptr &&other) noexcept {
			_p = std::move(other._p);
			return *this;
		}
		not_null_ptr &operator=(decltype(nullptr)) = delete;

		[[nodiscard]]
		constexpr auto &
		operator*() & noexcept {
			_assume(_p);
			return *_p;
		}
		[[nodiscard]]
		constexpr const auto &
		operator*() const & noexcept {
			_assume(_p);
			return *_p;
		}
		[[nodiscard]]
		constexpr auto &&
		operator*() && noexcept {
			_assume(_p);
			return *_p;
		}
		[[nodiscard]]
		constexpr const auto &&
		operator*() const && noexcept {
			_assume(_p);
			return *_p;
		}

		[[nodiscard]] constexpr operator Ptr() const noexcept { return _p; }

		[[nodiscard]]
		constexpr Ptr &
		operator->() noexcept {
			return _p;
		}
		[[nodiscard]]
		constexpr const Ptr &
		operator->() const noexcept {
			return _p;
		}
		[[nodiscard]]
		constexpr Ptr &get() noexcept {
			return _p;
		}
		[[nodiscard]]
		constexpr const Ptr &get() const noexcept {
			return _p;
		}

		template <class U,
				  std::enable_if_t<std::is_convertible<U, Ptr>::value &&
									   !std::is_same<U, not_null_ptr>::value,
								   int> = 0>
		[[nodiscard]]
		constexpr bool
		operator==(U &&p) const noexcept {
			return p == _p;
		}
		template <class U,
				  std::enable_if_t<std::is_convertible<U, Ptr>::value, int> = 0>
		[[nodiscard]]
		constexpr bool
		operator==(const not_null_ptr<U> &p) const noexcept {
			return p._p == _p;
		}

	private:
		Ptr _p;
	};

	template <class T, class U>
	[[nodiscard]]
	constexpr bool
	operator!=(const not_null_ptr<T> &a, U &&b) {
		return !(a == b);
	}

	/**
	 * An alias type which communicates explicitly the pointer
	 * owns the memory it points to. It is the responsibility
	 * of whoever has the pointer to release the memory
	 * @tparam T: The (possibly cv-qualified) type of object the pointer points
	 * to
	 */
	template <class T>
	using owning_ptr = std::add_pointer_t<T>;

	/**
	 * An alias type which communicates explicitly the pointer
	 * doesn't own the memory it points to
	 * @tparam T: The (possibly cv-qualified) type of object the pointer points
	 * to
	 */
	template <class T>
	using non_owning_ptr = std::add_pointer_t<T>;

} // namespace libstra
