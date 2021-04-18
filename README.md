# Parallel sort
В данном репозитории реализованы паралельные версии двух сортировок
1. Быстрая сортировка (Сортировка хоара
2. Сортировка подсчетом

## Count sort
Данная сортировка распараллелина следующими способами:
1. Версия с гонкой данных
2. Подсчет с использованием атомика
3. Подсчет с использованием нескольких мьютексов (от 1 до N)
4. Версии с использованием агрегации
   
   1. Последовательная агрегация
   2. Агрегация синхронизированная атомиком
   3. Агрегация синхронизированная мьютексом

Так же был распараллелина запись в выходной массив

```C++
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
```

## Quick sort

Данная сортировка распараллелина с использование OpenMP и `omp task`
