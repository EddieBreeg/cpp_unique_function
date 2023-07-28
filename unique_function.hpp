#if !defined(UNIQUE_FN_H)
#define UNIQUE_FN_H

#include <type_traits>
#include <utility>
#include <typeinfo>

template <class T>
constexpr T &&forward(std::remove_reference_t<T> &t) noexcept {
	return static_cast<T &&>(t);
}
template <class T>
constexpr T &&forward(std::remove_reference_t<T> &&t) noexcept {
	return static_cast<T &&>(t);
}
// required in case we need to forward an empty argument pack
constexpr void forward() noexcept {}

// Exception thrown when the call operator is used on an invalid unique_function
// object
struct invalid_function_access {
	constexpr const char *what() noexcept {
		return "Attempted to call an invalid unique_function object";
	}
};

/**
 * Represents a generic non-copyable callable object
 * @tparam Func: The underlying function type
 */
template <class Func> class unique_function;

template <class Func> class function_ref;

template <class R, typename... Args> class function_ref<R(Args...)> {
public:
	function_ref() = default;
	function_ref(R (*f)(Args...)) : _ptr((void *)f) {
		using F = decltype(f);
		_type_info = []() -> const std::type_info & {
			return typeid(R(Args...));
		};
		_invoke = [](void *f, Args &&...args) {
			return ((F)f)(forward<Args>(args)...);
		};
	}
	template <class Func> function_ref(Func &&f) {
		using _Raw_t = std::remove_reference_t<Func>;
		// the copy constructor never gets called, we have to do the copy here
		if (std::is_same_v<_Raw_t, function_ref>) {
			*this = std::move(f);
			return;
		}
		_ptr = &f;
		_type_info = []() -> const std::type_info & { return typeid(_Raw_t); };
		_invoke = [](void *f, Args &&...args) {
			return (*(_Raw_t *)f)(forward<Args>(args)...);
		};
	}
	const std::type_info &target_type() const noexcept { return _type_info(); }
	template <class T> T *target() noexcept {
		return typeid(T) == _type_info() ? (T *)_ptr : nullptr;
	}

	R operator()(Args &&...args) {
		if (!_ptr) throw invalid_function_access{};
		return _invoke(_ptr, forward<Args>(args)...);
	}

private:
	void *_ptr = nullptr;
	R (*_invoke)(void *, Args &&...) = nullptr;
	const std::type_info &(*_type_info)() = []() -> const std::type_info & {
		return typeid(void);
	};
};

template <class R, typename... Args> class unique_function<R(Args...)> {
public:
	/* Default constructor: constructs an invalid function object */
	constexpr unique_function() noexcept = default;

	/**
	 * Contructs a unique_function instance from a function pointer
	 */
	unique_function(R (*f)(Args...)) : _ptr((void *)f), _isSmall(true) {
		using F = R (*)(Args...);
		_tid = &typeid(R(Args...));
		_invoke = [](void *f, Args &&...args) {
			return (*(F *)f)(forward<Args>(args)...);
		};
	}

	/**
	 * Contructs a unique_function instance from a generic callable object
	 */
	template <class F>
	constexpr unique_function(F &&f) : _isSmall(sizeof(f) < 9) {
		using _Raw = std::remove_reference_t<F>;
		_tid = &typeid(_Raw);
		if (_isSmall) new (_mem) _Raw(std::forward<_Raw>(f));
		else _ptr = new _Raw(std::forward<_Raw>(f));
		if (_isSmall && !std::is_trivially_constructible_v<_Raw>)
			_deleter = [](void *ptr) { ((_Raw *)ptr)->~_Raw(); };
		else _deleter = [](void *ptr) { delete (_Raw *)ptr; };
		_invoke = [](void *f, Args &&...args) {
			return (*(_Raw *)f)(forward<Args>(args)...);
		};
	}
	unique_function(const unique_function &) = delete;

	/**
	 * Constructs a unique_function by moving another
	 * @param other: The object to move from. This object is left in an invalid
	 * state, such that a call to has_value() returns false
	 */
	constexpr unique_function(unique_function &&other) noexcept :
		_ptr(other._ptr),
		_isSmall(other._isSmall),
		_deleter(other._deleter),
		_invoke(other._invoke),
		_tid(other._tid) {
		other.reset();
	}
	/**
	 * Indicates whether *this is a valid function object
	 * @returns true if *this contains a valid callable object, false otherwise
	 */
	constexpr bool has_value() const noexcept { return (bool)_invoke; }
	/**
	 * Casts *this to a boolean
	 * @return Same as has_value()
	 */
	constexpr operator bool() const noexcept { return has_value(); }

	/**
	 * Swaps the contents of *this with other
	 * @param other: The other function object to swap the contents with
	 */
	void swap(unique_function &other) {
		std::swap(_ptr, other._ptr);
		std::swap(_invoke, other._invoke);
		std::swap(_deleter, other._deleter);
		std::swap(_isSmall, other._isSmall);
		std::swap(_tid, other._tid);
	}
	unique_function &operator=(unique_function &) = delete;
	unique_function &operator=(unique_function &&other) {
		swap(other);
		return *this;
	}
	/**
	 * Attempts to invoke the function
	 * @param args: The arguments to forward to the underlying callable object
	 * @throw Throws invalid_function_access if *this does not contain a valid
	 * callable object, that is if has_value() returns false
	 */
	R operator()(Args &&...args) {
		if (!_invoke) throw invalid_function_access{};
		return _invoke(_isSmall ? _mem : _ptr, forward<Args>(args)...);
	}
	/**
	 * @return typeid(T) if the stored function has type T, otherwise
	 * typeid(void)
	 */
	const std::type_info &target_type() const noexcept { return *_tid; }
	/**
	 * @returns A pointer to the stored function if target_type() == typeid(T),
	 * otherwise a null pointer.
	 */
	template <class T> T *target() noexcept {
		if (*_tid != typeid(T)) return nullptr;
		return _isSmall ? (T *)_mem : (T *)_ptr;
	}
	/**
	 * Destroys the function object
	 */
	~unique_function() {
		if (!_deleter) return;
		if (_isSmall) _deleter(_mem);
		else _deleter(_ptr);
	}

private:
	inline void reset() noexcept {
		_isSmall = false;
		_invoke = nullptr;
		_deleter = nullptr;
		_ptr = nullptr;
		_tid = &typeid(void);
	}
	union {
		void *_ptr = nullptr;
		char _mem[sizeof(_ptr)];
	};
	void (*_deleter)(void *ptr) = nullptr;
	R (*_invoke)(void *, Args &&...) = nullptr;
	const std::type_info *_tid = &typeid(void);
	bool _isSmall = false;
};

#endif // UNIQUE_FN_H
