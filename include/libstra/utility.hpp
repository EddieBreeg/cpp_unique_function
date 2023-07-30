#pragma once

#include <type_traits>

namespace libstra {
	struct in_place_t {};
	struct nullopt_t {};

	template <class T>
	T &&forward(std::remove_reference_t<T> &&t) noexcept {
		return (T &&)t;
	}
	template <class T>
	T &&forward(std::remove_reference_t<T> &t) noexcept {
		return (T &&)t;
	}
	void forward() noexcept {}
} // namespace libstra
