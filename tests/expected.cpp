#include <libstra/expected.hpp>
#include <cassert>
#include <stdexcept>
#include <iostream>
#include <expected>

struct E {
	int _code;
	constexpr E(int c) noexcept : _code(c) {}
	constexpr E(std::initializer_list<int>, int c) noexcept : _code(c) {}
	constexpr int what() const noexcept { return _code; }
	constexpr bool operator==(const E &other) const {
		return _code == other._code;
	}
};

void constexpr_tests() {
	{
		constexpr libstra::expected<int, E> exp{ .value{} };
		static_assert(exp.value == 0, "constexpr expected test failed");
	}
	{
		constexpr libstra::expected<void, E> exp{ ._unused{} };
		static_assert(exp.has_value, "constexpr expected test failed");
	}
}

void swap_test() {}

class Foo {
public:
	constexpr Foo() {}
	constexpr Foo(const Foo &) {}
	~Foo() = default;
};

int main(int argc, char const *argv[]) {
	constexpr_tests();
	swap_test();
}
