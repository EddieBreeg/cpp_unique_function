#include <libstra/semaphore.hpp>
#include <cassert>
#include <thread>
#include <vector>
#include <iostream>

void test1() {
	libstra::semaphore sem(0);

	std::thread t1([&]() {
		sem.acquire();
		std::cout << "T1\n";
	});
	std::thread t2([&]() {
		sem.acquire();
		std::cout << "T2\n";
	});

	std::cout << "T0\n";
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	sem.release(2);

	t1.join();
	t2.join();
}

int main(int argc, char const *argv[]) {
	test1();

	libstra::semaphore sem(0);

	auto f = [&](int n) {
		if (sem.try_acquire_for(std::chrono::milliseconds(n))) {
			std::cout << "Acquired\n";
		} else std::cout << "Failed\n";
	};
	std::thread t1(f, 100);
	std::this_thread::sleep_for(std::chrono::milliseconds(300));
	sem.release();
	t1.join();
	if (!sem.try_acquire()) return 1;

	t1 = std::thread(f, 300);
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	sem.release();
	t1.join();
}
