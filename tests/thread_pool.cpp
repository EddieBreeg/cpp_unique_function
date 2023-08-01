#include <libstra/thread_pool.hpp>
#include <iostream>
#include <cassert>
#include <stdexcept>

struct A {
	int _val = 0;
	bool _moved = false;
	A() = default;
	A(int x) : _val(x) {}
	A(const A &) = delete;
	A(A &&other) : _val(other._val) { other._moved = true; }
};

struct B {
	static int moves, copies;
	bool _moved = false;
	B() = default;
	B(const B &) { ++copies; }
	B(B &&other) {
		++moves;
		other._moved = true;
	}
};
int B::copies = 0, B::moves = 0;

int f(A x) {
	return x._val;
}

void test1() {
	libstra::thread_pool tp(3);
	auto res1 = tp.enqueue_task<int>(f, A{ 42 });
	assert(res1.get() == 42);
	auto g = [](int &x) { x = 666; };
	int x = 1;
	auto res2 = tp.enqueue_task<void>(g, x);
	res2.wait();
	assert(x == 666);
	B b;
	auto res3 = tp.enqueue_task<void>([](B x) {}, b);
	res3.wait();
	assert(!b._moved);
	std::cout << B::copies << '\n';
	std::cout << B::moves << '\n';

	tp.stop();
}
void test2() {
	using namespace std::chrono;
	libstra::thread_pool tp(2);
	auto res1 = tp.enqueue_task<void>([]() {
		std::this_thread::sleep_for(milliseconds{ 200 });
		std::cout << "Task 1 done\n";
	});
	auto res2 = tp.enqueue_task<void>([]() {
		std::this_thread::sleep_for(milliseconds{ 400 });
		std::cout << "Task 2 done\n";
	});
	auto res3 = tp.enqueue_task<void>([]() {
		std::this_thread::sleep_for(milliseconds(100));
		std::cout << "Task 3 done\n";
	});
	tp.wait();
}
void test3() {
	libstra::thread_pool tp(1);
	auto r = tp.enqueue_task<int>(
		[]() -> int { throw std::runtime_error("any error"); });
	try {
		int x = r.get();
		assert(0);
	} catch (const std::runtime_error &e) {
		std::cout << e.what() << '\n';
	}
}
int main() {
	test1();
	test2();
	test3();
}
