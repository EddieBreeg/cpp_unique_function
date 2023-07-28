#include <iostream>
#include <unique_function.hpp>

struct F {
	F() = default;
	F(const F &) = delete;
	F(F &&) {}
	int operator()() const { return 0; }
	~F() { std::cout << "Destroyed\n"; }
};

int main(int argc, char const *argv[]) {
	_FuncState s([x = 1]() { return 0; });
}
