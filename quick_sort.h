#ifndef SORT_QUICK_SORT_H
#define SORT_QUICK_SORT_H

#include <algorithm>
#include <omp.h>

template<class RandIter>
void quick_sort(RandIter first, RandIter last) {
    auto start = first, end = last;
    auto mid = start + (end - start) / 2;
    --end;
    do {
        while (*start < *mid) {
            start++;
        }

        while (*end > *mid) {
            end--;
        }

        if (start <= end) {
            std::swap(*start, *end);
            start++;
            end--;
        }
    } while (start <= end);


    if (end > first) {
        quick_sort(first, end + 1);
    }
    if (start < last) {
        quick_sort(start, last);
    }
}

template<class RandIter>
void quick_sort_par_omp_impl(RandIter first, RandIter last) {
    auto start = first, end = last;
    auto mid = start + (end - start) / 2;
    --end;
    do {
        while (*start < *mid) {
            start++;
        }

        while (*end > *mid) {
            end--;
        }

        if (start <= end) {
            std::swap(*start, *end);
            start++;
            end--;
        }
    } while (start <= end);


    if (end > first) {
#pragma omp task
        {
            if (end - first <= 150) {
                quick_sort(first, end + 1);
            } else {
                quick_sort_par_omp_impl(first, end + 1);
            }
        }
    }

    if (start < last) {
        if (last - start <= 150) {
            quick_sort(start, last);
        } else {
            quick_sort_par_omp_impl(start, last);
        }
    }


#pragma omp taskwait
}

template<class RandIter>
void quick_sort_par_omp(RandIter first, RandIter last) {
#pragma omp parallel
    {
#pragma omp single
        {
            quick_sort_par_omp_impl(first, last);
        }
    }
}

#endif // SORT_QUICK_SORT_H
