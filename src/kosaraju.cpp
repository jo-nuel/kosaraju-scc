#include "scc.hpp"

#include <cstddef>
#include <utility>
#include <vector>

namespace {

struct VisitFrame {
  std::size_t vertex;
  std::size_t nextNeighbour;
};

void finishFrom(const DirectedGraph& graph, std::size_t start,
                std::vector<bool>& visited,
                std::vector<std::size_t>& order) {
  std::vector<VisitFrame> pending = {{start, 0}};
  visited[start] = true;

  while (!pending.empty()) {
    VisitFrame& current = pending.back();
    const std::vector<std::size_t>& neighbours =
        graph.neighbours(current.vertex);

    if (current.nextNeighbour < neighbours.size()) {
      const std::size_t neighbour = neighbours[current.nextNeighbour];
      ++current.nextNeighbour;

      if (!visited[neighbour]) {
        visited[neighbour] = true;
        pending.push_back({neighbour, 0});
      }
    } else {
      // Keeping the next neighbour in each frame preserves finishing order.
      order.push_back(current.vertex);
      pending.pop_back();
    }
  }
}

void assignComponent(const DirectedGraph& graph, std::size_t start,
                     std::size_t component,
                     std::vector<std::size_t>& componentOf) {
  std::vector<std::size_t> pending = {start};
  componentOf[start] = component;

  while (!pending.empty()) {
    const std::size_t vertex = pending.back();
    pending.pop_back();

    for (std::size_t neighbour : graph.neighbours(vertex)) {
      if (componentOf[neighbour] == graph.vertexCount()) {
        // Mark a vertex when it is added so it cannot be added twice.
        componentOf[neighbour] = component;
        pending.push_back(neighbour);
      }
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
      finishFrom(graph, vertex, visited, order);
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
