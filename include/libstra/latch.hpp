#pragma once

#include <mutex>
#include <condition_variable>

namespace libstra {
	/**
	 * Simple synchronization primitive, which allows you to
	 * wait until a certain number of threads have reached the
	 * synchronization point in you code. Unlike barriers, the internal
	 * counter doesn't reset when reaching 0, and there is no way to reset it
	 */
	class latch {
	private:
		size_t _n;
		std::mutex _m;
		std::condition_variable _cv;

	public:
		/**
		 * Constructs the latch
		 * @param n: The initial value of the counter
		 */
		latch(size_t n);
		latch(latch &) = delete;
		latch(latch &&) = delete;
		/**
		 * If the internal counter is 0, returns, otherwise blocks until it is
		 */
		void wait();

		/**
		 * Returns true if the counter had reached 0
		 */
		bool try_wait();
		/**
		 * Decrements the internal counter (if non-0). If the counter reaches 0
		 * when decremented, a notification will be sent and all threads
		 * currently waiting on the latch will be unblocked
		 */
		void arrive();
		/**
		 * If the counter is not 0, decrements it, and then either blocks until
		 * it reaches 0 or notifies all threads that it is. When the counter
		 * reaches 0, all threads currently waiting on the latch will be
		 * unblocked
		 */
		void arrive_and_wait();
		/**
		 * Destructor. If the counter is 0, does nothing. Otherwise, sets it to
		 * 0 and notifies all waiting threads
		 */
		~latch();
	};

} // namespace libstra
