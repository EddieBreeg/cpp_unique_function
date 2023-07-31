#include <libstra/semaphore.hpp>

namespace libstra {
	using unique_lock = std::unique_lock<std::mutex>;

	semaphore::semaphore(size_t n) noexcept : _n(n) {}

	void semaphore::wait() {
		unique_lock lk(_m);
		while (!_n)
			_cv.wait(lk);
	}

	void semaphore::acquire() {
		unique_lock lk(_m);
		while (!_n)
			_cv.wait(lk);
		--_n;
	}
	void semaphore::release(size_t n) {
		std::lock_guard<std::mutex> lk(_m);
		_n += n;
		_cv.notify_all();
	}
	bool semaphore::try_acquire() {
		std::unique_lock<std::mutex> lk(_m);
		if (_n) {
			--_n;
			return true;
		}
		return false;
	}
} // namespace libstra
