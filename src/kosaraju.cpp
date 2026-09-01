#include "scc.hpp"

#include <cstddef>
#include <vector>

namespace {

void finishVisit(const DirectedGraph& graph, std::size_t vertex,
                 std::vector<bool>& visited,
                 std::vector<std::size_t>& order) {
  visited[vertex] = true;

  for (std::size_t neighbour : graph.neighbours(vertex)) {
    if (!visited[neighbour]) {
      finishVisit(graph, neighbour, visited, order);
    }
  }

  // Record the vertex after every reachable neighbour has been checked.
  order.push_back(vertex);
}

}  // namespace

std::vector<std::size_t> finishingOrder(const DirectedGraph& graph) {
  std::vector<bool> visited(graph.vertexCount(), false);
  std::vector<std::size_t> order;
  order.reserve(graph.vertexCount());

  for (std::size_t vertex = 0; vertex < graph.vertexCount(); ++vertex) {
    if (!visited[vertex]) {
      finishVisit(graph, vertex, visited, order);
    }
  }

  return order;
}
