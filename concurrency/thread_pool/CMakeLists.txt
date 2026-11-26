cmake_minimum_required(VERSION 3.16)

project(thread_pool_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

add_library(thread_pool INTERFACE)
add_library(mini::thread_pool ALIAS thread_pool)
target_include_directories(thread_pool INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(thread_pool INTERFACE Threads::Threads)
target_compile_features(thread_pool INTERFACE cxx_std_20)

add_executable(thread_pool_demo src/main.cpp)
target_link_libraries(thread_pool_demo PRIVATE mini::thread_pool)

include(CTest)
if(BUILD_TESTING)
    add_executable(thread_pool_test tests/thread_pool_test.cpp)
    target_link_libraries(thread_pool_test PRIVATE mini::thread_pool)
    add_test(NAME thread_pool_test COMMAND thread_pool_test)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(thread_pool INTERFACE -Wall -Wextra -Wpedantic)
endif()
