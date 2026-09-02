#ifndef GRAPH_GENERATORS_HPP_
#define GRAPH_GENERATORS_HPP_

#include <cstddef>

#include "graph.hpp"

DirectedGraph makeDirectedPath(std::size_t vertexCount);
DirectedGraph makeDirectedCycle(std::size_t vertexCount);

#endif  // GRAPH_GENERATORS_HPP_
