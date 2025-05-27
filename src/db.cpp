#include "db.hpp"
#include <algorithm>
#include <iosfwd>
#include <fstream>
#include <iostream>

struct Record {
    uint64_t offset;
    uint64_t length;
};

BitcaskDB::BitcaskDB(const std::string& DB_FILE_PATH) : DB_FILE_PATH(DB_FILE_PATH) {
    db_file.open(DB_FILE_PATH, std::ios::in | std::ios::out | std::ios::app);
    reader.open(DB_FILE_PATH, std::ios::in | std::ios::binary);
}


void BitcaskDB::set(const std::string& key, const std::string& val){
    db_file.clear();
    db_file.seekp(0, std::ios::end);
    std::streamoff offset = db_file.tellp();
    db_file << key << "," << val << '\n';
    db_file.flush();
    cache[key] = offset;
}

void BitcaskDB::print(){
    std::cout << "Current Cache State:\n";
    for (const auto& [k, v] : cache) {
        std::cout << "Key: " << k << ", Offset: " << v << '\n';
    }
}

std::string BitcaskDB::get_with_offset(const std::streamoff& offset) {
    std::cout << "getting with offset" <<std::endl;
    if (!reader) return "";

    reader.seekg(offset);
    if (!reader) return ""; // Check if seek failed

    std::string buffer;
    char ch;

    // Read until newline or EOF
    while (reader.get(ch) && ch != '\n') {
        buffer.push_back(ch);
    }

    // Find delimiter (comma)
    auto delim = buffer.find(',');
    if (delim == std::string::npos) return ""; // Malformed line

    return buffer.substr(delim + 1); // Return value only
}


std::string BitcaskDB::get(const std::string& key){
    std::streamoff pos;
    std::string line;
    std::string buffer;
    if(!reader) return "";
    /* check if key is in cache */
    auto it = cache.find(key);
    if(it != cache.end()) return get_with_offset(it->second);


    /* seek to end of file */
    std::cout << "getting without offset" <<std::endl;
    reader.seekg(0, std::ios::end);
    pos = reader.tellg();

    while(pos > 0){
        pos--;
        reader.seekg(pos);
        char ch;
        reader.get(ch);

        /* start reading backwards */
        if (ch == '\n' && !buffer.empty()){
            std::reverse(buffer.begin(), buffer.end());
            auto delim = buffer.find(",");

            if (delim != std::string::npos){

                std::string found_key = buffer.substr(0, delim);
                std::string found_val = buffer.substr(delim + 1);
                if (found_key == key) return found_val;
            }
            buffer.clear();
        }

        else{
            buffer.push_back(ch);
        }
        
    }


    /* Check the first line (in case file doesn’t end with \n) */
    if (!buffer.empty()) {
        std::reverse(buffer.begin(), buffer.end());
        auto delim = buffer.find(',');
        if (delim != std::string::npos) {
            std::string found_key = buffer.substr(0, delim);
            std::string found_val = buffer.substr(delim + 1);
            if (found_key == key) return found_val;
        }
    }

    /* empty string if not found*/
    return "";
}
