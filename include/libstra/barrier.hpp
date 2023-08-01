#pragma once

#include <mutex>
#include <condition_variable>

namespace libstra {
	template <class CompletionFunction>
	class barrier {
		using unique_lock = std::unique_lock<std::mutex>;
		using lock_guard = std::lock_guard<std::mutex>;

	public:
		barrier(size_t n, CompletionFunction f = CompletionFunction()) :
			_init(n), _c(n), _f(f) {}
		barrier(const barrier &) = delete;
		void wait() {
			unique_lock lk(_m);
			_cv.wait(lk);
		}
		void arrive() {
			unique_lock lk(_m);
			if (!--_c) {
				_f();
				_cv.notify_all();
				_c = _init;
			}
		}
		void arrive_and_wait() {
			unique_lock lk(_m);
			if (!--_c) {
				_f();
				_cv.notify_all();
				_c = _init;
			} else {
				_cv.wait(lk);
			}
		}
		void arrive_and_drop() {
			unique_lock lk(_m);
			--_init;
			if (!--_c) {
				_f();
				_cv.notify_all();
				_c = _init;
			}
		}
		~barrier() {}

	private:
		size_t _init;
		size_t _c;
		CompletionFunction _f;
		std::condition_variable _cv;
		std::mutex _m;
	};

} // namespace libstra
