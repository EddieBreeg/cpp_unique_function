#include <libstra/barrier.hpp>
#include <chrono>
#include <thread>
#include <iostream>
#include <cassert>

void test1() {
	int x = 0;
	auto f = [&]() { ++x; };
	libstra::barrier<decltype(f)> b(2, f);
	using namespace std::chrono_literals;

	std::thread t([&]() {
		std::this_thread::sleep_for(200ms);
		std::cout << "T1 waiting\n";
		b.arrive_and_wait();
		std::cout << "T1 done\n";
	});
	std::thread t2([&]() {
		std::this_thread::sleep_for(400ms);
		std::cout << "T2 waiting\n";
		b.arrive_and_wait();
		std::cout << "T2 done\n";
	});

	t.join();
	t2.join();
	assert(x == 1);
}
void test2() {
	int x = 0;
	auto f = [&]() { ++x; };
	libstra::barrier<decltype(f)> b(2, f);
	using namespace std::chrono_literals;

	std::thread t([&]() {
		std::this_thread::sleep_for(200ms);
		std::cout << "T1 waiting\n";
		b.wait();
		std::cout << "T1 done\n";
	});
	std::thread t2([&]() {
		std::this_thread::sleep_for(400ms);
		std::cout << "T2 waiting\n";
		b.arrive_and_drop();
		std::cout << "T2 done\n";
	});
	b.arrive();
	t.join();
	t2.join();
	t = std::thread([&]() { b.arrive(); });
	t.join();
	assert(x == 2);
}

int main(int argc, char const *argv[]) {
	test1();
	test2();
}
