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

Build and run instructions will be added after the first working version of
the program is created.

## Current status

The repository has been created and the project scope has been defined. The
next step is to add the graph representation and the first correctness tests.
