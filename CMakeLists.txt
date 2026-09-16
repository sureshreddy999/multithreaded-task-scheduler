cmake_minimum_required(VERSION 3.16)
project(multithreaded_task_scheduler VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(task_scheduler INTERFACE)
target_include_directories(task_scheduler INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(task_scheduler INTERFACE -Wall -Wextra -Wpedantic)

find_package(Threads REQUIRED)
target_link_libraries(task_scheduler INTERFACE Threads::Threads)

add_executable(task_scheduler_demo src/main.cpp)
target_link_libraries(task_scheduler_demo PRIVATE task_scheduler)

option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_executable(task_scheduler_tests tests/test_task_scheduler.cpp)
    target_link_libraries(task_scheduler_tests PRIVATE task_scheduler)
    add_test(NAME task_scheduler_tests COMMAND task_scheduler_tests)
endif()
