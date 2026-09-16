# Multithreaded Task Scheduler

A lightweight, reusable task scheduler implemented in modern C++17 for Linux. It uses a fixed-size thread pool and a thread-safe task queue to execute independent tasks concurrently.

## Features

- Fixed-size worker thread pool
- Thread-safe task queue
- `std::mutex` and `std::condition_variable` synchronization
- Generic task submission with C++ templates
- `std::future` support for retrieving return values and exceptions
- Graceful shutdown using RAII
- CMake build system
- Automated tests with CTest

## Project structure

```text
.
├── CMakeLists.txt
├── README.md
├── include/
│   └── task_scheduler.hpp
├── src/
│   └── main.cpp
└── tests/
    └── test_task_scheduler.cpp
```

## Requirements

- Linux, macOS, or WSL
- C++17-compatible compiler such as GCC 9+ or Clang 10+
- CMake 3.16+

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

## Run

```bash
./build/task_scheduler_demo
```

Example output:

```text
Multithreaded Task Scheduler
Worker threads: 4

20 + 22 = 42
Simulated I/O task completed
Sum of squared task IDs: 204
Pending tasks: 0
All tasks completed successfully.
```

## Test

```bash
ctest --test-dir build --output-on-failure
```

## Design overview

1. The constructor starts a fixed number of worker threads.
2. `submit()` wraps the callable in a `std::packaged_task` and pushes it into the queue.
3. A condition variable wakes an available worker.
4. The worker removes one task while holding the queue mutex, then executes it without holding the mutex.
5. The returned `std::future` provides the task result to the caller.
6. `shutdown()` stops accepting new work, allows queued tasks to finish, and joins all workers.

## Concurrency and safety notes

- The queue is protected by a mutex.
- Workers sleep efficiently when no tasks are available.
- The scheduler drains queued tasks before worker exit during normal shutdown.
- Task exceptions are captured by `std::future` and rethrown when `get()` is called.
- The class is non-copyable to avoid accidental ownership ambiguity.

## Possible extensions

- Task priorities
- Scheduled execution at a future time
- Task cancellation
- Dynamic worker scaling
- Metrics for queue length and execution time
- Logging and tracing

## Resume description

**Multithreaded Task Scheduler | C++ | Linux | OOP | Multithreading**

- Developed a multithreaded task scheduler using C++17 and Linux.
- Implemented a fixed-size thread pool, thread-safe task queue, mutexes, and condition variables for concurrent task execution.
- Used templates, `std::future`, RAII, and graceful shutdown to build reusable and reliable software components.
