#include <iostream>
#include <vector>
#include <random>
#include <fstream>

#include "helper.h"
#include "count_sort.h"

void time_test();
void data_race_test();

int main() {
    data_race_test();
    time_test();
    return 0;
}

void time_test() {
    std::ofstream file;
    file.open("res.csv");
    write_csv_header(file);
//    for (size_t N = 1000000; N <= 20000000; N += 1000000)
    {
        size_t N = 1000000000;
        auto arr = build_array(N, 1000);
        auto arr_copy_seq = arr;
        std::copy(arr.begin(), arr.end(), arr_copy_seq.begin());
        write_csv_data(file, "seq", 1, check_time([&]{ count_sort_seq(arr_copy_seq); }).count(), N);
        decltype(arr) arr_copy_par;
        arr_copy_par.resize(arr.size());

//        std::cout << "start par_atomic" << std::endl;
//        for (int concurrency = 2; concurrency <= 10; concurrency+=1) {
//            std::copy(arr.begin(), arr.end(), arr_copy_par.begin());
//            auto time = check_time([&]{  count_sort_atomic(arr_copy_par, concurrency); }).count();
//            write_csv_data(file, "par_atomic", concurrency, time, N);
//        }
//
//        std::cout << "start par_mutex" << std::endl;
//        for (int concurrency = 2; concurrency < 10; concurrency+=2) {
//            std::copy(arr.begin(), arr.end(), arr_copy_par.begin());
//            auto time = check_time([&]{  count_sort_mutex(arr_copy_par, concurrency); }).count();
//            write_csv_data(file, "par_mutex", concurrency, time, N);
//        }
//
//        std::cout << "start par_mutexes" << std::endl;
//        for (int concurrency : {2, 7, 29, 50, 100}) {
//            std::copy(arr.begin(), arr.end(), arr_copy_par.begin());
//            auto time = check_time([&]{  count_sort_mutexes(arr_copy_par, concurrency); }).count();
//            write_csv_data(file, "par_mutexes", concurrency, time, N);
//        }

        std::cout << "start count_sort_aggregate_seq_par_write" << std::endl;
        for (int concurrency = 2; concurrency < 10; concurrency++) {
            std::copy(arr.begin(), arr.end(), arr_copy_par.begin());
            auto time = check_time([&]{  count_sort_aggregate_seq<true>(arr_copy_par, concurrency); }).count();
            write_csv_data(file, "par_aggregate_seq_par_write", concurrency, time, N);
        }

        std::cout << "start count_sort_aggregate_atomic_par_write" << std::endl;
        for (int concurrency = 2; concurrency < 10; concurrency++) {
            std::copy(arr.begin(), arr.end(), arr_copy_par.begin());
            auto time = check_time([&]{  count_sort_aggregate_atomic<true>(arr_copy_par, concurrency); }).count();
            write_csv_data(file, "par_aggregate_atomic_par_write", concurrency, time, N);
        }

        std::cout << "start count_sort_aggregate_mutex_par_write" << std::endl;
        for (int concurrency = 2; concurrency < 10; concurrency++) {
            std::copy(arr.begin(), arr.end(), arr_copy_par.begin());
            auto time = check_time([&]{  count_sort_aggregate_mutex<true>(arr_copy_par, concurrency); }).count();
            write_csv_data(file, "par_aggregate_mutex_par_write", concurrency, time, N);
        }

        std::cout << "start count_sort_aggregate_seq_seq_write" << std::endl;
        for (int concurrency = 2; concurrency < 10; concurrency++) {
            std::copy(arr.begin(), arr.end(), arr_copy_par.begin());
            auto time = check_time([&]{  count_sort_aggregate_seq<false>(arr_copy_par, concurrency); }).count();
            write_csv_data(file, "par_aggregate_seq_seq_write", concurrency, time, N);
        }

        std::cout << "start count_sort_aggregate_atomic_seq_write" << std::endl;
        for (int concurrency = 2; concurrency < 10; concurrency++) {
            std::copy(arr.begin(), arr.end(), arr_copy_par.begin());
            auto time = check_time([&]{  count_sort_aggregate_atomic<false>(arr_copy_par, concurrency); }).count();
            write_csv_data(file, "par_aggregate_atomic_seq_write", concurrency, time, N);
        }

        std::cout << "start count_sort_aggregate_mutex_seq_write" << std::endl;
        for (int concurrency = 2; concurrency < 10; concurrency++) {
            std::copy(arr.begin(), arr.end(), arr_copy_par.begin());
            auto time = check_time([&]{  count_sort_aggregate_mutex<false>(arr_copy_par, concurrency); }).count();
            write_csv_data(file, "par_aggregate_mutex_seq_write", concurrency, time, N);
        }
    }
}

void data_race_test() {
    size_t N = 1000000;
    auto arr = build_array(N, 100);

    auto arr_copy_seq = arr;
    count_sort_seq(arr_copy_seq);

    auto arr_copy_data_race = arr;

    try {
        count_sort_data_race(arr_copy_data_race, 4);
    } catch (std::exception& ex) {
        std::cout << "error in sort" << std::endl;
    }

    if (!equal_array(arr_copy_data_race, arr_copy_seq)) {
        size_t error_count = 0;
        for (size_t i = 0; i < N; i++) {
            error_count += arr_copy_data_race[i] != arr_copy_seq[i];
        }
        std::cout << "errors: " << error_count << std::endl;
    }
}