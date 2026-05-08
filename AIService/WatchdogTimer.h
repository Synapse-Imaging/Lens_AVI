#ifndef AISERVICE_WATCHDOGTIMER_H
#define AISERVICE_WATCHDOGTIMER_H

#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <functional>
#include <mutex>

namespace AIService {
	class WatchdogTimer {
	public:
		WatchdogTimer(int timeout_ms, std::function<void()> on_timeout)
			: timeout_ms_(timeout_ms), on_timeout_(on_timeout), running_(false), paused_(false) {
		}

		void start() {
			running_ = true;
			paused_ = false;
			last_reset_time_ = std::chrono::steady_clock::now();
			thread_ = std::thread([this]() {
				while (running_) {
					std::this_thread::sleep_for(std::chrono::milliseconds(10));  // 체크 주기
					auto now = std::chrono::steady_clock::now();
					std::unique_lock<std::mutex> lock(mutex_);

					// 일시정지 상태에서는 타이머 체크를 하지 않음
					if (paused_) {
						continue;
					}

					if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last_reset_time_).count() >= timeout_ms_) {
						if (running_) on_timeout_();
						last_reset_time_ = std::chrono::steady_clock::now();
					}
				}
				});
		}

		void reset() {
			std::lock_guard<std::mutex> lock(mutex_);
			last_reset_time_ = std::chrono::steady_clock::now();
		}

		void pause() {
			std::lock_guard<std::mutex> lock(mutex_);
			paused_ = true;
		}

		void resume() {
			std::lock_guard<std::mutex> lock(mutex_);
			if (paused_) {
				paused_ = false;
				// 일시정지 해제 시 타이머 리셋
				last_reset_time_ = std::chrono::steady_clock::now();
			}
		}

		bool is_paused() const {
			std::lock_guard<std::mutex> lock(mutex_);
			return paused_;
		}

		void stop() {
			running_ = false;
			if (thread_.joinable())
				thread_.join();
		}

		~WatchdogTimer() {
			stop();
		}

	private:
		int timeout_ms_;
		std::function<void()> on_timeout_;
		std::atomic<bool> running_;
		std::atomic<bool> paused_;  // 일시정지 상태 추가
		std::chrono::steady_clock::time_point last_reset_time_;
		mutable std::mutex mutex_;
		std::thread thread_;
	};
}

#endif // AISERVICE_WATCHDOGTIMER_H
