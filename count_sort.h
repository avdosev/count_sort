#ifndef COUNTER_SORT_COUNT_SORT_H
#define COUNTER_SORT_COUNT_SORT_H

#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#include <atomic>

#include "helper.h"
#include "common.h"

using size_type = std::vector<data_t>::size_type;

void count_sort_seq(std::vector<data_t> &arr) {
    auto max = *std::max_element(arr.begin(), arr.end());
    std::vector<size_type> counts(max+1);
    for (auto item: arr) {
        counts[item] += 1;
    }

    size_type i = 0;
    for (size_type value = 0; value < counts.size(); value++) {
        auto count = counts[value];
        for (size_type k = 0; k < count; k++) {
            arr[i++] = value;
        }
    }
}

template <typename SizeT>
void seq_write(std::vector<data_t> &arr, std::vector<SizeT>& counts) {
    size_type i = 0;
    for (size_type value = 0; value < counts.size(); value++) {
        size_type count = counts[value];
        for (size_type k = 0; k < count; k++) {
            arr[i+k] = value;
        }
        i += count;
    }
}

template <typename SizeT>
void parallel_write_v1(std::vector<data_t> &arr, std::vector<SizeT>& counts, unsigned concurrency) {
    parallel_exec(concurrency, [&, concurrency](size_t block){
        size_type i = 0;
        for (size_type value = 0; value < counts.size(); value++) {
            size_type count = counts[value];
            for (size_type k = block; k < count; k+=concurrency) {
                arr[i+k] = value;
            }
            i += count;
        }
    });
}

template <typename SizeT>
void parallel_write_v2(std::vector<data_t> &arr, std::vector<SizeT>& counts, unsigned concurrency) {
    parallel_exec(concurrency, [&, concurrency](size_t block){
        size_type i = 0;
        for (size_type value = 0; value < counts.size(); value++) {
            size_type count = counts[value];
            if (value % concurrency == block) {
                for (size_type k = 0; k < count; k++) {
                    arr[i+k] = value;
                }
            }
            i += count;
        }
    });
}
template <typename SizeT>
void parallel_write_v3(std::vector<data_t> &arr, std::vector<SizeT>& counts, unsigned concurrency) {
    parallel_exec(concurrency, [&, concurrency](size_t block){
        size_type i = 0;
        for (size_type value = 0; value < counts.size(); value++) {
            size_type count = counts[value];
            if (value % concurrency == block) {
                for (size_type k = 0; k < count; k++) {
                    arr[i+k] = value;
                }
            }
            i += count;
        }
    });
}

template <typename SizeT>
void parallel_write(std::vector<data_t> &arr, std::vector<SizeT>& counts, unsigned concurrency) {
    parallel_write_v1(arr, counts, concurrency);
//    parallel_write_v2(arr, counts, concurrency);
}

void count_sort_atomic(std::vector<data_t> &arr, unsigned concurrency) {
    if (arr.empty()) return;

    auto max = *std::max_element(arr.begin(), arr.end());
    std::vector<std::atomic<size_type>> counts(max+1);
    parallel_exec(concurrency, [&](size_t block){
        size_type start = arr.size() / concurrency * block;
        size_type end = (block == concurrency-1) ? arr.size() : std::min(arr.size() / concurrency * (block+1), arr.size());
        for (size_type i = start; i < end; i++) {
            auto item = arr[i];
            counts[item].fetch_add(1, std::memory_order_release);
        }
    });

    size_type i = 0;
    for (size_type value = 0; value < counts.size(); value++) {
        auto count = counts[value].load(std::memory_order_acquire);
        for (size_type k = 0; k < count; k++) {
            arr[i++] = value;
        }
    }
}

void count_sort_data_race(std::vector<data_t> &arr, unsigned concurrency) {
    if (arr.empty()) return;

    auto max = *std::max_element(arr.begin(), arr.end());
    std::vector<size_type> counts(max+1);
    parallel_exec(concurrency, [&](size_t block){
        size_type start = arr.size() / concurrency * block;
        size_type end = (block == concurrency-1) ? arr.size() : std::min(arr.size() / concurrency * (block+1), arr.size());
        for (size_type i = start; i < end; i++) {
            auto item = arr[i];
            counts[item] += 1;
        }
    });

    size_type i = 0;
    for (size_type value = 0; value < counts.size(); value++) {
        auto count = counts[value];
        for (size_type k = 0; k < count; k++) {
            arr.at(i++) = value;
        }
    }
}

void count_sort_mutex(std::vector<data_t> &arr, unsigned concurrency) {
    if (arr.empty()) return;

    auto max = *std::max_element(arr.begin(), arr.end());
    std::vector<size_type> counts(max+1);
    std::mutex counts_mutex;

    parallel_exec(concurrency, [&](size_t block){
        size_type start = arr.size() / concurrency * block;
        size_type end = (block == concurrency-1) ? arr.size() : std::min(arr.size() / concurrency * (block+1), arr.size());
        for (size_type i = start; i < end; i++) {
            std::lock_guard counts_lock{counts_mutex};
            auto item = arr[i];
            counts[item] += 1;
        }
    });

    size_type i = 0;
    for (size_type value = 0; value < counts.size(); value++) {
        auto count = counts[value];
        for (size_type k = 0; k < count; k++) {
            arr[i++] = value;
        }
    }
}

void count_sort_mutexes(std::vector<data_t> &arr, unsigned concurrency) {
    if (arr.empty()) return;

    auto max = *std::max_element(arr.begin(), arr.end());
    std::vector<size_type> counts(max+1);
    std::vector<std::mutex> counts_mutex(concurrency*4);

    parallel_exec(concurrency, [&](size_t block){
        size_type start = arr.size() / concurrency * block;
        size_type end = (block == concurrency-1) ? arr.size() : std::min(arr.size() / concurrency * (block+1), arr.size());
        for (size_type i = start; i < end; i++) {
            auto item = arr[i];
            std::lock_guard counts_lock{counts_mutex[item%counts_mutex.size()]};
            counts[item] += 1;
        }
    });

    size_type i = 0;
    for (size_type value = 0; value < counts.size(); value++) {
        auto count = counts[value];
        for (size_type k = 0; k < count; k++) {
            arr[i++] = value;
        }
    }
}

template <bool par_write=true>
void count_sort_aggregate_seq(std::vector<data_t> &arr, unsigned concurrency) {
    if (arr.empty()) return;

    std::vector<std::vector<size_type>> locals_counts(concurrency);


    parallel_exec(concurrency, [&](size_t block){
        size_type start = arr.size() / concurrency * block;
        size_type end = (block == concurrency-1) ? arr.size() : std::min(arr.size() / concurrency * (block+1), arr.size());
        auto& local_counts = locals_counts[block];
        auto max = *std::max_element(arr.begin()+start, arr.begin()+end);
        local_counts.resize(max+1);
        for (size_type i = start; i < end; i++) {
            auto item = arr[i];
            local_counts[item] += 1;
        }
    });

    auto max_i = std::max_element(locals_counts.begin(),locals_counts.end(),[](auto c1, auto c2){
        return c1.size() > c2.size();
    }) - locals_counts.begin();

    auto& counts = locals_counts[max_i];
    for (size_type i = 0; i < locals_counts.size(); i++) {
        if (i == max_i) continue;
        auto& local_count = locals_counts[i];
        for (size_type j = 0; j < local_count.size(); j++) {
            counts[j] += local_count[j];
        }
    }

    if constexpr (par_write) {
        parallel_write(arr, counts, concurrency);
    } else {
        seq_write(arr, counts);
    }
}

template <bool par_write=true>
void count_sort_aggregate_atomic(std::vector<data_t> &arr, unsigned concurrency) {
    if (arr.empty()) return;

    std::vector<std::vector<size_type>> locals_counts(concurrency);
    auto max = *std::max_element(arr.begin(), arr.end());
    std::vector<std::atomic<size_type>> counts(max);

    parallel_exec(concurrency, [&](size_t block){
        size_type start = arr.size() / concurrency * block;
        size_type end = (block == concurrency-1) ? arr.size() : std::min(arr.size() / concurrency * (block+1), arr.size());
        auto& local_counts = locals_counts[block];
        local_counts.resize(max+1);
        for (size_type i = start; i < end; i++) {
            auto item = arr[i];
            local_counts[item] += 1;
        }

        auto lcs = local_counts.size();
        for (size_type i = 0; i < lcs; i++) {
            counts[i].fetch_add(local_counts[i], std::memory_order_release);
        }
    });

    if constexpr (par_write) {
        parallel_write(arr, counts, concurrency);
    } else {
        seq_write(arr, counts);
    }
}

template <bool par_write=true>
void count_sort_aggregate_mutex(std::vector<data_t> &arr, unsigned concurrency) {
    if (arr.empty()) return;

    std::vector<std::vector<size_type>> locals_counts(concurrency);
    std::vector<size_type> counts;
    std::mutex counts_mutex;

    parallel_exec(concurrency, [&](size_t block){
        size_type start = arr.size() / concurrency * block;
        size_type end = (block == concurrency-1) ? arr.size() : std::min(arr.size() / concurrency * (block+1), arr.size());
        auto& local_counts = locals_counts[block];
        auto max = *std::max_element(arr.begin()+start, arr.begin()+end);
        local_counts.resize(max+1);
        for (size_type i = start; i < end; i++) {
            auto item = arr[i];
            local_counts[item] += 1;
        }
        {
            std::lock_guard counts_lock{counts_mutex};
            auto lcs = local_counts.size();
            if (counts.size() < lcs) counts.resize(lcs);
            for (size_type i = 0; i < lcs; i++) {
                counts[i] += local_counts[i];
            }
        }
    });

    if constexpr (par_write) {
        parallel_write(arr, counts, concurrency);
    } else {
        seq_write(arr, counts);
    }
}



#endif //COUNTER_SORT_COUNT_SORT_H
