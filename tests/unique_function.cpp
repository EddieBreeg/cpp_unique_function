#include <libstra/unique_function.hpp>
#include <iostream>
#include <cassert>
#include <functional>

struct F {
	bool _moved = false;
	int _val = 0;
	F() = default;
	F(const F &) = delete;
	F(F &&other) : _val(other._val) { other._moved = true; }
	int operator()() const { return 10; }
	~F() {}
};

int foo() {
	return 2;
}
int bar(int x) {
	return x;
}

void function_ref_tests() {
	using libstra::function_ref;
	function_ref<int()> f1;
	assert(f1.target_type() == typeid(void));
	f1 = foo;
	assert(f1.target_type() == typeid(foo));
	auto f2 = f1;
	assert(f2.target_type() == typeid(foo));
	assert(f2() == 2 && f1() == 2);

	f1 = F{};
	assert(f1() == F{}());
	f2 = [x = F{}]() { return 42; };
	assert(f2() == 42);
}
void unique_function_tests() {
	using libstra::function_ref;
	using libstra::unique_function;
	unique_function<int()> f1;
	assert(f1.target_type() == typeid(void));
	f1 = foo;
	assert(f1.target_type() == typeid(foo));
	auto f2 = std::move(f1);
	assert(f1.target_type() == typeid(void));
	assert(!f1);
	assert(f2.target_type() == typeid(foo));
	assert(f2() == foo());
	f1 = std::move(f2);
	assert(f2.target_type() == typeid(void));
	assert(!f2);

	f1 = F{};
	assert(f1() == F{}());
	f2 = [x = F{}]() { return 42; };
	assert(f2() == 42);

	auto ref = f1.get_ref<F>();
	assert(ref);
	assert(ref.target_type() == typeid(F));
	assert(ref() == F{}());
}
void bind_tests() {
	using libstra::unique_function;
	unique_function<int()> b = std::bind(bar, 1);
	assert(b() == 1);
}

int main(int argc, char const *argv[]) {
	function_ref_tests();
	unique_function_tests();
	bind_tests();
}