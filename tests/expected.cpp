#include <libstra/expected.hpp>
#include <cassert>
#include <stdexcept>
#include <iostream>

struct E {
	const int _code;
	constexpr E(int c) noexcept : _code(c) {}
	constexpr E(std::initializer_list<int>, int c) noexcept : _code(c) {}
	constexpr int what() const noexcept { return _code; }
	constexpr bool operator==(const E &other) const {
		return _code == other._code;
	}
};

void constexpr_tests() {
	constexpr libstra::unexpected<E> a(E(42));
	static_assert(a.error().what() == 42, "constexpr test failed");
	constexpr libstra::unexpected<E> b(libstra::in_place_t{}, 1);
	constexpr libstra::unexpected<E> c(libstra::in_place_t{}, { 1, 2, 3 }, 1);
	static_assert(b == c, "constexpr test failed");
}

class Foo {
public:
	constexpr Foo() {}
	constexpr Foo(const Foo &) {}
	~Foo() = default;
};

int main(int argc, char const *argv[]) {
	constexpr_tests();
}
