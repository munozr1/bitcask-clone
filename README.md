# Bitcask Database

A lightweight, file-based key-value store database implementation inspired by the Bitcask storage model. This project demonstrates the implementation of a simple but efficient database system from scratch in C++.

## Overview

Bitcask is a log-structured hash table that provides fast reads and writes by maintaining an in-memory hash table of keys to file offsets. This implementation includes the core functionality of a key-value store with the following features:

- Fast writes through append-only operations
- Efficient reads using an in-memory index
- Simple key-value storage model
- File-based persistence

## Architecture

The database uses a simple but effective architecture:

1. **Storage Format**: Data is stored in a simple CSV-like format where each record is stored as `key,value\n`
2. **In-Memory Index**: A hash table (std::unordered_map) maintains the mapping between keys and their file offsets
3. **Append-Only Log**: All writes are appended to the end of the file, ensuring write performance
4. **Read Optimization**: Reads are optimized using the in-memory index for direct access

## Features

- `set(key, value)`: Store a key-value pair in the database
- `get(key)`: Retrieve a value by its key
- Persistent storage to disk
- In-memory caching for fast lookups
- Simple and efficient implementation

## Building the Project

The project uses CMake as its build system. To build:

```bash
mkdir build
cd build
cmake ..
make
```

## Usage Example

```cpp
#include "db.hpp"

int main() {
    BitcaskDB db("kv.db");
    
    // Store some data
    db.set("name", "John Doe");
    db.set("age", "30");
    
    // Retrieve data
    std::string name = db.get("name");  // Returns "John Doe"
    std::string age = db.get("age");    // Returns "30"
    
    return 0;
}
```


## Project Structure

```
.
├── include/
│   └── db.hpp          # Database interface
├── src/
│   ├── db.cpp          # Database implementation
│   └── main.cpp        # Example usage
├── test/               # Test files
├── benchmarks/         # Performance benchmarks
└── CMakeLists.txt      # Build configuration
```

## Future Improvements

Potential areas for enhancement:
- Implement data compression
- Add support for data expiration
- Implement background compaction
- Add transaction support
- Implement concurrent access control
