#include "graph_generators.hpp"

#include <cstddef>

DirectedGraph makeDirectedPath(std::size_t vertexCount) {
  DirectedGraph graph(vertexCount);

  for (std::size_t vertex = 1; vertex < vertexCount; ++vertex) {
    graph.addEdge(vertex - 1, vertex);
  }

  return graph;
}

DirectedGraph makeDirectedCycle(std::size_t vertexCount) {
  DirectedGraph graph(vertexCount);
  if (vertexCount == 0) {
    return graph;
  }

  for (std::size_t vertex = 0; vertex < vertexCount; ++vertex) {
    graph.addEdge(vertex, (vertex + 1) % vertexCount);
  }

  return graph;
}
