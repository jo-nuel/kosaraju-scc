#include "graph_generators.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <random>
#include <stdexcept>
#include <unordered_set>

namespace {

struct LocalEdge {
  std::size_t from;
  std::size_t to;
};

std::size_t possibleEdgeCount(std::size_t vertexCount) {
  if (vertexCount < 2) {
    return 0;
  }
  if (vertexCount >
      std::numeric_limits<std::size_t>::max() / (vertexCount - 1)) {
    throw std::overflow_error("the number of possible edges is too large");
  }
  return vertexCount * (vertexCount - 1);
}

LocalEdge decodeEdge(std::size_t encodedEdge, std::size_t vertexCount) {
  const std::size_t from = encodedEdge / (vertexCount - 1);
  const std::size_t position = encodedEdge % (vertexCount - 1);
  // Shift destinations at or after `from` to skip the self-loop.
  const std::size_t to = position < from ? position : position + 1;
  return {from, to};
}

std::size_t encodeEdge(std::size_t from, std::size_t to,
                       std::size_t vertexCount) {
  const std::size_t position = to < from ? to : to - 1;
  return from * (vertexCount - 1) + position;
}

void addRandomEdges(DirectedGraph& graph, std::size_t firstVertex,
                    std::size_t localVertexCount,
                    std::size_t targetEdgeCount,
                    std::unordered_set<std::size_t>& selectedEdges,
                    std::mt19937_64& randomEngine) {
  const std::size_t possibleEdges = possibleEdgeCount(localVertexCount);
  std::uniform_int_distribution<std::size_t> chooseEdge(0,
                                                         possibleEdges - 1);

  while (selectedEdges.size() < targetEdgeCount) {
    const std::size_t encodedEdge = chooseEdge(randomEngine);
    if (!selectedEdges.insert(encodedEdge).second) {
      continue;
    }

    const LocalEdge edge = decodeEdge(encodedEdge, localVertexCount);
    graph.addEdge(firstVertex + edge.from, firstVertex + edge.to);
  }
}

}  // namespace

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

  const std::size_t possibleEdges = possibleEdgeCount(vertexCount);
  if (edgeCount > possibleEdges) {
    throw std::invalid_argument("edge count exceeds the number of unique edges");
  }

  std::mt19937_64 randomEngine(seed);
  std::unordered_set<std::size_t> selectedEdges;
  selectedEdges.reserve(edgeCount);
  addRandomEdges(graph, 0, vertexCount, edgeCount, selectedEdges,
                 randomEngine);

  return graph;
}

DirectedGraph makeClusteredGraph(std::size_t groupCount,
                                 std::size_t groupSize,
                                 std::size_t extraInternalEdges,
                                 std::uint64_t seed) {
  if (groupCount == 0) {
    return DirectedGraph(0);
  }
  if (groupSize < 2) {
    throw std::invalid_argument("a clustered group needs at least two vertices");
  }
  if (groupCount >
      std::numeric_limits<std::size_t>::max() / groupSize) {
    throw std::overflow_error("the clustered graph is too large");
  }

  const std::size_t possibleInternalEdges = possibleEdgeCount(groupSize);
  if (extraInternalEdges > possibleInternalEdges - groupSize) {
    throw std::invalid_argument("too many extra edges for one group");
  }

  DirectedGraph graph(groupCount * groupSize);
  std::mt19937_64 randomEngine(seed);

  for (std::size_t group = 0; group < groupCount; ++group) {
    const std::size_t firstVertex = group * groupSize;
    std::unordered_set<std::size_t> selectedEdges;
    selectedEdges.reserve(groupSize + extraInternalEdges);

    // A cycle guarantees that every vertex in this group can reach the rest.
    for (std::size_t from = 0; from < groupSize; ++from) {
      const std::size_t to = (from + 1) % groupSize;
      selectedEdges.insert(encodeEdge(from, to, groupSize));
      graph.addEdge(firstVertex + from, firstVertex + to);
    }

    addRandomEdges(graph, firstVertex, groupSize,
                   groupSize + extraInternalEdges, selectedEdges,
                   randomEngine);

    if (group + 1 < groupCount) {
      // Forward-only bridges keep the groups connected but still separate.
      graph.addEdge(firstVertex + groupSize - 1, firstVertex + groupSize);
    }
  }

  return graph;
}
