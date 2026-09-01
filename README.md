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

## Current status

The graph representation and Kosaraju implementation are complete. The tests
cover all possible four-vertex directed graphs and a path containing 100,000
vertices. The next stage is to implement Tarjan's algorithm and prepare the
benchmark comparison.
