#include <libstra/thread_pool.hpp>

namespace libstra {
	using unique_lock = std::unique_lock<std::mutex>;
	using lock_guard = std::lock_guard<std::mutex>;

	thread_pool::thread_pool(size_t n) {
		for (size_t i = 0; i < n; i++) {
			_threads.emplace_back(&thread_pool::thread_loop, this);
		}
	}
	void thread_pool::wait() {
		unique_lock lk(_mutex);
		if (_stopped) return;

		_cv.wait(lk, [this]() { return !this->_current_tasks; });
	}
	void thread_pool::stop() {
		{
			unique_lock lk(_mutex);
			if (_stopped) return;

			_cv.wait(lk, [this]() { return !this->_current_tasks; });

			_stopped = true;
		}
		_cv.notify_all();
		join_threads();
	}
	void thread_pool::restart() {
		lock_guard lk(_mutex);
		if (!_stopped) return;

		for (auto &t : _threads) {
			t = std::thread(&thread_pool::thread_loop, this);
		}
	}
	void thread_pool::thread_loop() {
		for (;;) {
			unique_lock lk(_mutex);
			_cv.wait(lk, [this]() {
				return this->_stopped || this->_current_tasks;
			});
			if (_stopped) return;
			auto task = std::move(_tasks.front());
			_tasks.pop();
			bool notify = !--_current_tasks;

			lk.unlock();
			task();
			if (notify) _cv.notify_all();
		}
	}

	void thread_pool::join_threads() {
		lock_guard lk(_stopMutex);
		for (auto &t : _threads) {
			if (t.joinable()) t.join();
		}
	}

	thread_pool::~thread_pool() {
		{
			lock_guard lk(_mutex);
			if (_stopped) return;
			_stopped = true;
		}
		_cv.notify_all();
		join_threads();
	}

} // namespace libstra
