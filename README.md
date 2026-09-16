# Strongly Connected Components with Kosaraju's Algorithm

**Student:** Jonathan Immanuel  
**Student ID:** 14487692

## Project overview

This project implements Kosaraju's algorithm for finding strongly connected
components in a directed graph.

A directed graph can be thought of as a set of places connected by one-way
roads. A strongly connected component is a group of places where it is
possible to travel from any place in the group to every other place in the
same group. The algorithm separates the graph into all of these groups.

## How the algorithm works

Kosaraju's algorithm looks through the graph twice:

1. It visits every vertex and records the order in which each visit finishes.
2. It reverses the direction of every edge in the graph.
3. It visits the reversed graph using the finishing order from the first pass.
4. Each group found during the second pass is one strongly connected
   component.

## Project goals

The project will include:

- A C++ implementation of Kosaraju's algorithm
- Tests for small graphs and unusual cases
- A simple way to read a graph and display its components
- Tarjan's algorithm as a comparison
- A benchmark program for measuring running time and memory use
- Generated graphs with different sizes and connection patterns

The comparison will investigate how the two algorithms behave in practice.
Both algorithms have linear theoretical running time, but they organise their
work differently. Kosaraju's algorithm makes two passes and creates a reversed
graph, while Tarjan's algorithm finds the components in one pass.

## Planned tools

- C++23 for the graph algorithms
- CMake for building the programs
- A small test program for checking correctness
- Python for running benchmarks and creating charts from the results

## Planned repository layout

```text
Assessment 1/
|-- CMakeLists.txt
|-- README.md
|-- examples/
|   `-- working-example.txt
|-- include/
|   |-- graph.hpp
|   `-- scc.hpp
|-- src/
|   |-- graph.cpp
|   |-- kosaraju.cpp
|   |-- tarjan.cpp
|   `-- main.cpp
|-- tests/
|   `-- scc_tests.cpp
|-- benchmarks/
|   |-- benchmark.cpp
|   `-- generators.cpp
|-- scripts/
|   |-- run_benchmarks.py
|   `-- plot_results.py
`-- results/
```

## Building and running

The project requires a C++23 compiler and CMake 3.20 or newer. From the main
project folder, configure and build it with:

```powershell
cmake -S . -B build
cmake --build build --config Release
```

Run the tests with:

```powershell
ctest --test-dir build -C Release
```

The program reads the graph from a text file. The first line contains the
number of vertices and edges. Each remaining line contains one directed edge.
For example:

```text
6 7
0 1
1 2
2 0
2 3
3 4
4 3
4 5
```

Vertices are numbered from 0. The example can be run on a Visual Studio build
with:

```powershell
.\build\Release\scc.exe .\examples\working-example.txt
```

With a single-configuration compiler such as GCC, the executable is normally
in the main build folder:

```powershell
.\build\scc.exe .\examples\working-example.txt
```

If no file is provided, the program reads the same format from standard input.

## Running one benchmark case

The benchmark program supports path, cycle, random, and clustered graphs. For
example, this command measures a random graph with 10,000 vertices and 30,000
edges, then saves every run as CSV:

```powershell
.\build\Release\scc_benchmark.exe random 10000 30000 14487692 > results.csv
```

The available forms are:

```text
scc_benchmark path <vertices>
scc_benchmark cycle <vertices>
scc_benchmark random <vertices> <edges> <seed>
scc_benchmark clustered <groups> <group-size> <extra-edges-per-group> <seed>
```

Graph creation happens before timing starts. Each algorithm receives one
warm-up followed by seven measured runs on the same graph. The output records
the graph family, vertex and edge counts, seed, algorithm, run number, elapsed
nanoseconds, and component count.

## Running the benchmark sweep

Start with the quick sweep to check the executable and output format:

```powershell
python .\scripts\run_benchmarks.py --quick
```

The full candidate sweep is:

```powershell
python .\scripts\run_benchmarks.py
```

By default, the script expects a Visual Studio build in
`build/Release/scc_benchmark.exe`. A different executable can be supplied with
`--executable`. Quick results are written to `results/timings-quick.csv`, while
the full sweep writes `results/timings.csv`.

The largest cases are starting candidates. Watch their running time and memory
use during the first full sweep. The script writes each completed case
immediately, so it can be stopped if the next size is no longer practical.
For an initial bounded sweep, use --max-vertices and --max-edges. Each
algorithm's peak working set is measured in a fresh Windows process and saved
to a separate memory CSV. This is total process memory, not extra allocations
made by the algorithm alone.

## Current status

The Kosaraju and Tarjan implementations are in place. The tests compare them
on all possible four-vertex directed graphs and on a path containing 100,000
vertices. The benchmark can measure one generated graph case or automate the
candidate size sweep and save all individual timing results as CSV. The next
stage is to run the size sweep, adjust impractical cases, and analyse the
results.
