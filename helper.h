#ifndef COUNTER_SORT_HELPER_H
#define COUNTER_SORT_HELPER_H

#include <vector>
#include <thread>

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
std::chrono::duration<double> check_time(Func&& f) {
    auto start = std::chrono::steady_clock::now();
    f();
    auto stop = std::chrono::steady_clock::now();
    return stop-start;
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
void equal_array(const std::vector<ValueType>& collection1, const std::vector<ValueType>& collection2) {
    if (collection1 != collection2) {
        std::cout << "not equal" << std::endl;
        print_arr(collection1);
        print_arr(collection2);
        std::cout << std::endl;
    }
    else std::cout << "equal" << std::endl;
}

#endif //COUNTER_SORT_HELPER_H
