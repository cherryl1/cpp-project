#include "thread_pool.hpp"

#include <atomic>
#include <cstddef>
#include <functional>
#include <future>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace {

int failures = 0;

void check(bool condition, std::string_view message) {
    if (!condition) {
        ++failures;
        std::cerr << "FAIL: " << message << '\n';
    }
}

void test_returns_values() {
    mini::ThreadPool pool(4, 64);
    std::vector<std::future<int>> futures;

    for (int i = 0; i < 100; ++i) {
        futures.push_back(pool.submit([i] {
            return i * i;
        }));
    }

    int sum = 0;
    for (auto& future : futures) {
        sum += future.get();
    }

    check(sum == 328350, "all task results should be returned");
}

void test_runs_every_task() {
    mini::ThreadPool pool(4, 32);
    std::atomic<int> counter{0};

    for (int i = 0; i < 500; ++i) {
        pool.submit([&counter] {
            counter.fetch_add(1, std::memory_order_relaxed);
        });
    }

    pool.shutdown();
    check(counter.load() == 500, "all submitted tasks should run");
}

void test_exception_is_propagated() {
    mini::ThreadPool pool(2, 8);
    auto future = pool.submit([]() -> int {
        throw std::runtime_error("expected failure");
    });

    bool caught = false;
    try {
        (void)future.get();
    } catch (const std::runtime_error& error) {
        caught = std::string_view(error.what()) == "expected failure";
    }
    check(caught, "task exception should be visible through future");
}

void test_shutdown_is_graceful() {
    std::atomic<int> counter{0};

    {
        mini::ThreadPool pool(3, 128);
        for (int i = 0; i < 200; ++i) {
            pool.submit([&counter] {
                counter.fetch_add(1, std::memory_order_relaxed);
            });
        }
    }

    check(counter.load() == 200,
          "destructor should wait for queued tasks");
}

void test_submit_after_shutdown_is_rejected() {
    mini::ThreadPool pool(1, 4);
    pool.shutdown();

    bool rejected = false;
    try {
        auto future = pool.submit([] { return 42; });
        (void)future.get();
    } catch (const std::runtime_error&) {
        rejected = true;
    }

    check(rejected, "submit after shutdown should throw");
}

}  // namespace

int main() {
    test_returns_values();
    test_runs_every_task();
    test_exception_is_propagated();
    test_shutdown_is_graceful();
    test_submit_after_shutdown_is_rejected();

    if (failures != 0) {
        std::cerr << failures << " test(s) failed\n";
        return 1;
    }

    std::cout << "All thread pool tests passed\n";
    return 0;
}
