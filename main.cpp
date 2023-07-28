#include <iostream>
#include <unique_function.hpp>

struct F {
	bool _moved = false;
	int _val = 0;
	F() = default;
	F(const F &) = delete;
	F(F &&other) : _val(other._val) { other._moved = true; }
	int operator()() const { return 10; }
	~F() {
		if (!_moved) std::cout << "Destroyed\n";
	}
};

F foo() {
	F res{};
	res._val = 10;
	return res;
}

unique_function<F()> make_func() {
	return [f = F{}]() mutable {
		f._val = 11;
		return std::move(f);
	};
}

int main(int argc, char const *argv[]) {
	unique_function<F()> f = foo;
	f = make_func();
	F x = f();
	std::cout << x._val << '\n';
}
