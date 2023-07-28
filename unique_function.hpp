#if !defined(UNIQUE_FN_H)
#define UNIQUE_FN_H

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

struct invalid_function_access {
	constexpr const char *what() noexcept {
		return "Attempted to call an invalid unique_function object";
	}
};

template <class Func> class unique_function;

template <class R, typename... Args> class unique_function<R(Args...)> {
public:
	constexpr unique_function() noexcept = default;
	template <class F>
	constexpr unique_function(F &&f) : _isSmall(sizeof(f) < 9) {
		using _Raw = std::remove_reference_t<F>;
		if (_isSmall) new (_mem) _Raw(std::forward<_Raw>(f));
		else _ptr = new _Raw(std::forward<_Raw>(f));
		if (_isSmall && !std::is_trivially_constructible_v<_Raw>)
			_deleter = [](void *ptr) { ((_Raw *)ptr)->~_Raw(); };
		else _deleter = [](void *ptr) { delete (_Raw *)ptr; };
		_invoke = [](void *f, Args &&...args) {
			return (*(_Raw *)f)(forward<Args>(args)...);
		};
	}
	unique_function(R (*f)(Args...)) : _isSmall(true), _ptr(f) {
		using F = R (*)(Args...);
		_invoke = [](void *f, Args &&...args) {
			return (*(F *)f)(forward<Args>(args)...);
		};
	}
	unique_function(const unique_function &) = delete;
	constexpr unique_function(unique_function &&other) noexcept :
		_ptr(other._ptr),
		_isSmall(other._isSmall),
		_deleter(other._deleter),
		_invoke(other._invoke) {
		other.reset();
	}
	~unique_function() {
		if (!_deleter) return;
		if (_isSmall) _deleter(_mem);
		else _deleter(_ptr);
	}
	constexpr bool has_value() const noexcept { return _isSmall || (bool)_ptr; }
	constexpr operator bool() const noexcept { return has_value(); }
	R operator()(Args &&...args) {
		if (!_invoke) throw invalid_function_access{};
		return _invoke(_isSmall ? _mem : _ptr, forward<Args>(args)...);
	}

private:
	inline void reset() noexcept {
		_isSmall = false;
		_invoke = nullptr;
		_deleter = nullptr;
		_ptr = nullptr;
	}
	union {
		void *_ptr = nullptr;
		char _mem[sizeof(_ptr)];
	};
	bool _isSmall = false;
	void (*_deleter)(void *ptr) = nullptr;
	R (*_invoke)(void *, Args &&...) = nullptr;
};

#endif // UNIQUE_FN_H
