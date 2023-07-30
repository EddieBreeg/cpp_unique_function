#pragma once

#include <type_traits>
#include <new>

namespace libstra {

	template <class T, bool TrivialDestructor>
	struct _storage_impl;

	template <class T>
	struct _storage_impl<T, true> {
		union {
			T _val;
			alignas(T) char _mem[sizeof(T)] = { 0 };
		};
	};
	template <class T>
	struct _storage_impl<T, false> {
		union {
			T _val;
			alignas(T) char _mem[sizeof(T)] = { 0 };
		};

		~_storage_impl() { _val.~T(); }
	};

	/**
	 * Aligned storage class with enough storage to accommodate an object of
	 * type T
	 * @tparam T: The type of value in the storage
	 */
	template <class T>
	class storage
		: public _storage_impl<T, std::is_trivially_destructible<T>::value> {
	public:
		/**
		 * Constructs an empty storage, which does not contain any value
		 * @note T need not be default constructible
		 */
		constexpr storage() = default;

		template <class U, typename = std::enable_if_t<
							   !std::is_same<std::decay_t<U>, storage>::value &&
							   !std::is_void<T>::value>>
		storage &operator=(U &&x) {
			_val = std::forward<U>(x);
			return *this;
		}
		[[nodiscard]]
		T &
		operator*() & noexcept {
			return _val;
		}
		[[nodiscard]]
		T &&
		operator*() && noexcept {
			return _val;
		}
		[[nodiscard]]
		const T &
		operator*() const & noexcept {
			return _val;
		}
		[[nodiscard]]
		const T &&
		operator*() const && noexcept {
			return _val;
		}
		[[nodiscard]]
		T *
		operator->() noexcept {
			return &_val;
		}
		[[nodiscard]]
		const T *
		operator->() const noexcept {
			return &_val;
		}
	};

	template <>
	class storage<void> {};

} // namespace libstra

#ifdef _HAS_CPP_17
#undef _HAS_CPP_17
#endif