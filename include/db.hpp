#ifndef BITCASKDB_H
#define BITCASKDB_H

#include <string>
#include <fstream>
#include <unordered_map>

class BitcaskDB {
public:
    explicit BitcaskDB(const std::string& DB_FILE_PATH);
    void set(const std::string& key, const std::string& val);
    std::string get(const std::string& key);
    void print();

private:
    std::string get_with_offset(const std::streamoff& offset);
    std::string DB_FILE_PATH;
    std::fstream db_file;
    std::unordered_map<std::string, std::streamoff> cache;
    std::ifstream reader;
};

#endif // DB_H

