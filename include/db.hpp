#ifndef BITCASKDB_H
#define BITCASKDB_H

#include <string>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <thread>

class BitcaskDB {
public:
    explicit BitcaskDB(const std::string& DB_FILE_PATH);
    ~BitcaskDB();
    void set(const std::string& key, const std::string& val);
    std::string get(const std::string& key);
    void compact();
    void print_cache();

private:
    std::string get_with_offset(const std::streamoff& offset);
    std::filesystem::path db_file_path;
    std::fstream db_file;
    std::ifstream reader;
    std::unordered_map<std::string, std::streamoff> cache;
    std::mutex mtx;
    std::atomic<bool> stop_compaction;
    std::thread compaction_thread;
};

#endif // BITCASKDB_H
