#ifndef LIBSTRA_STORAGE_H
#define LIBSTRA_STORAGE_H

#if __cplusplus >= 201703
#define _CPP_17
#endif

#include <utility>
#include <type_traits>
#ifdef _CPP_17
#include <optional>
#endif

namespace libstra {
	template <class T, bool TrivalDestructor>
	union _storage_base;

	template <class T>
	union _storage_base<T, false> {
		T _val;
		char _buf[sizeof(T)];
		_storage_base() noexcept : _buf{} {}
		_storage_base(const T &x) : _val(x) {}
		_storage_base(T &&x) noexcept : _val(std::move(x)) {}

		template <class U>
		_storage_base(U &&other) : _val(std::forward<U>(other)) {}

#ifdef _CPP_17
		template <typename... Args>
		_storage_base(std::in_place_t, Args &&...args) :
			_val(std::forward<Args>(args)...) {}
		template <class U, typename... Args>
		_storage_base(std::in_place_t, std::initializer_list<U> il,
					  Args &&...args) :
			_val(il, std::forward<Args>(args)...) {}
#endif

		~_storage_base() { _val.~T(); }
	};
	template <class T>
	union _storage_base<T, true> {
		T _val;
		char _buf[sizeof(T)]{};
		constexpr _storage_base() noexcept : _buf{} {}
		constexpr _storage_base(const T &x) : _val(x) {}
		constexpr _storage_base(T &&x) noexcept : _val(std::move(x)) {}

		template <class U>
		constexpr _storage_base(U &&other) : _val(std::forward<U>(other)) {}
#ifdef _CPP_17
		template <typename... Args>
		constexpr _storage_base(std::in_place_t, Args &&...args) :
			_val(std::forward<Args>(args)...) {}
		template <class U, typename... Args>
		constexpr _storage_base(std::in_place_t, std::initializer_list<U> il,
								Args &&...args) :
			_val(il, std::forward<Args>(args)...) {}
#endif
	};

	template <class T>
	class storage {
		_storage_base<T, std::is_trivially_destructible<T>::value> _b;

	public:
		constexpr storage() noexcept = default;
		constexpr storage(const T &x) : _b(x) {}
		constexpr storage(T &&x) : _b(std::move(x)) {}

		template <class U, typename = std::enable_if_t<!std::is_same<
							   std::decay_t<U>, storage<T>>::value>>
		constexpr storage(U &&other) : _b(std::forward<U>(other)) {}

#ifdef _CPP_17
		template <typename... Args>
		constexpr storage(std::in_place_t, Args &&...args) :
			_b(std::in_place_t{}, std::forward<Args>(args)...) {}
		template <class U, typename... Args>
		constexpr storage(std::in_place_t, std::initializer_list<U> il,
						  Args &&...args) :
			_b(std::in_place_t{}, il, std::forward<Args>(args)...) {}
#endif
		template <class U, typename = std::enable_if_t<!std::is_same<
							   std::decay_t<U>, storage<T>>::value>>
		constexpr storage &operator=(U &&other) {
			_b._val = std::forward<U>(other);
			return *this;
		}
		constexpr storage &operator=(const storage &other) {
			_b = other._b;
			return *this;
		}

		[[nodiscard]]
		constexpr T &
		operator*() & noexcept {
			return _b._val;
		}
		[[nodiscard]]
		constexpr const T &
		operator*() const & noexcept {
			return _b._val;
		}

		[[nodiscard]]
		constexpr T *
		operator->() noexcept {
			return &_b._val;
		}
		[[nodiscard]]
		constexpr const T *
		operator->() const noexcept {
			return &_b._val;
		}

#ifdef _CPP_17
		constexpr void
		swap(storage &other) noexcept(std::is_nothrow_swappable_v<T>) {
			std::swap(**this, *other);
		}
#else
		constexpr void swap(storage &other) { std::swap(**this, *other); }
#endif
	};

	template <class T, class U>
	[[nodiscard]]
	constexpr bool
	operator==(const storage<T> &a, const storage<U> &b) noexcept {
		return *a == *b;
	}

	template <class T, class U>
	[[nodiscard]]
	constexpr bool
	operator!=(const storage<T> &a, const storage<U> &b) noexcept {
		return *a != *b;
	}

} // namespace libstra

#endif