#pragma once

#include <vector>
#include <queue>
#include <future>
#include <thread>
#include <condition_variable>
#include <libstra/unique_function.hpp>
#include <libstra/utility.hpp>

namespace libstra {

	class thread_pool {
	public:
		thread_pool(size_t n);
		~thread_pool();

		void stop();
		void wait();
		template <class R, class F, typename... Args>
		[[nodiscard]]
		std::future<R> enqueue_task(F &&f, Args &&...args) {
			std::promise<R> p;
			auto res = p.get_future();
			{
				std::lock_guard<std::mutex> lk(_mutex);
				auto &&task = make_task<R>(std::move(p), forward<F>(f),
										   forward<Args>(args)...);
				_tasks.emplace(std::move(task));
				++_current_tasks;
			}
			_cv.notify_one();
			return res;
		}

	private:
		void thread_loop();
		void join_threads();

		template <class R, class F,
				  std::enable_if_t<!std::is_void<R>::value, int> = 0,
				  typename... Args>
		auto make_task(std::promise<R> &&p, F &&f, Args &&...args) {
			std::tuple<Args...> t{ forward<Args>(args)... }; // store the values
			return [p = std::move(p), f = forward<F>(f),
					args = std::move(t)]() mutable {
				p.set_value(apply(std::move(f), std::move(args)));
			};
		}
		template <class R, class F,
				  std::enable_if_t<std::is_void<R>::value, int> = 0,
				  typename... Args>
		auto make_task(std::promise<R> &&p, F &&f, Args &&...args) {
			std::tuple<Args...> t{ forward<Args>(args)... }; // store the values
			return [p = std::move(p), f = forward<F>(f),
					args = std::move(t)]() mutable {
				apply(std::move(f), std::move(args));
				p.set_value();
			};
		}

		std::vector<std::thread> _threads;
		std::condition_variable _cv;
		std::queue<unique_function<void()>> _tasks;
		std::mutex _mutex, _stopMutex;
		size_t _current_tasks = 0;
		bool _stopped = false;
	};
} // namespace libstra
