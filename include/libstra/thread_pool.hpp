#pragma once

#include <vector>
#include <queue>
#include <future>
#include <thread>
#include <condition_variable>
#include <libstra/unique_function.hpp>

namespace libstra {

	class thread_pool {
	public:
		thread_pool();
		~thread_pool();

	private:
		std::vector<std::thread> _threads;
		std::condition_variable _cv;
		std::queue<unique_function<void()>> _tasks;
		std::mutex _mutex;
	};
} // namespace libstra
