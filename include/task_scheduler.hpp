#pragma once

#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace scheduler {

class TaskScheduler {
public:
    explicit TaskScheduler(std::size_t worker_count = std::thread::hardware_concurrency())
        : stopping_(false) {
        if (worker_count == 0) {
            worker_count = 1;
        }

        workers_.reserve(worker_count);
        for (std::size_t i = 0; i < worker_count; ++i) {
            workers_.emplace_back(&TaskScheduler::worker_loop, this);
        }
    }

    TaskScheduler(const TaskScheduler&) = delete;
    TaskScheduler& operator=(const TaskScheduler&) = delete;
    TaskScheduler(TaskScheduler&&) = delete;
    TaskScheduler& operator=(TaskScheduler&&) = delete;

    ~TaskScheduler() {
        shutdown();
    }

    template <typename Function, typename... Args>
    auto submit(Function&& function, Args&&... args)
        -> std::future<std::invoke_result_t<Function, Args...>> {
        using result_type = std::invoke_result_t<Function, Args...>;

        auto packaged_task = std::make_shared<std::packaged_task<result_type()>>(
            std::bind(std::forward<Function>(function), std::forward<Args>(args)...));

        std::future<result_type> result = packaged_task->get_future();
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            if (stopping_) {
                throw std::runtime_error("Cannot submit task after scheduler shutdown");
            }

            tasks_.emplace([packaged_task]() { (*packaged_task)(); });
        }
        condition_.notify_one();
        return result;
    }

    void shutdown() noexcept {
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            if (stopping_) {
                return;
            }
            stopping_ = true;
        }

        condition_.notify_all();
        for (std::thread& worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

    std::size_t pending_tasks() const {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        return tasks_.size();
    }

    std::size_t worker_count() const noexcept {
        return workers_.size();
    }

private:
    using Task = std::function<void()>;

    void worker_loop() {
        while (true) {
            Task task;
            {
                std::unique_lock<std::mutex> lock(queue_mutex_);
                condition_.wait(lock, [this] {
                    return stopping_ || !tasks_.empty();
                });

                if (stopping_ && tasks_.empty()) {
                    return;
                }

                task = std::move(tasks_.front());
                tasks_.pop();
            }

            try {
                task();
            } catch (...) {
                // Exceptions are delivered to the caller through std::future.
            }
        }
    }

    mutable std::mutex queue_mutex_;
    std::condition_variable condition_;
    std::queue<Task> tasks_;
    std::vector<std::thread> workers_;
    bool stopping_;
};

}  // namespace scheduler
