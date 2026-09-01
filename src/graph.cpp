#include "graph.hpp"

#include <stdexcept>

DirectedGraph::DirectedGraph(std::size_t vertexCount)
    : adjacencyList_(vertexCount) {}

std::size_t DirectedGraph::vertexCount() const {
  return adjacencyList_.size();
}

void DirectedGraph::addEdge(std::size_t from, std::size_t to) {
  checkVertex(from);
  checkVertex(to);
  adjacencyList_[from].push_back(to);
}

const std::vector<std::size_t>& DirectedGraph::neighbours(
    std::size_t vertex) const {
  checkVertex(vertex);
  return adjacencyList_[vertex];
}

DirectedGraph DirectedGraph::transpose() const {
  DirectedGraph reversed(vertexCount());

  for (std::size_t from = 0; from < vertexCount(); ++from) {
    for (std::size_t to : neighbours(from)) {
      // An edge from A to B becomes an edge from B to A.
      reversed.addEdge(to, from);
    }
  }

  return reversed;
}

void DirectedGraph::checkVertex(std::size_t vertex) const {
  if (vertex >= vertexCount()) {
    throw std::out_of_range("vertex is outside the graph");
  }
}
