#include "task_scheduler.hpp"

#include <chrono>
#include <iostream>
#include <numeric>
#include <string>
#include <thread>
#include <vector>

int main() {
    using namespace std::chrono_literals;

    scheduler::TaskScheduler task_scheduler(4);

    std::cout << "Multithreaded Task Scheduler\n";
    std::cout << "Worker threads: " << task_scheduler.worker_count() << "\n\n";

    auto addition = task_scheduler.submit([](int a, int b) {
        return a + b;
    }, 20, 22);

    auto simulated_io = task_scheduler.submit([] {
        std::this_thread::sleep_for(150ms);
        return std::string("Simulated I/O task completed");
    });

    std::vector<std::future<long long>> calculations;
    for (int task_id = 1; task_id <= 8; ++task_id) {
        calculations.push_back(task_scheduler.submit([task_id] {
            std::this_thread::sleep_for(50ms);
            return static_cast<long long>(task_id) * task_id;
        }));
    }

    std::cout << "20 + 22 = " << addition.get() << "\n";
    std::cout << simulated_io.get() << "\n";

    const long long total = std::accumulate(
        calculations.begin(), calculations.end(), 0LL,
        [](long long sum, std::future<long long>& result) {
            return sum + result.get();
        });

    std::cout << "Sum of squared task IDs: " << total << "\n";
    std::cout << "Pending tasks: " << task_scheduler.pending_tasks() << "\n";
    std::cout << "All tasks completed successfully.\n";

    return 0;
}
