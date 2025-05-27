#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include "db.hpp" // Replace with your actual DB header

int main() {
    BitcaskDB db("bitcask.db");

    const int total_keys = 10000;
    const int hot_keys_count = 50;
    const int access_per_key = 100;

    std::vector<std::string> all_keys;
    for (int i = 0; i < total_keys; ++i) {
        std::string key = "key" + std::to_string(i);
        std::string val = "val" + std::to_string(i);
        db.set(key, val);
        all_keys.push_back(key);
    }

    std::cout << "Inserted " << total_keys << " key-value pairs.\n";

    // Choose 50 "hot" keys randomly
    std::shuffle(all_keys.begin(), all_keys.end(), std::mt19937{std::random_device{}()});
    std::vector<std::string> hot_keys(all_keys.begin(), all_keys.begin() + hot_keys_count);

    std::cout << "Selected 50 hot keys for stress test.\n";

    // Access pattern (random mix of get/set)
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> op_dist(0, 1); // 0 = get, 1 = set
    std::uniform_int_distribution<int> key_dist(0, hot_keys_count - 1);

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < hot_keys_count * access_per_key; ++i) {
        const std::string& key = hot_keys[key_dist(rng)];
        if (op_dist(rng) == 0) {
            volatile std::string val = db.get(key); // volatile to prevent compiler optimization
        } else {
            db.set(key, "newval" + std::to_string(i));
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Completed " << (hot_keys_count * access_per_key)
              << " mixed get/set operations in " << elapsed.count() << " seconds.\n";

    return 0;
}

