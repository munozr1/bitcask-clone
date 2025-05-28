#include <cstdint>
#include <iostream>
#include <string>
#include <db.hpp>



int main() {
    BitcaskDB db("bitcask2.db");

    // Step 1: Initialize with 1000 key-value pairs
    /*for (int i = 0; i < 100; ++i) {
        db.set("key" + std::to_string(i), "val" + std::to_string(i));
    }
     */

    std::cout << "Initialized 1000 key-value pairs.\n";

    // Step 2: Interactive get/set loop
    while (true) {
        std::string command;
        std::cout << "\nEnter command (get/set/print/exit): ";
        std::cin >> command;

        if (command == "get") {
            std::string key;
            std::cout << "Enter key: ";
            std::cin >> key;
            std::string value = db.get(key);
            if (value.empty()) {
                std::cout << "Key not found.\n";
            } else {
                std::cout << "Value: " << value << "\n";
            }

        } else if (command == "set") {
            std::string key, val;
            std::cout << "Enter key: ";
            std::cin >> key;
            std::cout << "Enter value: ";
            std::cin >> val;
            db.set(key, val);
            std::cout << "Key-value pair set.\n";

        } else if (command == "print"){
            db.print_cache();
            
        }else if (command == "exit") {
            break;
        } else {
            std::cout << "Invalid command.\n";
        }
    }

    return 0;
}
