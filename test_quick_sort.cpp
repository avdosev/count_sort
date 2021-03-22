#include <iostream>
#include <vector>
#include <random>
#include <fstream>

#include "helper.h"
#include "quick_sort.h"
#include <omp.h>

void time_test();

void data_race_test();

void init_omp() {
    omp_set_dynamic(0);
}

int main() {
    init_omp();
    time_test();
    return 0;
}


void time_test() {
    std::ofstream file;
    file.open("res_quick_sort.csv");
    write_csv_header(file);
    for (size_t N = 1000000; N <= 20000000; N += 1000000)
    {
//        size_t N = 20000000;
        auto arr = build_array(N, N / 10);
        auto arr_copy_seq = arr;
        std::copy(arr.begin(), arr.end(), arr_copy_seq.begin());
        write_csv_data(file, "seq", 1,
                       check_time([&] { quick_sort(arr_copy_seq.begin(), arr_copy_seq.end()); }).count(), N);
        decltype(arr) arr_copy_par;
        arr_copy_par.resize(arr.size());

        std::cout << "start par" << std::endl;
        for (int concurrency = 2; concurrency <= 10; concurrency += 1) {
            omp_set_num_threads(concurrency);
            std::copy(arr.begin(), arr.end(), arr_copy_par.begin());
            auto time = check_time([&] {
                quick_sort_par_omp(arr_copy_seq.begin(), arr_copy_seq.end());
            }).count();
            write_csv_data(file, "par omp", concurrency, time, N);
        }
    }
}

