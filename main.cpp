#include <iostream>
#include <unique_function.hpp>

struct F {
	bool _moved = false;
	char _dummy[8];
	F() = default;
	F(const F &) = delete;
	F(F &&other) { other._moved = true; }
	int operator()() const { return 10; }
	~F() {
		if (!_moved) std::cout << "Destroyed\n";
	}
};

int f() {
	return 10;
}

int main(int argc, char const *argv[]) {
	unique_function<int()> func = F{};
	std::cout << func() << '\n';
}
