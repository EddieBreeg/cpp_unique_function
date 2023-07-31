#pragma once

#include <mutex>
#include <condition_variable>

namespace libstra {
	class latch {
	private:
		size_t _n;
		std::mutex _m;
		std::condition_variable _cv;

	public:
		latch(size_t n);
		void wait();
		void arrive();
		void arrive_and_wait();
		~latch();
	};

} // namespace libstra
