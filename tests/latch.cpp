#include <libstra/latch.hpp>
#include <thread>
#include <iostream>
#include <assert.h>

int main(int argc, char const *argv[]) {
	libstra::latch l(3);
	char results[4];
	char *ptr = results;

	std::thread t1([&]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		std::cout << "T1 arrived\n";
		l.arrive();
		*ptr++ = 1;
		std::cout << "T1 done\n";
	});
	std::thread t2([&]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout << "T2 arrived\n";
		l.arrive();
		*ptr++ = 2;
		std::cout << "T2 done\n";
	});
	std::thread t3([&]() {
		std::cout << "T3 arrived\n";
		l.arrive();
		*ptr++ = 3;
		std::cout << "T3 done\n";
	});
	std::thread t4([&]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(1500));
		std::cout << "T4 arrived\n";
		l.arrive_and_wait();
		*ptr++ = 4;
		std::cout << "T4 done\n";
	});
	std::cout << "main thread waiting\n";
	l.wait();
	std::cout << "main thread done\n";
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	assert(results[0] == 3);
	assert(results[1] == 1);
	assert(results[2] == 2);
	assert(results[3] == 4);
}
