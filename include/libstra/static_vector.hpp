#pragma once

#include <cstddef>
#include <type_traits>
#include <stdexcept>
#include <libstra/utility.hpp>
#include <iterator>
#include "internal/attrib_macros.h"
#include <initializer_list>

namespace libstra {

	/**
	 * A stack allocated dynamic array
	 * @tparam T: The type of values in the array. T may not be a reference
	 * type, use std::reference_wrapper to store references
	 * @tparam N: The fixed capacity of the vector
	 */
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
		using value_type = T;
		using reference = T &;
		using const_reference = const T &;
		using size_type = size_t;
		using iterator = iterator_base<false>;
		using const_iterator = iterator_base<true>;
		using difference_type = typename iterator::difference_type;

		// Constructs an empty static_vector
		static_vector() = default;
		/**
		 * Constructs a static_vector with n default-constructed elements
		 * @exception May throw std::out_of_range if n > capacity()
		 */
		static_vector(size_t n) : _size(n) {
#ifndef NDEBUG
			if _unlikely (n > N)
				throw std::out_of_range("Attempted to construct a static "
										"vector with too many elements");
#endif
			for (size_t i = 0; i < n; i++)
				new (_elems[i]._buf) T();
		}
		/**
		 * Constructs a static_vector with n constructed elements, copied from
		 * val. This constructor only participates in overload resolution if U
		 * is convertible to T
		 * @param n: The number of elements to construct
		 * @param val: The default value for the elements to be constructed
		 * @exception May throw std::out_of_range if n > capacity()
		 */
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
		/**
		 * Constructs a static_vector from a pair of iterators.
		 * This constructor only participates in overload resolution if
		 * std::iterator_traits<Iter>::difference_type is present
		 * @param start: An iterator to the first element in the array
		 * @param end: An iterator to past the last element in the array
		 * @warning If end is not reachable from start, behaviour is undefined
		 * @exception May throw std::out_of_range if std::distance(start, end) >
		 * capacity()
		 */
		template <class Iter,
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
		/**
		 * Constructs a static_vector from an initializer list. This constructor
		 * only participates in overload resolution if U is convertible to T
		 * @exception May throw std::out_of_range if il.size() > capacity()
		 */
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
		/**
		 * Copy constructor
		 */
		static_vector(const static_vector &other) {
			_assume(other._size <= N);
			for (; _size < other._size; ++_size)
				new (_elems[_size]._buf) T(other[_size]);
		}
		/**
		 * Move constructor: moves all elements contained in other.
		 * @note Because the vector is statically allocated on the stack,
		 * the complexity is the same as for a copy, that is: linear with
		 * other.size()
		 */
		static_vector(static_vector &&other) noexcept {
			_assume(other._size <= N);
			for (; _size < other._size; ++_size)
				new (_elems[_size]._buf) T((T &&)(other[_size]));
			other._size = 0;
		}
		/**
		 * Move assignement
		 * @note Because the vector is statically allocated on the stack,
		 * the complexity is the same as for a copy, that is: linear with
		 * other.size()
		 */
		static_vector &operator=(static_vector &&other) {
			swap(other);
			return *this;
		}
		/**
		 * Copy assignement
		 */
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
		/**
		 * Appends a value to the vector. The new value is assigned using
		 * perfect forwarding
		 * @exception May throw std::out_of_range if size() == capacity()
		 */
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
		/**
		 * Constructs a value in place. The new element is constructed using a
		 * placement new expression
		 * @param args: A list of arguments to forward to the constructor of the
		 * new object
		 * @exception May throw std::out_of_range if size() == capacity()
		 */
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
		/**
		 * Removes the last element in the vector
		 */
		void pop_back() {
#ifndef NDEBUG
			if (!_size)
				throw std::out_of_range(
					"Called pop_back() on an empty static_vector");
#endif
			--_size;
			if (!std::is_trivially_destructible<T>::value) _elems[_size]->~T();
		}
		/**
		 * Returns a reference to the first element in the vector
		 * @exception May throw std::out_of_range if the vector is empty
		 */
		[[nodiscard]]
		T &front() & {
#ifndef NDEBUG
			if _unlikely (!_size)
				throw std::out_of_range(
					"Called front() on an empty static_vector");
#endif
			return *(_elems[0]);
		}
		/**
		 * Returns a reference to the first element in the vector
		 * @exception May throw std::out_of_range if the vector is empty
		 */
		[[nodiscard]]
		const T &front() const & {
#ifndef NDEBUG
			if _unlikely (!_size)
				throw std::out_of_range(
					"Called front() on an empty static_vector");
#endif
			return *(_elems[0]);
		}
		/**
		 * Returns a reference to the first element in the vector
		 * @exception May throw std::out_of_range if the vector is empty
		 */
		[[nodiscard]]
		T &&front() && {
#ifndef NDEBUG
			if _unlikely (!_size)
				throw std::out_of_range(
					"Called front() on an empty static_vector");
#endif
			return *(_elems[0]);
		}
		/**
		 * Returns a reference to the first element in the vector
		 * @exception May throw std::out_of_range if the vector is empty
		 */
		[[nodiscard]]
		const T &&front() const && {
#ifndef NDEBUG
			if _unlikely (!_size)
				throw std::out_of_range(
					"Called front() on an empty static_vector");
#endif
			return *(_elems[0]);
		}
		/**
		 * Returns a reference to the last element in the vector
		 * @exception May throw std::out_of_range if the vector is empty
		 */
		[[nodiscard]]
		T &back() & {
#ifndef NDEBUG
			if _unlikely (!_size)
				throw std::out_of_range(
					"Called back() on an empty static_vector");
#endif
			return *(_elems[_size - 1]);
		}
		/**
		 * Returns a reference to the last element in the vector
		 * @exception May throw std::out_of_range if the vector is empty
		 */
		[[nodiscard]]
		const T &back() const & {
#ifndef NDEBUG
			if _unlikely (!_size)
				throw std::out_of_range(
					"Called back() on an empty static_vector");
#endif
			return *(_elems[_size - 1]);
		}
		/**
		 * Returns a reference to the last element in the vector
		 * @exception May throw std::out_of_range if the vector is empty
		 */
		[[nodiscard]]
		T &&back() && {
#ifndef NDEBUG
			if _unlikely (!_size)
				throw std::out_of_range(
					"Called back() on an empty static_vector");
#endif
			return *(_elems[_size - 1]);
		}
		/**
		 * Returns a reference to the last element in the vector
		 * @exception May throw std::out_of_range if the vector is empty
		 */
		[[nodiscard]]
		const T &&back() const && {
#ifndef NDEBUG
			if _unlikely (!_size)
				throw std::out_of_range(
					"Called back() on an empty static_vector");
#endif
			return *(_elems[_size - 1]);
		}
		/**
		 * Returns an iterator to the first element in the vector. The behaviour
		 * of said iterator is undefined if the vector was empty
		 */
		[[nodiscard]]
		iterator begin() noexcept {
			return iterator(_elems);
		}
		/**
		 * Returns an iterator to the first element in the vector. The behaviour
		 * of said iterator is undefined if the vector was empty
		 */
		[[nodiscard]]
		const_iterator begin() const noexcept {
			return _elems;
		}
		/**
		 * Returns an iterator to past the last element in the vector
		 */
		[[nodiscard]]
		iterator end() noexcept {
			return iterator(_elems + _size);
		}
		/**
		 * Returns an iterator to past the last element in the vector
		 */
		[[nodiscard]]
		const_iterator end() const noexcept {
			return (_elems + _size);
		}
		/**
		 * Accesses an element in the vector
		 * @param i: The index of the element to access
		 * @exception May throw std::out_of_range if i >= size()
		 */
		[[nodiscard]]
		inline T &
		operator[](size_t i) & {
#ifndef NDEBUG
			if _unlikely (i >= _size)
				throw std::out_of_range("static_vector subscript out of range");
#endif
			return *(_elems[i]);
		}
		/**
		 * Accesses an element in the vector
		 * @param i: The index of the element to access
		 * @exception May throw std::out_of_range if i >= size()
		 */
		[[nodiscard]]
		inline const T &
		operator[](size_t i) const & {
#ifndef NDEBUG
			if _unlikely (i >= _size)
				throw std::out_of_range("static_vector subscript out of range");
#endif
			return *(_elems[i]);
		}
		/**
		 * Accesses an element in the vector
		 * @param i: The index of the element to access
		 * @exception May throw std::out_of_range if i >= size()
		 */
		[[nodiscard]]
		inline T &&
		operator[](size_t i) && {
#ifndef NDEBUG
			if _unlikely (i >= _size)
				throw std::out_of_range("static_vector subscript out of range");
#endif
			return *(_elems[i]);
		}
		/**
		 * Accesses an element in the vector
		 * @param i: The index of the element to access
		 * @exception May throw std::out_of_range if i >= size()
		 */
		[[nodiscard]]
		inline const T &&
		operator[](size_t i) const && {
#ifndef NDEBUG
			if _unlikely (i >= _size)
				throw std::out_of_range("static_vector subscript out of range");
#endif
			return *(_elems[i]);
		}

		/**
		 * @returns A raw pointer to the underlying data buffer. This pointer is
		 * always valid, even when the vector is empty
		 */
		[[nodiscard]]
		T *data() noexcept {
			return (T *)_elems;
		}
		/**
		 * @returns A raw pointer to the underlying data buffer. This pointer is
		 * always valid, even when the vector is empty
		 */
		[[nodiscard]]
		const T *data() const noexcept {
			return (T *)_elems;
		}
		/**
		 * @return The current number of elements in the vector
		 */
		[[nodiscard]]
		inline size_t size() const noexcept {
			return _size;
		}
		/**
		 * @returns size() == 0
		 */
		[[nodiscard]]
		inline bool is_empty() const noexcept {
			return _size == 0;
		}
		/**
		 * @returns N
		 */
		[[nodiscard]]
		static constexpr inline size_t capacity() noexcept {
			return N;
		}
		/**
		 * Clears the contents of the vector. If T is not trivially
		 * destructible, all the objects present in the vector will get
		 * destroyed manually
		 */
		void clear() {
			if (std::is_trivially_constructible<T>::value) {
				_size = 0;
				return;
			}
			for (size_t i = 0; i < _size; ++i)
				_elems[i]->~T();
			_size = 0;
		}
		/**
		 * Changes the size of the vector. If newSize < size() and T is not
		 * trivially destructible, all the extra objects will get destroyed
		 * manually
		 * @exception May throw std::out_of_range if newSize > capacity()
		 */
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

		/**
		 * Changes the size of the vector. If newSize < size() and T is not
		 * trivially destructible, all the extra objects will get destroyed
		 * manually. If newSize > size(), the newly created elements will be
		 * initialized with val
		 * @exception May throw std::out_of_range if newSize > capacity()
		 */
		template <class U,
				  std::enable_if_t<std::is_convertible<U, T>::value, int> = 0>
		void resize(size_t newSize, const U &val) {
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
		/**
		 * Swaps the contents of *this with other
		 */
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

		/**
		 * Returns true if other.size() == size() and if all elements in both
		 * vectors are equal
		 */
		[[nodiscard]]
		bool
		operator==(const static_vector &other) const noexcept {
			if (_size != other._size) return false;
			for (size_t i = 0; i < _size; ++i)
				if (*(_elems[i]) != *(other._elems[i])) return false;
			return true;
		}
		/**
		 * If T is not trivially destructible, calls T::~T() on all the elements
		 * in the vector. Otherwise, does nothing
		 */
		~static_vector() {
			if (std::is_trivially_destructible<T>::value) return;
			for (size_t i = 0; i < _size; i++) {
				_elems[i]->~T();
			}
		}
	};

} // namespace libstra
