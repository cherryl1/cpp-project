# C++ 线程池小项目

这是一个适合学习 C++ 并发编程的完整小项目。它实现了固定大小的线程池，
并使用有界任务队列控制内存占用。

## 功能

- 使用 `std::thread` 创建固定数量的工作线程。
- 使用 `std::mutex` 和两个 `std::condition_variable` 保护任务队列。
- 通过 `std::packaged_task` 和 `std::future` 获取任务返回值。
- 任务抛出的异常由 `future.get()` 重新抛出。
- 有界队列在满载时阻塞提交，形成自然的背压。
- 析构函数或 `shutdown()` 会等待已排队任务完成，再回收线程。
- 线程池不可复制、不可移动，避免资源所有权歧义。

## 编译运行

使用 Make：

```bash
make test
make run
```

或者使用 CMake：

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
./build/thread_pool_demo
```

## 核心调用方式

```cpp
mini::ThreadPool pool(4, 32);  // 4 个工作线程，最多排队 32 个任务

auto future = pool.submit([](int x) {
    return x * x;
}, 12);

int result = future.get();  // 144
```

`submit()` 会先构造任务，再尝试放入队列。如果队列已满，它会等待某个任务被
工作线程取走。因此不要让工作线程执行依赖提交线程继续推进的任务，否则可能死锁。

## 建议的练习

1. 增加 `try_submit()`：队列已满时立即返回 `false`，不阻塞调用方。
2. 把单一的共享队列拆成“每个线程一个队列 + 工作窃取”。
3. 增加 `std::stop_token`，支持协作式取消正在运行的任务。
4. 为 `Task` 增加优先级，观察调度和公平性的变化。
5. 用 sanitizers 和压力测试验证生命周期管理：

```bash
c++ -std=c++20 -Wall -Wextra -Wpedantic -fsanitize=address,undefined \
    -Iinclude tests/thread_pool_test.cpp -o build/thread_pool_test_asan -pthread
./build/thread_pool_test_asan
```
