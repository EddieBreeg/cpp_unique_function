#include <libstra/semaphore.hpp>
#include <cassert>
#include <thread>
#include <vector>
#include <iostream>

int main(int argc, char const *argv[]) {
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
	sem.release(1);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	sem.release(1);

	t1.join();
	t2.join();
}
