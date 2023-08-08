#pragma once

#include <cstddef>
#include <type_traits>
#include <stdexcept>
#include <libstra/utility.hpp>

namespace libstra {

	template <class T, size_t N>
	class static_vector {
	private:
		struct _storage {
			alignas(T) char _buf[sizeof(T)];
			T *ptr() { return (T *)_buf; }
			T *operator->() { return (T *)_buf; }
			const T *operator->() const { return (T *)_buf; }
			T &operator*() { return *(T *)_buf; }
			const T &operator*() const { return *(T *)_buf; }
		} _elems[N];
		size_t _size = 0;

		template <bool Const>
		class iterator_base {
		public:
			using difference_type = std::ptrdiff_t;
			using value_type = std::conditional_t<Const, const T, T>;
			using reference = std::conditional_t<Const, const T &, T>;
			using pointer = std::conditional_t<Const, const T *, T *>;
			using iterator_category = std::random_access_iterator_tag;
			using _Ptr_t =
				std::conditional_t<Const, const _storage *, _storage *>;

		private:
			_Ptr_t _ptr;

		public:
			iterator_base(_Ptr_t p) : _ptr(p) {}
			pointer operator->() noexcept { return (pointer)_ptr->_buf; }
			[[nodiscard]]
			reference
			operator*() noexcept {
				return **_ptr;
			}
			iterator_base &operator++() {
				++_ptr;
				return *this;
			}
			iterator_base operator++(int) { return iterator_base(_ptr++); }
			iterator_base &operator--() {
				--_ptr;
				return *this;
			}
			iterator_base operator--(int) { return iterator_base(_ptr--); }
			[[nodiscard]]
			bool
			operator!=(iterator_base other) const noexcept {
				return _ptr != other._ptr;
			}
			[[nodiscard]]
			bool
			operator==(iterator_base other) const noexcept {
				return _ptr == other._ptr;
			}

			[[nodiscard]]
			iterator_base
			operator+(std::ptrdiff_t n) const noexcept {
				return _ptr + n;
			}
			[[nodiscard]]
			iterator_base
			operator-(std::ptrdiff_t n) const noexcept {
				return _ptr - n;
			}
			[[nodiscard]]
			difference_type
			operator-(iterator_base other) const noexcept {
				return (_ptr - other._ptr);
			}
		};

	public:
		using iterator = iterator_base<false>;
		using const_iterator = iterator_base<true>;
		static_vector() = default;
		static_vector(size_t n) : _size(n) {
			if (n > N)
				throw std::out_of_range("Attempted to construct a static "
										"vector with too many elements");
			for (size_t i = 0; i < n; i++)
				new (_elems[i]._buf) T();
		}
		template <class U,
				  std::enable_if_t<std::is_convertible<U, T>::value, int> = 0>
		static_vector(size_t n, const U &val) : _size(n) {
			if (n > N)
				throw std::out_of_range("Attempted to construct a static "
										"vector with too many elements");
			for (size_t i = 0; i < n; i++)
				new (_elems[i]._buf) T(val);
		}
		[[nodiscard]]
		iterator begin() noexcept {
			return iterator(_elems);
		}
		[[nodiscard]]
		const_iterator begin() const noexcept {
			return _elems;
		}
		[[nodiscard]]
		iterator end() noexcept {
			return iterator(_elems + N);
		}
		[[nodiscard]]
		const_iterator end() const noexcept {
			return (_elems + N);
		}
		~static_vector() {
			if (std::is_trivially_destructible<T>::value) return;
			for (size_t i = 0; i < _size; i++) {
				_elems[i]->~T();
			}
		}
	};

} // namespace libstra
