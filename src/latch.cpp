#include <libstra/latch.hpp>

namespace libstra {
	void latch::wait() {
		std::unique_lock<std::mutex> lk(_m);
		if (!_n) return;
		_cv.wait(lk, [this]() { return !this->_n; });
	}

	void latch::arrive() {
		{
			std::unique_lock<std::mutex> lk(_m);
			if (!_n) return;
			if (--_n) return;
		}
		_cv.notify_all();
	}
	void latch::arrive_and_wait() {
		std::unique_lock<std::mutex> lk(_m);
		if (!_n) return;
		if (!--_n) {
			lk.unlock();
			_cv.notify_all();
		} else {
			_cv.wait(lk, [this]() { return !this->_n; });
		}
	}
	latch::latch(size_t n) {
		std::lock_guard<std::mutex> lk(_m);
		_n = n;
	}
	latch::~latch() {
		if (!_n) return;
		{
			std::lock_guard<std::mutex> lk(_m);
			_n = 0;
		}
		_cv.notify_all();
	}
} // namespace libstra
