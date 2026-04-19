#include <cstdint>
#include <iostream>
#include <string>
#include <db.hpp>



int main() {
    BitcaskDB db("kv.db");

    std::cout << "Bitcask DB — type: get / set / print / compact / exit\n";

    // Step 2: Interactive get/set loop
    while (true) {
        std::string command;
        std::cout << "\nEnter command (get/set/print/compact/exit): ";
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

        } else if (command == "print") {
            db.print_cache();

        } else if (command == "compact") {
            db.compact();
            std::cout << "Compaction complete.\n";

        } else if (command == "exit") {
            break;
        } else {
            std::cout << "Invalid command.\n";
        }
    }

    return 0;
}
