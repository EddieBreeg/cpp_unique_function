#pragma once

#include <vector>
#include <queue>
#include <future>
#include <thread>
#include <condition_variable>
#include <libstra/unique_function.hpp>
#include <libstra/utility.hpp>

namespace libstra {
	/**
	 * Represents a pool of threads you can assign any kind of task to
	 */
	class thread_pool {
	public:
		/**
		 * Constructs the pool with a fixed amount of threads
		 * @param n: The number of threads to create. If 0,
		 * the behaviour is undefined
		 */
		thread_pool(size_t n);
		/**
		 * Destroys the pool, by stopping all threads. May block
		 * if some threads are actively executing a task, but any
		 * remaining task in the internal queue will get cancelled
		 */
		~thread_pool();

		thread_pool(thread_pool &) = delete;
		thread_pool(thread_pool &&) = delete;

		/**
		 * Adds a new task to the queue
		 * @tparam R: The type of object returned by the task
		 * @tparam F: A callable object type
		 * @tparam Args: A list of argument types
		 * @param f: The callable object to invoke
		 * @param Args: The arguments to pass to f
		 * @returns A std::future object you can use to wait on the task,
		 * and get its result or any exception that might have occurred
		 * @note If the callable object takes arguments by reference, you
		 * MUST wrap the references, using std::ref
		 */
		template <class R, class F, typename... Args>
		[[nodiscard]]
		std::future<R> enqueue_task(F &&f, Args &&...args) {
			std::promise<R> p;
			auto res = p.get_future();
			{
				std::lock_guard<std::mutex> lk(_mutex);
				auto &&task = make_task<R>(std::move(p), forward<F>(f),
										   libstra::forward<Args>(args)...);
				_tasks.emplace(std::move(task));
				++_current_tasks;
			}
			_cv.notify_one();
			return res;
		}
		/**
		 * Waits for all current tasks to finish executing
		 * @note If you want to wait for a specific task, use the wait method
		 * on the std::future object associated with the task
		 */
		void wait();
		/**
		 * Waits for all current tasks to finish, then stops and joins all the
		 * threads
		 * @note If the pool had already been stopped, this function is a no op
		 */
		void stop();
		/**
		 * Restarts the pool, with the same number of threads as when it was
		 * first constructed
		 * @note If the pool wasn't stopped, this function is a no op
		 */
		void restart();

	private:
		void thread_loop();
		void join_threads();

		template <class R, class F,
				  std::enable_if_t<!std::is_void<R>::value, int> = 0,
				  typename... Args>
		auto make_task(std::promise<R> &&p, F &&f, Args &&...args) {
			std::tuple<std::decay_t<Args>...> t{ libstra::forward<Args>(
				args)... }; // store the values
			return [p = std::move(p), f = forward<F>(f),
					args = std::move(t)]() mutable {
				try {
					p.set_value(apply(std::move(f), std::move(args)));
				} catch (...) {
					p.set_exception(std::current_exception());
				}
			};
		}
		template <class R, class F,
				  std::enable_if_t<std::is_void<R>::value, int> = 0,
				  typename... Args>
		auto make_task(std::promise<R> &&p, F &&f, Args &&...args) {
			std::tuple<std::decay_t<Args>...> t{ libstra::forward<Args>(
				args)... }; // store the values
			return [p = std::move(p), f = forward<F>(f),
					args = std::move(t)]() mutable {
				try {
					apply(std::move(f), std::move(args));
					p.set_value();

				} catch (...) {
					p.set_exception(std::current_exception());
				}
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
