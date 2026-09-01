#include "scc.hpp"

#include <cstddef>
#include <utility>
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

void assignComponent(const DirectedGraph& graph, std::size_t vertex,
                     std::size_t component,
                     std::vector<std::size_t>& componentOf) {
  // Every vertex reached in this search belongs to the current group.
  componentOf[vertex] = component;

  for (std::size_t neighbour : graph.neighbours(vertex)) {
    if (componentOf[neighbour] == graph.vertexCount()) {
      assignComponent(graph, neighbour, component, componentOf);
    }
  }
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

SCCResult stronglyConnectedComponents(const DirectedGraph& graph) {
  const std::vector<std::size_t> order = finishingOrder(graph);
  const DirectedGraph reversed = graph.transpose();

  // The number of vertices cannot be a real component number.
  std::vector<std::size_t> componentOf(graph.vertexCount(),
                                       graph.vertexCount());
  std::size_t componentCount = 0;

  // The vertex that finished last in the first pass must be checked first.
  for (auto position = order.rbegin(); position != order.rend(); ++position) {
    if (componentOf[*position] == graph.vertexCount()) {
      assignComponent(reversed, *position, componentCount, componentOf);
      ++componentCount;
    }
  }

  return {std::move(componentOf), componentCount};
}
