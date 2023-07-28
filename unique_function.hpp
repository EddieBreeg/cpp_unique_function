#if !defined(UNIQUE_FN_H)
#define UNIQUE_FN_H

struct _FuncState {
	union {
		void *_ptr = nullptr;
		char _mem[sizeof(_ptr)];
	};
	bool _isSmall = false;
	void (*_deleter)(void *ptr) = nullptr;
	bool has_value() const { return _isSmall || (bool)_ptr; }

	_FuncState() = default;
	template <class F> _FuncState(F &&f) : _isSmall(sizeof(f) < 9) {
		using _Raw = std::remove_reference_t<F>;
		if (_isSmall) new (_mem) _Raw(std::forward<_Raw>(f));
		else _ptr = new _Raw(std::forward<_Raw>(f));
		if (_isSmall && !std::is_trivially_constructible_v<_Raw>)
			_deleter = [](void *ptr) { ((_Raw *)ptr)->~_Raw(); };
		else _deleter = [](void *ptr) { delete (_Raw *)ptr; };
	}
	template <class T> T *get_as() { return _isSmall ? (T *)_mem : (T *)_ptr; }
	~_FuncState() {
		if (!_deleter) return;
		if (_isSmall) _deleter(_mem);
		else _deleter(_ptr);
	}
};

template <class F> class unique_function;
template <class R, typename... Args> class unique_function<R(Args...)> {
	_FuncState _s;

public:
	template <class Func> unique_function(Func &&f) : _s(f) {}
};

#endif // UNIQUE_FN_H
