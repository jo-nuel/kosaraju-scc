#ifndef GRAPH_GENERATORS_HPP_
#define GRAPH_GENERATORS_HPP_

#include <cstddef>
#include <cstdint>

#include "graph.hpp"

DirectedGraph makeDirectedPath(std::size_t vertexCount);
DirectedGraph makeDirectedCycle(std::size_t vertexCount);
DirectedGraph makeRandomGraph(std::size_t vertexCount, std::size_t edgeCount,
                              std::uint64_t seed);

#endif  // GRAPH_GENERATORS_HPP_
