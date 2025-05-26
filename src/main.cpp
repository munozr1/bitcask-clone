#include <algorithm>
#include <cstdint>
#include <iosfwd>
#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>

struct Record {
    uint64_t offset;
    uint64_t key_hash;
};


struct DB{

    DB(const std::string& DB_FILE_PATH) : DB_FILE_PATH(DB_FILE_PATH) {
        /* create and/open db file for reading and writing*/
        db.open(DB_FILE_PATH, std::ios::out | std::ios::in | std::ios::app);
    }

    void set(const std::string& key, const std::string& val){
        db.clear(); // Clear any flags (e.g., EOF)
        db.seekp(0, std::ios::end); // Ensure we're at the end
        db << key << "," << val << '\n';
        db.flush(); // Ensure it's written to disk
    }

    std::string get(const std::string& key){


        std::ifstream read_db(DB_FILE_PATH, std::ios::in | std::ios::binary);
        std::streamoff pos;
        std::string line;
        std::string buffer;

        if(!read_db) return "";

        /* seek to end of file */
        read_db.seekg(0, std::ios::end);
        pos = read_db.tellg();

        while(pos > 0){
            pos--;
            read_db.seekg(pos);
            char ch;
            read_db.get(ch);

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

    
    private:
        std::string DB_FILE_PATH;
        std::fstream db;
        std::unordered_map<std::string, std::string> cache;

};


int main() {
    DB mydb("kv.db");

    mydb.set("foo", "bar");
    mydb.set("baz", "qux");
    mydb.set("foo", "newbar");

    std::cout << "foo = " << mydb.get("foo") << '\n'; // newbar
    std::cout << "baz = " << mydb.get("baz") << '\n'; // qux
    std::cout << "not_here = " << mydb.get("not_here") << '\n'; // empty

    return 0;
}
