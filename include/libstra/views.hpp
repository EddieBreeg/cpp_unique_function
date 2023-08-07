#pragma once

#include <cstddef>
#include <iterator>

namespace libstra {
	template <class T>
	class array_view {
	private:
		const T *_start = nullptr, *_end = nullptr;

	public:
		constexpr array_view() noexcept = default;
		constexpr array_view(const T *ptr, size_t n) noexcept :
			_start(ptr), _end(ptr + n) {}
		template <class Iter>
		constexpr array_view(Iter begin, Iter end) :
			_start(&(*begin)), _end(&(*end)) {}
		template <class Iterable>
		constexpr array_view(const Iterable &x) :
			_start(std::begin(x)), _end(std::end(x)) {}

		[[nodiscard]]
		constexpr const T &
		operator[](size_t i) const {
			return _start[i];
		}
		[[nodiscard]]
		constexpr const T *data() const noexcept {
			return _start;
		}
		[[nodiscard]]
		constexpr const T &front() const {
			return *_start;
		}
		[[nodiscard]]
		constexpr const T &back() const {
			return *(_end - 1);
		}
		[[nodiscard]]
		constexpr const T *begin() const noexcept {
			return _start;
		}
		[[nodiscard]]
		constexpr const T *end() const noexcept {
			return _end;
		}
		[[nodiscard]]
		constexpr size_t size() const noexcept {
			return _end - _start;
		}
		[[nodiscard]]
		constexpr bool is_empty() const noexcept {
			return !size();
		}
		[[nodiscard]]
		constexpr array_view sub(size_t start, size_t n = -1) const noexcept {
			return array_view(_start + start, n != -1 ? _start + n : _end);
		}
	};
} // namespace libstra
