#include <libstra/expected.hpp>
#include <cassert>
#include <stdexcept>
#include <iostream>

struct dummy_error {
	constexpr dummy_error() = default;
	const char *what() const noexcept { return "Dummy Error"; }
};
struct error2 {
	const char *_msg = nullptr;
	constexpr error2(const char *str) noexcept : _msg(str) {}
	constexpr error2(std::initializer_list<int>, const char *str) noexcept :
		_msg(str) {}
	constexpr const char *what() const noexcept { return _msg; }
	constexpr bool operator==(const error2 &other) const {
		return _msg == other._msg;
	}
};
constexpr bool operator==(const dummy_error &, const dummy_error &) {
	return true;
}

void constexpr_tests() {
	constexpr libstra::unexpected<dummy_error> x{ dummy_error{} };
	static_assert(error2{ "Msg" }.what() == "Msg");
	constexpr libstra::unexpected<error2> y(libstra::in_place_t{},
											(const char *)"foobar");
	constexpr decltype(y) z(libstra::in_place_t{}, { 1, 2 }, "foobar");
	static_assert(z.error().what() == "foobar");
	static_assert(y == z);
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
