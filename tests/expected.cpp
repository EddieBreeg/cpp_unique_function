#include <libstra/expected.hpp>
#include <cassert>
#include <stdexcept>
#include <iostream>

struct dummy_error {
	constexpr dummy_error() noexcept {}
	const char *what() const noexcept { return "Dummy Error"; }
};
constexpr bool operator==(const dummy_error &, const dummy_error &) {
	return true;
}

// void unexpected_eq_test() {
// 	constexpr libstra::unexpected<dummy_error> err(std::in_place_t{});
// 	static_assert(err.error() == dummy_error{},
// 				  "unexpected equality tests failed");
// 	constexpr libstra::unexpected<dummy_error> err2(std::in_place_t{});
// 	static_assert(err == err2, "unexpected equality tests failed");
// }
// void unexpected_swap_tests() {
// 	libstra::unexpected<int> err1 = 0, err2 = 1;
// 	static_assert(noexcept(err1.swap(err2)), "noexcept swap test failed");
// 	err1.swap(err2);
// 	assert(err1.error() == 1 && err2.error() == 0);
// }

class Foo {
public:
	constexpr Foo() {}
	constexpr Foo(const Foo &) {}
	~Foo() = default;
};

void expected_eq_tests() {
	// constexpr libstra::expected<Foo, dummy_error> x;
}

int main(int argc, char const *argv[]) {
	// unexpected_eq_test();
	// unexpected_swap_tests();
	// expected_eq_tests();
}
