#include "task_scheduler.hpp"

#include <atomic>
#include <cassert>
#include <stdexcept>
#include <vector>

int main() {
    scheduler::TaskScheduler task_scheduler(4);

    auto result = task_scheduler.submit([](int value) {
        return value * 2;
    }, 21);
    assert(result.get() == 42);

    std::atomic<int> completed{0};
    std::vector<std::future<void>> tasks;
    for (int i = 0; i < 100; ++i) {
        tasks.push_back(task_scheduler.submit([&completed] {
            ++completed;
        }));
    }

    for (auto& task : tasks) {
        task.get();
    }
    assert(completed == 100);

    task_scheduler.shutdown();
    bool rejected = false;
    try {
        task_scheduler.submit([] {});
    } catch (const std::runtime_error&) {
        rejected = true;
    }
    assert(rejected);
    (void)rejected;

    return 0;
}
