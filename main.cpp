#include <iostream>
#include <vector>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <random>
#include <map>

using data_t = unsigned;

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

void print_arr(const std::vector<data_t>& collection) {
    std::cout << "[ ";
    for (auto item: collection) {
        std::cout << item << ' ';
    }
    std::cout << "]";
}

void count_sort_seq(std::vector<data_t> &arr) {
    using size_type = std::vector<data_t>::size_type;
    std::map<data_t, size_type> counts;
    for (auto item: arr) {
        if (auto it = counts.find(item); it == counts.end()) {
            counts.insert({item, 1});
        } else {
            it->second++;
        }
    }

    size_type i = 0;
    for (auto[item, count]: counts) {
        for (size_type j = 0; j < count; j++) {
            arr[i++] = item;
        }
    }
}

template<typename Function>
void parallel_exec(size_t concurrency, Function f);

void count_sort_par(std::vector<data_t> &arr, unsigned concurrency) {
    using size_type = std::vector<data_t>::size_type;
    std::vector<std::unordered_map<data_t, size_type>> par_counts(concurrency);

    parallel_exec(concurrency, [&](size_t block){
        size_type start = arr.size() / concurrency * block;
        size_type end = std::min(arr.size() / concurrency * (block+1), arr.size());
        auto& counts = par_counts[block];
        for (size_type i = start; i < end; i++) {
            auto item = arr[i];
            if (auto it = counts.find(item); it == counts.end()) {
                counts.insert({item, 1});
            } else {
                it->second++;
            }
        }
    });

    std::map<data_t, size_type> counts;
    for (unsigned i = 0; i < concurrency; i++) {
        for (auto[item, count]: par_counts[i]) {
            if (auto it = counts.find(item); it == counts.end()) {
                counts.insert({item, count});
            } else {
                it->second+=count;
            }
        }
    }


    size_type i = 0;
    auto it = counts.begin();
    std::vector<std::thread> threads;
    threads.reserve(2);
    std::mutex next_block;

    for (size_t t = 0; t < 2; ++t) {
        threads.emplace_back([&arr, &it, &i, &next_block, &counts](){
            decltype(it) lock_it;
            size_type start, end;
            data_t value;
            while (true) {
                {
                    std::lock_guard lock{next_block};
                    lock_it = it;
                    if (lock_it == counts.end()) {
                        break;
                    }
                    ++it;
                    start = i;
                    i += lock_it->second;
                }
                value = lock_it->first;
                end = lock_it->second + start;
                for (auto j = start; j < end; j++) {
                    arr[j] = value;
                }
            }
        });
    }

    for (auto &t : threads) {
        t.join();
    }
}

void time_test();

int main() {
    time_test();
    return 0;
}

template <typename Func>
std::chrono::duration<double> check_time(Func&& f) {
    auto start = std::chrono::steady_clock::now();
    f();
    auto stop = std::chrono::steady_clock::now();
    return stop-start;
}


void time_test() {
    size_t N = 10000000;
    auto arr = build_array(N, N/10);
    auto arr_copy_seq = arr;
    std::cout << "seq: " << check_time([&]{ count_sort_seq(arr_copy_seq); }).count() << std::endl;
    for (int concurrency = 2; concurrency < 10; concurrency++) {
        auto arr_copy_par = arr;
        std::cout << "par " << concurrency << ": " << check_time([&]{  count_sort_par(arr_copy_par, concurrency); }).count() << std::endl;
    }
}

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
