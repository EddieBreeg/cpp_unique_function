#include <libstra/storage.hpp>
#include <assert.h>
#include <iostream>

struct A {
	int x = 0;
	char *str = nullptr;
	A() = delete;
	A(int v) : x(v), str(new char) {}
	A(const A &other) : x(other.x) {
		std::cout << "wtf is this shit???\n";
		return;
	}
	A &operator=(const A &other) {
		x = other.x;
		return *this;
	}
	~A() {
		if (str) {
			std::cout << "Deleted called\n";
			delete str;
		};
	}
	bool operator==(const A &other) const noexcept { return x == other.x; }
};

struct B {
	constexpr B() {}
	B(const B &) = delete;
	constexpr B(B &&) {}
};

int main() {
	constexpr libstra::storage<int> x{ 10 };
	constexpr auto y = x;
	static_assert(x._val == y._val, "constexpr storage test failed");

	libstra::storage<A> a;
	libstra::storage<A> b{ 1 };
	a = b._val;
	assert(*a == *b);

	constexpr libstra::storage<void> v;
	constexpr libstra::storage<B> u{ B{} };
}