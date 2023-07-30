#pragma once

#if __cplusplus >= 201703
#define _HAS_CPP_17
#endif

#include <utility>
#include <initializer_list>
#include <libstra/utility.hpp>

namespace libstra {

	template <class T, class E, bool TrivialDestructor>
	struct _expected;

	template <class T, class E>
	struct _expected<T, E, false> {
		union {
			T value = T{};
			E error;
		};
		bool has_value = true;
		~_expected() {
			if (has_value) value.~T();
			else error.~E();
		}
	};
	template <class T, class E>
	struct _expected<T, E, true> {
		union {
			T value = T{};
			E error;
		};
		bool has_value = true;
	};
	template <class E>
	struct _expected<void, E, false> {
		union {
			E error;
			char _unused[sizeof(E)] = {};
		};
		bool has_value = true;
		~_expected() {
			if (!has_value) error.~E();
		}
	};
	template <class E>
	struct _expected<void, E, true> {
		union {
			E error;
			char _unused[sizeof(E)] = {};
		};
		bool has_value = true;
	};

	template <class T, class E>
	using expected = _expected<T, E,
							   (std::is_trivially_destructible<T>::value ||
								std::is_void<T>::value) &&
								   std::is_trivially_destructible<E>::value>;

} // namespace libstra
