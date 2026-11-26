#pragma once

#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace mini {

// 一个支持有界任务队列的固定大小线程池。
class ThreadPool {
public:
    explicit ThreadPool(
        std::size_t worker_count = std::thread::hardware_concurrency(),
        std::size_t max_pending_tasks = 1024)
        : worker_count_(worker_count == 0 ? 1 : worker_count),
          max_pending_tasks_(max_pending_tasks == 0 ? 1 : max_pending_tasks) {
        workers_.reserve(worker_count_);

        try {
            for (std::size_t i = 0; i < worker_count_; ++i) {
                workers_.emplace_back(&ThreadPool::worker_loop, this);
            }
        } catch (...) {
            // 如果有线程创建失败，先结束已创建的线程，再抛出异常。
            shutdown();
            throw;
        }
    }

    ~ThreadPool() {
        shutdown();
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    // 提交任务。调用方可通过返回的 future 等待结果或获取异常。
    // 当队列已满且没有 shutdown 时，submit 会阻塞以形成背压。
    template <typename F, typename... Args>
    auto submit(F&& function, Args&&... args)
        -> std::future<std::invoke_result_t<F, Args...>> {
        using Result = std::invoke_result_t<F, Args...>;

        auto task = std::make_shared<std::packaged_task<Result()>>(
            std::bind(std::forward<F>(function), std::forward<Args>(args)...));
        std::future<Result> result = task->get_future();

        enqueue([task = std::move(task)]() mutable {
            (*task)();
        });

        return result;
    }

    // 停止接收新任务，等待已排队任务全部执行，然后回收线程。
    // 可以重复调用；不要在线程池自己的工作线程中销毁线程池。
    void shutdown() {
        std::call_once(shutdown_once_, [this] {
            {
                std::lock_guard lock(mutex_);
                accepting_tasks_ = false;
            }

            not_empty_.notify_all();
            not_full_.notify_all();

            for (std::thread& worker : workers_) {
                if (worker.joinable()) {
                    worker.join();
                }
            }
        });
    }

    std::size_t worker_count() const noexcept {
        return worker_count_;
    }

    std::size_t pending_tasks() const {
        std::lock_guard lock(mutex_);
        return tasks_.size();
    }

private:
    using Task = std::function<void()>;

    void enqueue(Task task) {
        {
            std::unique_lock lock(mutex_);
            not_full_.wait(lock, [this] {
                return !accepting_tasks_ ||
                       tasks_.size() < max_pending_tasks_;
            });

            if (!accepting_tasks_) {
                throw std::runtime_error("thread pool has been shut down");
            }

            tasks_.push(std::move(task));
        }

        not_empty_.notify_one();
    }

    void worker_loop() {
        while (true) {
            Task task;
            {
                std::unique_lock lock(mutex_);
                not_empty_.wait(lock, [this] {
                    return !accepting_tasks_ || !tasks_.empty();
                });

                if (tasks_.empty()) {
                    return;
                }

                task = std::move(tasks_.front());
                tasks_.pop();
            }

            not_full_.notify_one();

            // packaged_task 会把普通异常写入 future，这里再兜底一次，
            // 防止任何意外异常导致工作线程直接退出。
            try {
                task();
            } catch (...) {
            }
        }
    }

    const std::size_t worker_count_;
    const std::size_t max_pending_tasks_;

    mutable std::mutex mutex_;
    std::condition_variable not_empty_;
    std::condition_variable not_full_;
    std::queue<Task> tasks_;
    std::vector<std::thread> workers_;
    bool accepting_tasks_{true};
    std::once_flag shutdown_once_;
};

}  // namespace mini
