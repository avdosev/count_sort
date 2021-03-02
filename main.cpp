#include <iostream>
#include <vector>
#include <thread>
#include <unordered_map>
#include <map>

int main() {

    return 0;
}

void count_sort_seq(std::vector<int> &arr) {
    using size_type = std::vector<int>::size_type;
    std::map<int, size_type> counts;
    for (auto item: arr) {
        if (auto it = counts.find(item); it == counts.end()) {
            counts.insert({item, 1});
        } else {
            it->second++;
        }
    }

    size_type i = 0;
    for (auto [item, count]: counts) {
        for (size_type j = 0; j < count; j++) {
            arr[i++] = item;
        }
    }
}
