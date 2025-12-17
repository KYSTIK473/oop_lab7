cmake_minimum_required(VERSION 3.10)
project(BalagurFate3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(balagur_async
    src/main_async.cpp
)

target_include_directories(balagur_async PRIVATE src/)

find_package(GTest REQUIRED)

add_executable(tests_lab7
    tests/test_async.cpp
)

target_include_directories(tests_lab7 PRIVATE src/)
target_link_libraries(tests_lab7 GTest::gtest GTest::gtest_main)

enable_testing()
add_test(NAME Lab7Tests COMMAND tests_lab7)