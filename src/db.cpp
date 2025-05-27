#include "db.hpp"
#include <algorithm>
#include <iosfwd>
#include <fstream>

struct Record {
    uint64_t offset;
    uint64_t length;
};

BitcaskDB::BitcaskDB(const std::string& DB_FILE_PATH) : DB_FILE_PATH(DB_FILE_PATH) {
    /* create and/open db_file.file for reading and writing*/
    db_file.open(DB_FILE_PATH, std::ios::out | std::ios::in | std::ios::app);
}

void BitcaskDB::set(const std::string& key, const std::string& val){
    /* 1. append to log file */
    db_file.clear(); // Clear any flags (e.g., EOF)
    db_file.seekp(0, std::ios::end); // Ensure we're at the end
    db_file << key << "," << val << '\n';
    db_file.flush(); // Ensure it's written to disk
}

std::string BitcaskDB::get(const std::string& key){

    std::ifstream read_db_file(DB_FILE_PATH, std::ios::in | std::ios::binary);
    std::streamoff pos;
    std::string line;
    std::string buffer;

    if(!read_db_file) return "";

    /* seek to end of file */
    read_db_file.seekg(0, std::ios::end);
    pos = read_db_file.tellg();

    while(pos > 0){
        pos--;
        read_db_file.seekg(pos);
        char ch;
        read_db_file.get(ch);

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
