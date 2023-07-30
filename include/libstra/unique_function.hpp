#if !defined(UNIQUE_FN_H)
#define UNIQUE_FN_H

#include <type_traits>
#include <utility>
#include <typeinfo>

namespace libstra {
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

	// Exception thrown when the call operator is used on an invalid
	// unique_function object
	struct invalid_function_access {
		constexpr const char *what() noexcept {
			return "Attempted to call an invalid unique_function object";
		}
	};

	/**
	 * Represents a generic non-copyable callable object
	 * @tparam Func: The underlying function type
	 */
	template <class Func>
	class unique_function;

	/**
	 * Represents a reference to a generic callable object
	 * @tparam Func: The underlying function type
	 * @warning If the referenced object reaches its end of life before the
	 * reference, the reference will become dangling
	 */
	template <class Func>
	class function_ref;

	template <class R, typename... Args>
	class function_ref<R(Args...)> {
	public:
		/** Constructs an empty function_ref */
		constexpr function_ref() noexcept = default;
		/** Constructs the reference from a function pointer */
		function_ref(R (*f)(Args...)) noexcept : _ptr((void *)f) {
			using F = decltype(f);
			_type_info = []() -> const std::type_info & {
				return typeid(R(Args...));
			};
			_invoke = [](void *f, Args &&...args) {
				return ((F)f)(forward<Args>(args)...);
			};
		}
		function_ref(const function_ref &) = default;
		/** Constructs the reference from a generic callable object */
		template <
			class Func,
			typename = std::enable_if_t<
				!std::is_same<std::decay_t<Func>, function_ref>::value, bool>>
		function_ref(Func &&f) noexcept {
			using _Raw_t = std::remove_reference_t<Func>;
			_ptr = &f;
			_type_info = []() -> const std::type_info & {
				return typeid(_Raw_t);
			};
			_invoke = [](void *f, Args &&...args) {
				return (*(_Raw_t *)f)(forward<Args>(args)...);
			};
		}
		/**
		 * @return typeid(T) if the target function has type T, otherwise
		 * typeid(void)
		 */
		[[nodiscard]]
		const std::type_info &target_type() const noexcept {
			return _type_info();
		}
		/**
		 * @return The pointer to the target object if target_type() ==
		 * typeid(T), otherwise nullptr
		 */
		template <class T>
		[[nodiscard]]
		T *target() noexcept {
			return typeid(T) == _type_info() ? (T *)_ptr : nullptr;
		}

		/**
		 * Attempts to invoke the function
		 * @param args: The arguments to forward to the underlying callable
		 * object
		 * @throw Throws invalid_function_access if *this does not contain a
		 * valid callable object, that is if has_value() returns false
		 */
		R operator()(Args &&...args) {
			if (!_ptr) throw invalid_function_access{};
			return _invoke(_ptr, forward<Args>(args)...);
		}

		/** @return true if *this is not empty, false otherwise */
		[[nodiscard]]
		inline constexpr bool has_value() const noexcept {
			return (bool)_ptr;
		}
		/** @return true if *this is not empty, false otherwise */
		[[nodiscard]] inline constexpr operator bool() const noexcept {
			return has_value();
		}

	private:
		void *_ptr = nullptr;
		R (*_invoke)(void *, Args &&...) = nullptr;
		const std::type_info &(*_type_info)() = []() -> const std::type_info & {
			return typeid(void);
		};
	};

	template <class R, typename... Args>
	class unique_function<R(Args...)> {
	public:
		/* Default constructor: constructs an invalid function object */
		constexpr unique_function() noexcept = default;

		/**
		 * Contructs a unique_function instance from a function pointer
		 */
		unique_function(R (*f)(Args...)) noexcept :
			_ptr((void *)f), _isSmall(true) {
			using F = R (*)(Args...);
			_tid = &typeid(R(Args...));
			_invoke = [](void *f, Args &&...args) {
				return (*(F *)f)(forward<Args>(args)...);
			};
		}

		/**
		 * Contructs a unique_function instance from a generic callable object
		 */
		template <class F, typename = std::enable_if_t<!std::is_same<
							   std::decay_t<F>, unique_function>::value>>
		unique_function(F &&f) : _isSmall(sizeof(f) <= sizeof(_mem)) {
			using _Raw = std::remove_reference_t<F>;
			_tid = &typeid(_Raw);
			if (_isSmall) new (_mem) _Raw(std::forward<_Raw>(f));
			else _ptr = new _Raw(std::forward<_Raw>(f));
			if (_isSmall && !std::is_trivially_constructible<_Raw>::value)
				_deleter = [](void *ptr) { ((_Raw *)ptr)->~_Raw(); };
			else _deleter = [](void *ptr) { delete (_Raw *)ptr; };
			_invoke = [](void *f, Args &&...args) {
				return (*(_Raw *)f)(forward<Args>(args)...);
			};
		}
		unique_function(const unique_function &) = delete;

		/**
		 * Constructs a unique_function by moving another
		 * @param other: The object to move from. This object is left in an
		 * invalid state, such that a call to has_value() returns false
		 */
		unique_function(unique_function &&other) noexcept :
			_ptr(other._ptr),
			_isSmall(other._isSmall),
			_deleter(other._deleter),
			_invoke(other._invoke),
			_tid(other._tid) {
			other.reset();
		}
		/**
		 * Indicates whether *this is a valid function object
		 * @returns true if *this contains a valid callable object, false
		 * otherwise
		 */
		[[nodiscard]]
		constexpr bool has_value() const noexcept {
			return (bool)_invoke;
		}
		/**
		 * Casts *this to a boolean
		 * @return Same as has_value()
		 */
		[[nodiscard]] constexpr operator bool() const noexcept {
			return has_value();
		}

		/**
		 * Swaps the contents of *this with other
		 * @param other: The other function object to swap the contents with
		 */
		void swap(unique_function &other) noexcept {
			std::swap(_ptr, other._ptr);
			std::swap(_invoke, other._invoke);
			std::swap(_deleter, other._deleter);
			std::swap(_isSmall, other._isSmall);
			std::swap(_tid, other._tid);
		}
		unique_function &operator=(unique_function &) = delete;
		unique_function &operator=(unique_function &&other) noexcept {
			swap(other);
			return *this;
		}
		/**
		 * Attempts to invoke the function
		 * @param args: The arguments to forward to the underlying callable
		 * object
		 * @throw Throws invalid_function_access if *this does not contain a
		 * valid callable object, that is if has_value() returns false
		 */
		R operator()(Args &&...args) {
			if (!_invoke) throw invalid_function_access{};
			return _invoke(_isSmall ? _mem : _ptr, forward<Args>(args)...);
		}
		/**
		 * @return typeid(T) if the stored function has type T, otherwise
		 * typeid(void)
		 */
		[[nodiscard]]
		const std::type_info &target_type() const noexcept {
			return *_tid;
		}
		/**
		 * @returns A pointer to the stored function if target_type() ==
		 * typeid(T), otherwise a null pointer.
		 */
		template <class T>
		[[nodiscard]]
		T *target() noexcept {
			if (*_tid != typeid(T)) return nullptr;
			return _isSmall ? (T *)_mem : (T *)_ptr;
		}
		/** Returns a reference to the stored function object
		 * @return A reference to the stored callable object if target_type() ==
		 * T, otherwise an empty reference
		 */
		template <class T>
		[[nodiscard]]
		function_ref<R(Args...)> get_ref() {
			if (*_tid != typeid(T)) return {};
			return *target<T>();
		}
		/**
		 * Destroys the function object
		 */
		~unique_function() {
			if (_isSmall) _deleter(_mem);
			else _deleter(_ptr);
		}

	private:
		inline void reset() noexcept {
			_isSmall = false;
			_deleter = [](void *) {};
			_ptr = nullptr;
			_invoke = nullptr;
			_tid = &typeid(void);
		}
		union {
			void *_ptr = nullptr;
			char _mem[sizeof(void *)];
		};
		void (*_deleter)(void *ptr) = [](void *) {};
		R (*_invoke)(void *, Args &&...) = nullptr;
		const std::type_info *_tid = &typeid(void);
		bool _isSmall = false;
	};

	template <class F>
	struct invoke_result;

	template <class R, typename... Args>
	struct invoke_result<unique_function<R(Args...)>> {
		using type = R;
	};
	template <class R, typename... Args>
	struct invoke_result<function_ref<R(Args...)>> {
		using type = R;
	};

	template <class F>
	using invoke_result_t = typename invoke_result<F>::type;

} // namespace libstra

#endif // UNIQUE_FN_H
