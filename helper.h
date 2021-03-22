#ifndef COUNTER_SORT_HELPER_H
#define COUNTER_SORT_HELPER_H

#include <vector>
#include <thread>
#include <algorithm>
#include "common.h"

template<typename Function>
void parallel_exec(size_t concurrency, Function f) {
    std::vector<std::thread> threads;
    threads.reserve(concurrency);

    for (size_t i = 0; i < concurrency; ++i) {
        threads.emplace_back(f, i);
    }

    for (auto &t : threads) {
        t.join();
    }
}

template <typename Func>
std::chrono::duration<double> check_time(Func&& f, size_t count = 1) {
    auto start = std::chrono::steady_clock::now();
    for (size_t i = 0; i < count; i++)
        f();
    auto stop = std::chrono::steady_clock::now();
    return (stop-start) / count;
}

template<typename ValueType>
void print_arr(const std::vector<ValueType>& collection) {
    std::cout << "[ ";
    for (auto item: collection) {
        std::cout << item << ' ';
    }
    std::cout << "]";
}

template<typename ValueType>
bool equal_array(const std::vector<ValueType>& collection1, const std::vector<ValueType>& collection2) {
    if (collection1 != collection2) {
        std::cout << "not equal" << std::endl;
        return false;
    } else {
        std::cout << "equal" << std::endl;
        return true;
    }
}

auto build_array(size_t size, unsigned M) {
    std::vector<data_t> arr;
    arr.reserve(size);
    std::mt19937 gen;
    std::uniform_int_distribution<data_t> distrib(0, M);
    for (size_t i = 0; i < size; i++) {
        arr.push_back(distrib(gen));
    }
    return arr;
}

void write_csv_header(std::ostream& stream) {
    stream << "name,concurrency,time,size" << std::endl;
}

void write_csv_data(std::ostream& stream, std::string_view name, size_t concurrency, double time, size_t size) {
    stream << name << ',' << concurrency << ',' << time << ',' << size << std::endl;
}

#endif //COUNTER_SORT_HELPER_H
