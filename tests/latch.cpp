#include <libstra/latch.hpp>
#include <thread>
#include <iostream>

int main(int argc, char const *argv[]) {
	libstra::latch l(3);

	std::thread t1([&]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		std::cout << "T1 arrived\n";
		l.arrive_and_wait();
		std::cout << "T1 done\n";
	});
	std::thread t2([&]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout << "T2 arrived\n";
		l.arrive_and_wait();
		std::cout << "T2 done\n";
	});
	std::thread t3([&]() {
		std::cout << "T3 arrived\n";
		l.arrive();
		std::cout << "T3 done\n";
	});
	std::thread t4([&]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(1500));
		std::cout << "T4 arrived\n";
		l.arrive_and_wait();
		std::cout << "T4 done\n";
	});
	std::cout << "main thread waiting\n";
	l.wait();
	std::cout << "main thread done\n";

	t1.join();
	t2.join();
	t3.join();
	t4.join();
}
