# C++ Cache Simulator

A C++ project that simulates different CPU cache configurations and compares their performance using memory access traces. The simulator tracks cache hits and evaluates cache organization, replacement policies, write-miss behavior, and prefetching strategies.

## Features

- **Direct-Mapped Cache:** Simulates caches with different capacities and one cache line per set.
- **Set-Associative Cache:** Models set-associative caches with 2, 4, 8, and 16 lines per set.
- **Fully Associative Cache:** Simulates a cache in which any memory block can occupy any cache line.
- **LRU Replacement:** Uses Least Recently Used (LRU) tracking to select cache lines for replacement.
- **Write-Miss Policies:** Compares cache behavior under different write-miss handling strategies.
- **Prefetching:** Simulates fetching the next sequential cache block following a memory access.
- **Hit Tracking:** Counts cache hits for each configuration and records the total number of memory accesses.
- **File-Based Processing:** Reads memory traces from an input file and writes simulation results to an output file.

### Compile

From the directory containing `cache_simulator.cpp`, run:

```bash
g++ cache_simulator.cpp -o cache_simulator
```

### Run

Pass the input trace file and output filename as command-line arguments.

**Windows PowerShell:**

```powershell
.\cache_simulator input.txt output.txt
```

**Linux/macOS:**

```bash
./cache_simulator input.txt output.txt
```

## Input Format

Each line contains a memory access type followed by a hexadecimal memory address.

Example:

```text
L 00400000
S 00400020
L 00400040
L 00400000
```

- `L` represents a load.
- `S` represents a store.
- Addresses are parsed in hexadecimal format.

## Cache Configurations

The simulator includes implementations for several cache designs:

| Configuration | Description |
|---|---|
| Direct-mapped | Each memory block maps to one cache line |
| Set-associative | Each memory block maps to a set containing multiple lines |
| Fully associative | A memory block can occupy any cache line |
| LRU replacement | Replaces the least recently used line |
| Write-miss variations | Evaluates different behaviors when a store misses |
| Sequential prefetching | Fetches the next cache block based on the current address |
| Hot-cold replacement | Uses hot-cold state bits to choose a replacement candidate |

## Output

The output file contains cache-hit counts and total memory accesses for each implemented configuration.

Results are written as semicolon-separated groups, with each configuration recording its hit count and total access count. These values can be used to calculate cache hit rates:

\[
\text{Hit Rate} = \frac{\text{Cache Hits}}{\text{Total Memory Accesses}} \times 100\%
\]

## Purpose

This project explores how cache organization and memory access behavior affect cache performance, using a C++ simulation to compare different cache designs and memory-management strategies.
