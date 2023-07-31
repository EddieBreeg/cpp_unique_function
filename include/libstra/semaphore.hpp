#pragma once

#include <condition_variable>
#include <mutex>
#include <cstddef>
#include <chrono>

namespace libstra {
	class semaphore {
	private:
		size_t _n;
		std::mutex _m;
		std::condition_variable _cv;

	public:
		explicit semaphore(size_t n = 1) noexcept;
		semaphore(semaphore &) = delete;

		void acquire();
		void release(size_t n = 1);
		void wait();

		[[nodiscard]]
		bool try_acquire();

		template <class Rep, class Period>
		[[nodiscard]]
		bool try_acquire_for(const std::chrono::duration<Rep, Period> &d) {
			std::unique_lock<std::mutex> lk(_m);
			_cv.wait_for(lk, d, [this]() { return this->_n; });
			if (_n) {
				--_n;
				return true;
			} else return false;
		}
		template <class Clock, class Duration>
		[[nodiscard]]
		bool try_acquire_until(
			const std::chrono::time_point<Clock, Duration> &abs_time) {
			std::unique_lock<std::mutex> lk(_m);
			_cv.wait_until(lk, abs_time, [this]() { return this->_n; });
			if (_n) {
				--_n;
				return true;
			} else return false;
		}
	};

} // namespace libstra
