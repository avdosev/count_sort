#include <iostream>
#include <vector>
#include <unordered_map>
#include <random>
#include <fstream>

#include "helper.h"
#include "common.h"
#include "par_count_sort.h"

void time_test();

int main() {
    time_test();
    return 0;
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


void time_test() {
    std::ofstream file;
    file.open("res.csv");
    file << "name,concurrency,time\n";
    size_t N = 1000000;
    auto arr = build_array(N, N/10);
    auto arr_copy_seq = arr;
    file << "seq,1," << check_time([&]{ count_sort_seq(arr_copy_seq); }).count() << std::endl;
    decltype(arr) arr_copy_par;
    arr_copy_par.resize(arr.size());
    for (int concurrency = 2; concurrency < 10; concurrency++) {
        std::copy(arr.begin(), arr.end(), arr_copy_par.begin());
        file << "par_baseline," << concurrency << "," << check_time([&]{  count_sort_par(arr_copy_par, concurrency); }).count() << std::endl;
    }
    for (int concurrency = 2; concurrency < 10; concurrency++) {
        std::copy(arr.begin(), arr.end(), arr_copy_par.begin());
        file << "par_seq_write," << concurrency << "," << check_time([&]{  count_sort_par(arr_copy_par, concurrency); }).count() << std::endl;
    }

}

