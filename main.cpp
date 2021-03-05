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

void count_sort_seq_vec(std::vector<data_t> &arr) {
    using size_type = std::vector<data_t>::size_type;
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

void time_test() {
    std::ofstream file;
    file.open("res.csv");
    write_csv_header(file);
    for (size_t N = 1000000; N <= 20000000; N += 1000000)
    {

        auto arr = build_array(N, 1000);
        auto arr_copy_seq = arr;
        std::copy(arr.begin(), arr.end(), arr_copy_seq.begin());
        write_csv_data(file, "seq", 1, check_time([&]{ count_sort_seq_vec(arr_copy_seq); }).count(), N);
        decltype(arr) arr_copy_par;
        arr_copy_par.resize(arr.size());

        std::cout << "start par_atomic" << std::endl;
        for (int concurrency = 2; concurrency <= 10; concurrency+=1) {
            std::copy(arr.begin(), arr.end(), arr_copy_par.begin());
            auto time = check_time([&]{  count_sort_atomic(arr_copy_par, concurrency); }).count();
            write_csv_data(file, "par_atomic", concurrency, time, N);
        }

        std::cout << "start par_mutex" << std::endl;
        for (int concurrency = 2; concurrency < 10; concurrency+=2) {
            std::copy(arr.begin(), arr.end(), arr_copy_par.begin());
            auto time = check_time([&]{  count_sort_mutex(arr_copy_par, concurrency); }).count();
            write_csv_data(file, "par_mutex", concurrency, time, N);
        }

        std::cout << "start par_mutexes" << std::endl;
        for (int concurrency : {2, 7, 29, 50, 100}) {
            std::copy(arr.begin(), arr.end(), arr_copy_par.begin());
            auto time = check_time([&]{  count_sort_mutexes(arr_copy_par, concurrency); }).count();
            write_csv_data(file, "par_mutexes", concurrency, time, N);
        }

        std::cout << "start count_sort_aggregate_seq" << std::endl;
        for (int concurrency = 2; concurrency < 10; concurrency++) {
            std::copy(arr.begin(), arr.end(), arr_copy_par.begin());
            auto time = check_time([&]{  count_sort_aggregate_seq(arr_copy_par, concurrency); }).count();
            write_csv_data(file, "par_aggregate_seq", concurrency, time, N);
        }

        std::cout << "start count_sort_aggregate_atomic" << std::endl;
        for (int concurrency = 2; concurrency < 10; concurrency++) {
            std::copy(arr.begin(), arr.end(), arr_copy_par.begin());
            auto time = check_time([&]{  count_sort_aggregate_atomic(arr_copy_par, concurrency); }).count();
            write_csv_data(file, "par_aggregate_atomic", concurrency, time, N);
        }

        std::cout << "start count_sort_aggregate_mutex" << std::endl;
        for (int concurrency = 2; concurrency < 10; concurrency++) {
            std::copy(arr.begin(), arr.end(), arr_copy_par.begin());
            auto time = check_time([&]{  count_sort_aggregate_mutex(arr_copy_par, concurrency); }).count();
            write_csv_data(file, "par_aggregate_mutex", concurrency, time, N);
        }
    }
}

