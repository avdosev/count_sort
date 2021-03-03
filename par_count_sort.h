#ifndef COUNTER_SORT_PAR_COUNT_SORT_H
#define COUNTER_SORT_PAR_COUNT_SORT_H

#include <vector>
#include <map>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <algorithm>
#include <atomic>

#include "helper.h"
#include "common.h"


void count_sort_par(std::vector<data_t> &arr, unsigned concurrency) {
    using size_type = std::vector<data_t>::size_type;
    std::vector<std::unordered_map<data_t, size_type>> par_counts(concurrency);

    parallel_exec(concurrency, [&](size_t block){
        size_type start = arr.size() / concurrency * block;
        size_type end = (block == concurrency-1) ? arr.size() : std::min(arr.size() / concurrency * (block+1), arr.size());
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
    std::mutex next_block;

    parallel_exec(concurrency, [&arr, &it, &i, &next_block, &counts](size_t id){
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

void count_sort_par_seq_write(std::vector<data_t> &arr, unsigned concurrency) {
    using size_type = std::vector<data_t>::size_type;
    std::vector<std::unordered_map<data_t, size_type>> par_counts(concurrency);

    parallel_exec(concurrency, [&](size_t block){
        size_type start = arr.size() / concurrency * block;
        size_type end = (block == concurrency-1) ? arr.size() : std::min(arr.size() / concurrency * (block+1), arr.size());
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
    for (auto[item, count]: counts) {
        for (size_type j = 0; j < count; j++) {
            arr[i++] = item;
        }
    }
}

void count_sort_atomic(std::vector<data_t> &arr, unsigned concurrency) {
    if (arr.empty()) return;

    using size_type = std::vector<data_t>::size_type;
    auto max = *std::max_element(arr.begin(), arr.end());
    std::vector<std::atomic<size_type>> counts(max+1);

    parallel_exec(concurrency, [&](size_t block){
        size_type start = arr.size() / concurrency * block;
        size_type end = (block == concurrency-1) ? arr.size() : std::min(arr.size() / concurrency * (block+1), arr.size());
        for (size_type i = start; i < end; i++) {
            auto item = arr[i];
            counts[item].fetch_add(1);
        }
    });

    size_type i = 0;
    for (size_type value = 0; value < counts.size(); value++) {
        auto count = counts[value].load();
        for (size_type k = 0; k < count; k++) {
            arr[i++] = value;
        }
    }
}



#endif //COUNTER_SORT_PAR_COUNT_SORT_H
