#pragma once

#include <cstddef>
#include <type_traits>
#include <stdexcept>
#include <libstra/utility.hpp>
#include <iterator>
#include "internal/attrib_macros.h"
#include <initializer_list>

namespace libstra {

	template <class T, size_t N>
	class static_vector {
		static_assert(!std::is_reference<T>::value,
					  "T must not be a reference type");

	private:
		struct _storage {
			alignas(T) char _buf[sizeof(T)];
			T *ptr() { return (T *)_buf; }
			T *operator->() { return (T *)_buf; }
			const T *operator->() const { return (T *)_buf; }
			T &operator*() { return *(T *)_buf; }
			const T &operator*() const { return *(T *)_buf; }
		};
		_storage _elems[N];
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
			[[nodiscard]]
			pointer
			operator->() noexcept {
				return (pointer)_ptr;
			}
			[[nodiscard]]
			pointer get() noexcept {
				return (pointer)_ptr;
			}
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
#ifndef NDEBUG
			if _unlikely (n > N)
				throw std::out_of_range("Attempted to construct a static "
										"vector with too many elements");
#endif
			for (size_t i = 0; i < n; i++)
				new (_elems[i]._buf) T();
		}
		template <class U,
				  std::enable_if_t<std::is_convertible<U, T>::value, int> = 0>
		static_vector(size_t n, const U &val) : _size(n) {
#ifndef NDEBUG
			if _unlikely (n > N)
				throw std::out_of_range("Attempted to construct a static "
										"vector with too many elements");
#endif
			for (size_t i = 0; i < n; i++)
				new (_elems[i]._buf) T(val);
		}
		template <class Iter,
				  // only enable if Iter is a valid iterator type
				  typename std::iterator_traits<Iter>::difference_type = 0>
		static_vector(Iter start, Iter end) : _size(std::distance(start, end)) {
#ifndef NDEBUG
			if _unlikely (_size > N)
				throw std::out_of_range("Attempted to construct a static "
										"vector with too many elements");
#endif
			for (auto p = _elems; start != end; ++start, ++p)
				new (p->_buf) T(*start);
		}
		template <class U,
				  std::enable_if_t<std::is_convertible<U, T>::value, int> = 0>
		static_vector(std::initializer_list<U> il) : _size(il.size()) {
#ifndef NDEBUG
			if _unlikely (_size > N)
				throw std::out_of_range("Attempted to construct a static "
										"vector with too many elements");
#endif
			auto it = il.begin();
			for (auto p = _elems; it != il.end(); ++it, ++p)
				new (p->_buf) T(*it);
		}

		static_vector(const static_vector &other) {
			_assume(other._size <= N);
			for (; _size < other._size; ++_size)
				new (_elems[_size]._buf) T(other[_size]);
		}
		static_vector(static_vector &&other) noexcept {
			_assume(other._size <= N);
			for (; _size < other._size; ++_size)
				new (_elems[_size]._buf) T((T &&)(other[_size]));
			other._size = 0;
		}

		static_vector &operator=(static_vector &&other) {
			swap(other);
			return *this;
		}
		static_vector &operator=(const static_vector &other) {
			_assume(_size <= N && other._size <= N);
			for (size_t i = 0; i < other._size; ++i)
				*(_elems[i]) = *(other._elems[i]);
			if (!std::is_trivially_destructible<T>::value) {
				for (size_t i = other._size; i < _size; ++i)
					_elems[i]->~T();
			}
			_size = other._size;
			return *this;
		}

		template <class U>
		iterator push_back(U &&val) {
#ifndef NDEBUG
			if _unlikely (_size == N)
				throw std::out_of_range(
					"Attempted to add an element to a full static vector");
#endif
			iterator it = end();
			*(_elems[_size++]) = std::forward<U>(val);
			return it;
		}

		template <class... Args>
		iterator emplace_back(Args &&...args) {
#ifndef NDEBUG
			if _unlikely (_size == N)
				throw std::out_of_range(
					"Attempted to add an element to a full static vector");
#endif
			iterator it = end();
			new (_elems[_size++]._buf) T(libstra::forward<Args>(args)...);
			return it;
		}
		void pop_back() {
#ifndef NDEBUG
			if (!_size)
				throw std::out_of_range(
					"Called pop_back() on an empty static_vector");
#endif
			--_size;
			if (!std::is_trivially_destructible<T>::value) _elems[_size]->~T();
		}

		[[nodiscard]]
		T &front() & {
#ifndef NDEBUG
			if _unlikely (!_size)
				throw std::out_of_range(
					"Called front() on an empty static_vector");
#endif
			return *(_elems[0]);
		}
		[[nodiscard]]
		const T &front() const & {
#ifndef NDEBUG
			if _unlikely (!_size)
				throw std::out_of_range(
					"Called front() on an empty static_vector");
#endif
			return *(_elems[0]);
		}
		[[nodiscard]]
		T &&front() && {
#ifndef NDEBUG
			if _unlikely (!_size)
				throw std::out_of_range(
					"Called front() on an empty static_vector");
#endif
			return *(_elems[0]);
		}
		[[nodiscard]]
		const T &&front() const && {
#ifndef NDEBUG
			if _unlikely (!_size)
				throw std::out_of_range(
					"Called front() on an empty static_vector");
#endif
			return *(_elems[0]);
		}
		[[nodiscard]]
		T &back() & {
#ifndef NDEBUG
			if _unlikely (!_size)
				throw std::out_of_range(
					"Called back() on an empty static_vector");
#endif
			return *(_elems[_size - 1]);
		}
		[[nodiscard]]
		const T &back() const & {
#ifndef NDEBUG
			if _unlikely (!_size)
				throw std::out_of_range(
					"Called back() on an empty static_vector");
#endif
			return *(_elems[_size - 1]);
		}
		[[nodiscard]]
		T &&back() && {
#ifndef NDEBUG
			if _unlikely (!_size)
				throw std::out_of_range(
					"Called back() on an empty static_vector");
#endif
			return *(_elems[_size - 1]);
		}
		[[nodiscard]]
		const T &&back() const && {
#ifndef NDEBUG
			if _unlikely (!_size)
				throw std::out_of_range(
					"Called back() on an empty static_vector");
#endif
			return *(_elems[_size - 1]);
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
			return iterator(_elems + _size);
		}
		[[nodiscard]]
		const_iterator end() const noexcept {
			return (_elems + _size);
		}

		[[nodiscard]]
		inline T &
		operator[](size_t i) & {
#ifndef NDEBUG
			if _unlikely (i >= _size)
				throw std::out_of_range("static_vector subscript out of range");
#endif
			return *(_elems[i]);
		}
		[[nodiscard]]
		inline const T &
		operator[](size_t i) const & {
#ifndef NDEBUG
			if _unlikely (i >= _size)
				throw std::out_of_range("static_vector subscript out of range");
#endif
			return *(_elems[i]);
		}
		[[nodiscard]]
		inline T &&
		operator[](size_t i) && {
#ifndef NDEBUG
			if _unlikely (i >= _size)
				throw std::out_of_range("static_vector subscript out of range");
#endif
			return *(_elems[i]);
		}
		[[nodiscard]]
		inline const T &&
		operator[](size_t i) const && {
#ifndef NDEBUG
			if _unlikely (i >= _size)
				throw std::out_of_range("static_vector subscript out of range");
#endif
			return *(_elems[i]);
		}

		[[nodiscard]]
		T *data() noexcept {
			return (T *)_elems;
		}
		[[nodiscard]]
		const T *data() const noexcept {
			return (T *)_elems;
		}

		[[nodiscard]]
		inline size_t size() const noexcept {
			return _size;
		}
		[[nodiscard]]
		inline bool is_empty() const noexcept {
			return _size == 0;
		}
		[[nodiscard]]
		static constexpr inline size_t capacity() noexcept {
			return N;
		}

		void clear() {
			if (std::is_trivially_constructible<T>::value) {
				_size = 0;
				return;
			}
			for (size_t i = 0; i < _size; ++i)
				_elems[i]->~T();
			_size = 0;
		}
		void resize(size_t newSize) {
#ifndef NDEBUG
			if _unlikely (newSize > N) {
				throw std::out_of_range(
					"New static_vector size was out of range");
			}
#endif
			for (size_t i = _size; i < newSize; ++i)
				new (_elems[i]._buf) T();
			if (!std::is_trivially_destructible<T>::value) {
				for (size_t i = newSize; i < _size; ++i)
					_elems[i]->~T();
			}
			_size = newSize;
		}

		template <class U,
				  std::enable_if_t<std::is_convertible<U, T>::value, int> = 0>
		void resize(size_t newSize, const U &val = U()) {
#ifndef NDEBUG
			if _unlikely (newSize > N) {
				throw std::out_of_range(
					"New static_vector size was out of range");
			}
#endif
			for (size_t i = _size; i < newSize; ++i)
				new (_elems[i]._buf) T(val);
			if (!std::is_trivially_destructible<T>::value) {
				for (size_t i = newSize; i < _size; ++i)
					_elems[i]->~T();
			}
			_size = newSize;
		}

		void swap(static_vector &other) {
			_assume(_size <= N && other._size <= N);
			const size_t m = _size < other._size ? _size : other._size;
			for (size_t i = 0; i < m; i++)
				std::swap(*(_elems[i]), *(other._elems[i]));
			for (size_t i = m; i < _size; ++i)
				new (other._elems[i]._buf) T(std::move(*(_elems[i])));
			for (size_t i = m; i < other._size; ++i)
				new (_elems[i]._buf) T(std::move(*(other._elems[i])));
			std::swap(_size, other._size);
		}

		[[nodiscard]]
		bool
		operator==(const static_vector &other) const noexcept {
			if (_size != other._size) return false;
			for (size_t i = 0; i < _size; ++i)
				if (*(_elems[i]) != *(other._elems[i])) return false;
			return true;
		}

		~static_vector() {
			if (std::is_trivially_destructible<T>::value) return;
			for (size_t i = 0; i < _size; i++) {
				_elems[i]->~T();
			}
		}
	};

} // namespace libstra
