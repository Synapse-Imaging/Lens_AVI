#pragma once
#include <vector>
#include <thread>
#include <future>
#include <functional>
#include <stdexcept>
#include <memory>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <sstream>
#include <windows.h>

namespace AIService {

	inline std::string ptr_to_string(const void* ptr) {
		std::ostringstream oss;
		oss << ptr;
		return oss.str();
	}

	inline void debug_output(int id, const std::string& msg) {
		/*std::ostringstream oss;
		oss << "[ThreadPool #" << id << "] " << msg << "\n";
		OutputDebugStringA(oss.str().c_str());*/
	}

	inline int get_next_threadpool_id() {
		static std::atomic<int> counter{ 0 };
		return ++counter;
	}

	class ThreadPool {
	public:
		ThreadPool() noexcept
			: stop_all_(false), paused_(false), num_threads_(0), instance_id_(get_next_threadpool_id()) {
			debug_output(instance_id_, "constructed");
		}

		~ThreadPool() {
			shutdown();
			debug_output(instance_id_, "destructed");
		}

		void init(size_t num_threads) {
			debug_output(instance_id_, "init called");

			if (num_threads == 0) {
				throw std::invalid_argument("Thread count must be at least 1");
			}
			if (!worker_threads_.empty()) {
				throw std::runtime_error("ThreadPool is already initialized");
			}

			num_threads_ = num_threads;
			stop_all_ = false;
			paused_ = false;
			worker_threads_.reserve(num_threads_);

			for (size_t i = 0; i < num_threads_; ++i) {
				worker_threads_.emplace_back(&ThreadPool::worker_thread, this);
			}
		}

		template <class F, class... Args>
		std::future<typename std::invoke_result_t<F, Args...>> enqueue_job(F&& f, Args&&... args) {
			using return_type = typename std::invoke_result_t<F, Args...>;

			auto task = std::make_shared<std::packaged_task<return_type()>>(
				std::bind(std::forward<F>(f), std::forward<Args>(args)...)
				);

			std::future<return_type> result = task->get_future();

			{
				std::unique_lock lock(jobs_mutex_);
				if (stop_all_) {
					throw std::runtime_error("Cannot enqueue job: ThreadPool is stopped");
				}
				jobs_.emplace([task]() { (*task)(); });
			}

			jobs_cv_.notify_one();
			return result;
		}

		template <class F, class... Args>
		void enqueue_job_no_return(F&& f, Args&&... args) {
			std::function<void()> job = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
			{
				std::unique_lock<std::mutex> lock(jobs_mutex_);
				if (stop_all_) {
					throw std::runtime_error("ThreadPool has been stopped");
				}
				jobs_.emplace(std::move(job));
			}

			jobs_cv_.notify_one();
		}

		bool is_job_empty() {
			std::unique_lock<std::mutex> lock(jobs_mutex_);
			return jobs_.empty();
		}

		size_t get_job_count() {
			std::unique_lock<std::mutex> lock(jobs_mutex_);
			return jobs_.size();
		}

		// 일시 정지 기능
		void pause() {
			std::unique_lock<std::mutex> lock(jobs_mutex_);
			if (!paused_) {
				paused_ = true;
				debug_output(instance_id_, "ThreadPool paused");
			}
		}

		// 재시작 기능
		void resume() {
			{
				std::unique_lock<std::mutex> lock(jobs_mutex_);
				if (paused_) {
					paused_ = false;
					debug_output(instance_id_, "ThreadPool resumed");
				}
			}
			// 대기 중인 모든 스레드를 깨워서 작업을 계속하도록 함
			jobs_cv_.notify_all();
		}

		// 일시 정지 상태 확인
		bool is_paused() const {
			std::unique_lock<std::mutex> lock(jobs_mutex_);
			return paused_;
		}

		void shutdown() {
			std::call_once(shutdown_flag_, [this]() {
				debug_output(instance_id_, "shutdown initiated");

				{
					std::unique_lock<std::mutex> lock(jobs_mutex_);
					stop_all_ = true;
					paused_ = false; // shutdown 시 pause 상태 해제
				}
				jobs_cv_.notify_all();

				for (auto& thread : worker_threads_) {
					if (thread.joinable()) {
						thread.join();
					}
				}

				clear_jobs();
				debug_output(instance_id_, "shutdown completed");
			});
		}

	private:
		size_t num_threads_;
		std::vector<std::thread> worker_threads_;
		std::queue<std::function<void()>> jobs_;
		mutable std::mutex jobs_mutex_;
		std::condition_variable jobs_cv_;
		std::once_flag shutdown_flag_;
		std::atomic<bool> stop_all_;
		std::atomic<bool> paused_; // 일시 정지 상태
		int instance_id_; // 디버그용 ID

		void worker_thread() {
			debug_output(instance_id_, "worker_thread started. this = " + ptr_to_string(this));

			while (true) {
				std::function<void()> job;
				{
					std::unique_lock<std::mutex> lock(jobs_mutex_);
					debug_output(instance_id_, "mutex locked");

					jobs_cv_.wait(lock, [this]() {
						return stop_all_ || (!paused_ && !jobs_.empty());
						});

					if (stop_all_ && jobs_.empty()) {
						debug_output(instance_id_, "no more jobs, thread exiting");
						return;
					}

					// 일시 정지 상태면 계속 대기
					if (paused_) {
						continue;
					}

					if (!jobs_.empty()) {
						job = std::move(jobs_.front());
						jobs_.pop();
					} else {
						continue;
					}
				}

				try {
					debug_output(instance_id_, "executing job");
					job();
				}
				catch (const std::exception& e) {
					debug_output(instance_id_, std::string("exception: ") + e.what());
				}
				catch (...) {
					debug_output(instance_id_, "unknown exception");
				}
			}
		}

		void clear_jobs() {
			std::unique_lock lock(jobs_mutex_);
			while (!jobs_.empty()) {
				jobs_.pop();
			}
		}
	};

}
