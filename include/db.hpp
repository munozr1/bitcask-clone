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

private:
    std::string DB_FILE_PATH;
    std::fstream db_file;
    std::unordered_map<std::string, std::string> cache;
};

#endif // DB_H

