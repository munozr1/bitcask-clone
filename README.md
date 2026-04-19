# Bitcask DB

A lightweight key-value store written in C++, inspired by the [Bitcask](https://riak.com/assets/bitcask-intro.pdf) storage model. Writes are append-only for speed; an in-memory hash index maps every key to its offset in the log for O(1) reads.

## Prerequisites

| Requirement | Version |
|-------------|---------|
| CMake | ≥ 3.12 |
| C++ compiler | C++17 — GCC ≥ 7, Clang ≥ 5, or Apple Clang (Xcode Command Line Tools) |

**macOS** — install the Xcode Command Line Tools and CMake:
```bash
xcode-select --install
brew install cmake   # or download from cmake.org
```

**Linux (Debian/Ubuntu)**:
```bash
sudo apt update && sudo apt install build-essential cmake
```

**Linux (Fedora/RHEL)**:
```bash
sudo dnf install gcc-c++ cmake
```

## Build

```bash
git clone https://github.com/munozr1/bitcask-clone.git
cd bitcask-clone
mkdir build && cd build
cmake ..
cmake --build .
```

This produces two binaries inside `build/`:

| Binary | Description |
|--------|-------------|
| `bitcask` | Interactive CLI |
| `l1test` | L1-cache benchmark |

## Interactive CLI

```
$ ./bitcask
Bitcask DB — type: get / set / print / exit

Enter command (get/set/print/exit): set
Enter key: name
Enter value: Alice
Key-value pair set.

Enter command (get/set/print/exit): set
Enter key: lang
Enter value: C++
Key-value pair set.

Enter command (get/set/print/exit): get
Enter key: name
Value: Alice

Enter command (get/set/print/exit): print
Current Cache State:
Key: name, Offset: 0
Key: lang, Offset: 11

Enter command (get/set/print/exit): exit
```

Data is written to `kv.db` in the current directory. The index is rebuilt from the file on the next launch, so previously stored keys are always available.

## Benchmark

```bash
./l1test
```

Inserts 10,000 key-value pairs, selects 50 "hot" keys at random, then runs 5,000 mixed get/set operations against those keys while measuring wall-clock time.

## Architecture

```
┌─────────────────────────────────────────────┐
│                 Application                 │
│           set(key, val) / get(key)          │
└───────────────────┬─────────────────────────┘
                    │
         ┌──────────▼──────────┐
         │   In-Memory Index   │
         │  unordered_map      │
         │  key → file offset  │
         └──────────┬──────────┘
                    │ seek to offset
         ┌──────────▼──────────┐
         │   Append-Only Log   │
         │      kv.db          │
         │  key,value\n  ...   │
         └─────────────────────┘
```

- **Writes**: record appended to the log; index updated with the new offset.
- **Reads**: offset looked up in the index; single seek + forward read from that position.
- **Startup**: the log is scanned once to rebuild the index; duplicate keys resolve to the latest entry.

## Project Structure

```
.
├── include/
│   └── db.hpp          # BitcaskDB class interface
├── src/
│   ├── db.cpp          # Storage engine implementation
│   └── main.cpp        # Interactive CLI
├── test/
│   └── l1cache_benchmark.cpp
└── CMakeLists.txt
```

## Limitations

- Single-process only — no concurrent access.
- No compaction: deleted or overwritten keys leave dead records in the log file.
- Keys and values must not contain commas (`,`) — the storage format uses comma as a delimiter.
