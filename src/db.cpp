#include "db.hpp"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>


BitcaskDB::BitcaskDB(const std::string& db_file_path) : db_file_path(db_file_path),
db_file(db_file_path, std::ios::in | std::ios::out | std::ios::app),
reader(db_file_path, std::ios::in | std::ios::binary),
stop_compaction(false)
{
    /* Rebuild in-memory index from existing file so gets work across restarts */
    std::ifstream init_reader(db_file_path, std::ios::in | std::ios::binary);
    std::string line;
    while (true) {
        std::streamoff line_start = init_reader.tellg();
        if (!std::getline(init_reader, line)) break;
        auto delim = line.find(',');
        if (delim != std::string::npos) {
            cache[line.substr(0, delim)] = line_start;
        }
    }

    compaction_thread = std::thread([this]() {
        while (!stop_compaction.load()) {
            for (int i = 0; i < 300 && !stop_compaction.load(); ++i)
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (!stop_compaction.load()) compact();
        }
    });
}

BitcaskDB::~BitcaskDB() {
    stop_compaction.store(true);
    compaction_thread.join();
    db_file.flush();
    db_file.close();
    reader.close();
}


void BitcaskDB::set(const std::string& key, const std::string& val){
    std::lock_guard<std::mutex> lock(mtx);
    db_file.clear();
    db_file.seekp(0, std::ios::end);
    std::streamoff offset = db_file.tellp();
    db_file << key << "," << val << '\n';
    db_file.flush();
    cache[key] = offset;
}

void BitcaskDB::print_cache(){
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << "Current Cache State:\n";
    for (const auto& [k, v] : cache) {
        std::cout << "Key: " << k << ", Offset: " << v << '\n';
    }
}

/* Called only from within get(), which already holds mtx — do not lock here */
std::string BitcaskDB::get_with_offset(const std::streamoff& offset) {
    reader.clear();
    if (!reader) return "";

    reader.seekg(offset);
    if (!reader) return "";

    std::string buffer;
    char ch;

    while (reader.get(ch) && ch != '\n') {
        buffer.push_back(ch);
    }

    auto delim = buffer.find(',');
    if (delim == std::string::npos) return "";

    return buffer.substr(delim + 1);
}


std::string BitcaskDB::get(const std::string& key){
    std::lock_guard<std::mutex> lock(mtx);
    reader.clear();
    std::streamoff pos;
    std::string line;
    std::string buffer;
    if(!reader) return "";

    auto it = cache.find(key);
    if(it != cache.end()) return get_with_offset(it->second);

    reader.seekg(0, std::ios::end);
    pos = reader.tellg();

    while(pos > 0){
        pos--;
        reader.seekg(pos);
        char ch;
        reader.get(ch);

        if (ch == '\n' && !buffer.empty()){
            std::reverse(buffer.begin(), buffer.end());
            auto delim = buffer.find(",");

            if (delim != std::string::npos){
                std::string found_key = buffer.substr(0, delim);
                std::string found_val = buffer.substr(delim + 1);
                if (found_key == key) return found_val;
            }
            buffer.clear();
        } else {
            buffer.push_back(ch);
        }
    }

    /* Check the first line (in case file doesn't end with \n) */
    if (!buffer.empty()) {
        std::reverse(buffer.begin(), buffer.end());
        auto delim = buffer.find(',');
        if (delim != std::string::npos) {
            std::string found_key = buffer.substr(0, delim);
            std::string found_val = buffer.substr(delim + 1);
            if (found_key == key) return found_val;
        }
    }

    return "";
}


void BitcaskDB::compact() {
    std::lock_guard<std::mutex> lock(mtx);
    if (cache.empty()) return;

    auto snapshot = cache;
    auto tmp_path = db_file_path;
    tmp_path += ".tmp";

    std::ofstream tmp(tmp_path, std::ios::out | std::ios::trunc | std::ios::binary);
    if (!tmp) return;

    std::unordered_map<std::string, std::streamoff> new_offsets;
    for (auto& [key, old_offset] : snapshot) {
        reader.clear();
        reader.seekg(old_offset);
        std::string line;
        if (!std::getline(reader, line) || line.empty()) continue;
        std::streamoff new_offset = tmp.tellp();
        tmp << line << '\n';
        new_offsets[key] = new_offset;
    }

    tmp.flush();
    tmp.close();

    db_file.close();
    reader.close();

    std::error_code ec;
    std::filesystem::rename(tmp_path, db_file_path, ec);
    if (ec) {
        db_file.open(db_file_path, std::ios::in | std::ios::out | std::ios::app);
        reader.open(db_file_path, std::ios::in | std::ios::binary);
        return;
    }

    db_file.open(db_file_path, std::ios::in | std::ios::out | std::ios::app);
    reader.open(db_file_path, std::ios::in | std::ios::binary);

    for (auto& [key, new_offset] : new_offsets) {
        cache[key] = new_offset;
    }
    /* Remove keys that couldn't be read (malformed lines) */
    for (auto& [key, _] : snapshot) {
        if (new_offsets.find(key) == new_offsets.end()) {
            cache.erase(key);
        }
    }
}
