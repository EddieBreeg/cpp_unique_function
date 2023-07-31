#include <libstra/thread_pool.hpp>
#include <iostream>
#include <cassert>

struct A {
	int _val = 0;
	bool _moved = false;
	A() = default;
	A(int x) : _val(x) {}
	A(const A &) = delete;
	A(A &&other) : _val(other._val) { other._moved = true; }
};

int f(A x) {
	return x._val;
}

int main() {
	libstra::thread_pool tp(3);
	auto res1 = tp.enqueue_task<int>(f, A{ 42 });
	assert(res1.get() == 42);

	tp.stop();
}
