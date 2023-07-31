#pragma once

#include <condition_variable>
#include <mutex>
#include <cstddef>

namespace libstra {
	class semaphore {
	private:
		size_t _n;
		std::mutex _m;
		std::condition_variable _cv;

	public:
		semaphore(size_t n);
		void acquire();
		void release(size_t n = 1);

		void wait();
	};

} // namespace libstra
