#pragma once

#include <mutex>
#include <condition_variable>

namespace libstra {
	struct empty_function_t {
		constexpr void operator()() const noexcept {}
	};

	/**
	 * A synchronization primitive which, similarly to latches, allows to block
	 * a group of threads until they all reach the barrier. Unlike latches,
	 * barriers are re-usable, and they allow for the execution of a completion
	 * function before unblocking the waiting threads
	 * @tparam CompletionFunction: The callable type to use as a completion
	 * function. This function must take no argument, and any return value will
	 * be ignored
	 */
	template <class CompletionFunction = empty_function_t>
	class barrier {
		using unique_lock = std::unique_lock<std::mutex>;
		using lock_guard = std::lock_guard<std::mutex>;

	public:
		/**
		 * Constructor
		 * @param n: The initial value for the expected count. When all threads
		 * have arrived, the counter is reset to that value, if it hasn't been
		 * dropped by a call to arrive_and_drop
		 * @param f: The completion function to call upon completion
		 */
		barrier(size_t n, CompletionFunction f = CompletionFunction()) :
			_init(n), _c(n), _f(f) {}
		barrier(const barrier &) = delete;
		/**
		 * Blocks and waits other threads to arrive until the expected count is
		 * 0
		 */
		void wait() {
			unique_lock lk(_m);
			_cv.wait(lk);
		}
		/**
		 * Decrements the expected count for the current phase, without blocking
		 */
		void arrive() {
			unique_lock lk(_m);
			if (!--_c) {
				_f();
				_c = _init;
				lk.unlock();
				_cv.notify_all();
			}
		}
		/**
		 * Decrements the expected count for the current phase, then blocks
		 * until it is 0
		 */
		void arrive_and_wait() {
			unique_lock lk(_m);
			if (!--_c) {
				_f();
				_c = _init;
				lk.unlock();
				_cv.notify_all();
			} else {
				_cv.wait(lk);
			}
		}
		/**
		 * Drops the initial expected count by one, then decrements the current
		 * expected count without blocking
		 */
		void arrive_and_drop() {
			unique_lock lk(_m);
			--_init;
			if (!--_c) {
				_f();
				_c = _init;
				lk.unlock();
				_cv.notify_all();
			}
		}
		/**
		 * Destroys the barrier. This does not set the expected count to 0, and
		 * does not call the completion function
		 */
		~barrier() {}

	private:
		size_t _init;
		size_t _c;
		CompletionFunction _f;
		std::condition_variable _cv;
		std::mutex _m;
	};

} // namespace libstra
