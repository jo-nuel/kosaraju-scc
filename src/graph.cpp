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

void DirectedGraph::checkVertex(std::size_t vertex) const {
  if (vertex >= vertexCount()) {
    throw std::out_of_range("vertex is outside the graph");
  }
}
