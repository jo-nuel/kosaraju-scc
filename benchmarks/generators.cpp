#include "graph_generators.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <random>
#include <stdexcept>
#include <unordered_set>

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

DirectedGraph makeRandomGraph(std::size_t vertexCount, std::size_t edgeCount,
                              std::uint64_t seed) {
  DirectedGraph graph(vertexCount);
  if (vertexCount < 2) {
    if (edgeCount != 0) {
      throw std::invalid_argument(
          "a graph with fewer than two vertices cannot contain these edges");
    }
    return graph;
  }

  if (vertexCount >
      std::numeric_limits<std::size_t>::max() / (vertexCount - 1)) {
    throw std::overflow_error("the number of possible edges is too large");
  }

  const std::size_t possibleEdges = vertexCount * (vertexCount - 1);
  if (edgeCount > possibleEdges) {
    throw std::invalid_argument("edge count exceeds the number of unique edges");
  }

  std::mt19937_64 randomEngine(seed);
  std::uniform_int_distribution<std::size_t> chooseEdge(0,
                                                         possibleEdges - 1);
  std::unordered_set<std::size_t> selectedEdges;
  selectedEdges.reserve(edgeCount);

  while (selectedEdges.size() < edgeCount) {
    const std::size_t encodedEdge = chooseEdge(randomEngine);
    if (!selectedEdges.insert(encodedEdge).second) {
      continue;
    }

    const std::size_t from = encodedEdge / (vertexCount - 1);
    const std::size_t position = encodedEdge % (vertexCount - 1);
    // Shift destinations at or after `from` to skip the self-loop.
    const std::size_t to = position < from ? position : position + 1;
    graph.addEdge(from, to);
  }

  return graph;
}
