#pragma once

#include <condition_variable>
#include <mutex>
#include <cstddef>
#include <chrono>

namespace libstra {
	/** Lightweight synchonization primitive, which allows for more than one
	 * concurrent access
	 */
	class semaphore {
	private:
		size_t _n;
		std::mutex _m;
		std::condition_variable _cv;

	public:
		/**
		 * Constructor
		 * @param n: The initial value for the internal counter
		 */
		explicit semaphore(size_t n = 1) noexcept;
		semaphore(semaphore &) = delete;

		/**
		 * Decrements the internal counter, or blocks until it can; that is,
		 * when said counter is > 0
		 */
		void acquire();

		/**
		 * Increments the counter, and notifies the other threads of the change
		 * @param n: The amount to increase the counter by
		 * @note This method can be called even if the current thread had not
		 * decremented the counter before
		 */
		void release(size_t n = 1);

		/**
		 * If the counter is non-0, returns immediately. Otherwise, blocks until
		 * it is.
		 */
		void wait();

		/**
		 * Tries to decrement the counter without blocking
		 * @returns true if the counter was decremented, false otherwise
		 */
		[[nodiscard]]
		bool try_acquire();

		/**
		 * Attempts to derectement the counter if it can be; otherwise, waits
		 * until it can, or a duration of d has been exceeded
		 * @param d: the maximum duration to wait for
		 * @return true if the counter was decremented, false otherwise
		 */
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
		/**
		 * Attempts to derectement the counter if it can be; otherwise, waits
		 * until it can, or the deadline abs_time has been hit
		 * @param abs_time: The earliest deadline the function must wait before
		 * failing
		 * @return true if the counter was decremented, false otherwise
		 */
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
