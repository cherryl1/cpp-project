#include "thread_pool.hpp"

#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <thread>
#include <vector>

namespace {

struct PrimeResult {
    std::uint64_t begin;
    std::uint64_t end;
    std::size_t prime_count;
    std::thread::id worker_id;
};

bool is_prime(std::uint64_t value) {
    if (value < 2) {
        return false;
    }
    if (value % 2 == 0) {
        return value == 2;
    }

    for (std::uint64_t divisor = 3; divisor * divisor <= value; divisor += 2) {
        if (value % divisor == 0) {
            return false;
        }
    }
    return true;
}

PrimeResult count_primes(std::uint64_t begin, std::uint64_t end) {
    std::size_t count = 0;
    for (std::uint64_t value = begin; value <= end; ++value) {
        if (is_prime(value)) {
            ++count;
        }
    }

    return {begin, end, count, std::this_thread::get_id()};
}

}  // namespace

int main() {
    const std::size_t hardware_threads =
        std::max(1u, std::thread::hardware_concurrency());
    const std::size_t worker_count = std::min<std::size_t>(4, hardware_threads);

    // 最多缓存 8 个待处理任务；队列满时提交任务会等待。
    mini::ThreadPool pool(worker_count, 8);

    constexpr std::uint64_t range_size = 20'000;
    constexpr std::uint64_t task_count = 12;

    std::vector<std::future<PrimeResult>> futures;
    futures.reserve(task_count);

    for (std::uint64_t i = 0; i < task_count; ++i) {
        const std::uint64_t begin = 1 + i * range_size;
        const std::uint64_t end = begin + range_size - 1;
        futures.push_back(pool.submit(count_primes, begin, end));
    }

    std::size_t total_primes = 0;
    std::cout << "Thread pool demo: " << pool.worker_count()
              << " workers, bounded queue size 8\n\n";
    std::cout << std::left << std::setw(16) << "Range"
              << std::right << std::setw(10) << "Primes"
              << "  Worker\n";
    std::cout << std::string(46, '-') << '\n';

    for (auto& future : futures) {
        const PrimeResult result = future.get();
        total_primes += result.prime_count;

        std::cout << std::left
                  << std::setw(16)
                  << (std::to_string(result.begin) + "-" +
                      std::to_string(result.end))
                  << std::right
                  << std::setw(10) << result.prime_count
                  << "  " << result.worker_id << '\n';
    }

    std::cout << "\nTotal primes: " << total_primes << '\n';
    return 0;
}
