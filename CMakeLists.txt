cmake_minimum_required(VERSION 3.17)
project(parallel_sorts)

set(CMAKE_CXX_STANDARD 20)

add_executable(counter_sort test_count_sort.cpp count_sort.h helper.h common.h)

find_package(OpenMP REQUIRED)

add_executable(quick_sort test_quick_sort.cpp quick_sort.h helper.h common.h)

target_link_libraries(quick_sort PRIVATE OpenMP::OpenMP_CXX)