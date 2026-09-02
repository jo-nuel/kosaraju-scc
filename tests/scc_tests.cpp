#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "graph.hpp"
#include "scc.hpp"

namespace {

int failures = 0;

void check(bool condition, const char* message) {
  if (!condition) {
    std::cerr << "FAILED: " << message << '\n';
    ++failures;
  }
}

bool inSameComponent(const SCCResult& result, std::size_t first,
                     std::size_t second) {
  return result.componentOf[first] == result.componentOf[second];
}

bool samePartition(const SCCResult& first, const SCCResult& second) {
  if (first.componentOf.size() != second.componentOf.size() ||
      first.componentCount != second.componentCount) {
    return false;
  }

  for (std::size_t left = 0; left < first.componentOf.size(); ++left) {
    for (std::size_t right = 0; right < first.componentOf.size(); ++right) {
      if (inSameComponent(first, left, right) !=
          inSameComponent(second, left, right)) {
        return false;
      }
    }
  }

  return true;
}

std::vector<bool> reachableFrom(const DirectedGraph& graph,
                                std::size_t start) {
  std::vector<bool> reached(graph.vertexCount(), false);
  std::vector<std::size_t> pending = {start};
  reached[start] = true;

  while (!pending.empty()) {
    const std::size_t vertex = pending.back();
    pending.pop_back();

    for (std::size_t neighbour : graph.neighbours(vertex)) {
      if (!reached[neighbour]) {
        reached[neighbour] = true;
        pending.push_back(neighbour);
      }
    }
  }

  return reached;
}

bool partitionMatchesReachability(const DirectedGraph& graph,
                                  const SCCResult& result) {
  std::vector<std::vector<bool> > reachable;
  reachable.reserve(graph.vertexCount());

  for (std::size_t vertex = 0; vertex < graph.vertexCount(); ++vertex) {
    reachable.push_back(reachableFrom(graph, vertex));
  }

  for (std::size_t first = 0; first < graph.vertexCount(); ++first) {
    for (std::size_t second = 0; second < graph.vertexCount(); ++second) {
      // Two vertices share a component exactly when both can reach each other.
      const bool mutuallyReachable =
          reachable[first][second] && reachable[second][first];
      if (inSameComponent(result, first, second) != mutuallyReachable) {
        return false;
      }
    }
  }

  return true;
}

void testDirectedEdges() {
  DirectedGraph graph(4);

  check(graph.vertexCount() == 4, "the graph keeps every vertex");
  check(graph.neighbours(0).empty(), "a new vertex has no outgoing edges");

  graph.addEdge(0, 1);
  graph.addEdge(0, 2);
  graph.addEdge(2, 3);

  const std::vector<std::size_t> expectedFromZero = {1, 2};
  const std::vector<std::size_t> expectedFromTwo = {3};

  check(graph.neighbours(0) == expectedFromZero,
        "edges are stored under their starting vertex");
  check(graph.neighbours(1).empty(),
        "an incoming edge is not stored as an outgoing edge");
  check(graph.neighbours(2) == expectedFromTwo,
        "edge direction is preserved");
}

void testInvalidVertex() {
  DirectedGraph graph(2);
  bool threwException = false;

  try {
    graph.addEdge(0, 2);
  } catch (const std::out_of_range&) {
    threwException = true;
  }

  check(threwException, "an edge cannot refer to a missing vertex");
}

void testTransposedGraph() {
  DirectedGraph graph(5);
  graph.addEdge(0, 1);
  graph.addEdge(2, 1);
  graph.addEdge(2, 1);
  graph.addEdge(2, 3);
  graph.addEdge(3, 3);

  const DirectedGraph reversed = graph.transpose();
  const std::vector<std::size_t> expectedFromOne = {0, 2, 2};
  const std::vector<std::size_t> expectedFromThree = {2, 3};

  check(reversed.vertexCount() == graph.vertexCount(),
        "transposing keeps every vertex");
  check(reversed.neighbours(0).empty(),
        "a reversed edge is not left at its old starting vertex");
  check(reversed.neighbours(1) == expectedFromOne,
        "normal and repeated edges are reversed");
  check(reversed.neighbours(3) == expectedFromThree,
        "a self-loop still points to the same vertex");
  check(reversed.neighbours(4).empty(),
        "an isolated vertex remains in the graph");
}

void testFinishingOrderOnPath() {
  DirectedGraph graph(4);
  graph.addEdge(0, 1);
  graph.addEdge(1, 2);
  graph.addEdge(2, 3);

  const std::vector<std::size_t> expected = {3, 2, 1, 0};
  check(finishingOrder(graph) == expected,
        "a path finishes from its last vertex back to its first");
}

void testFinishingOrderOnCycle() {
  DirectedGraph graph(3);
  graph.addEdge(0, 1);
  graph.addEdge(1, 2);
  graph.addEdge(2, 0);

  const std::vector<std::size_t> expected = {2, 1, 0};
  check(finishingOrder(graph) == expected,
        "a cycle records every vertex once");
}

void testFinishingOrderOnDisconnectedGraph() {
  DirectedGraph graph(4);
  graph.addEdge(0, 1);
  graph.addEdge(2, 3);

  const std::vector<std::size_t> expected = {1, 0, 3, 2};
  check(finishingOrder(graph) == expected,
        "the search restarts for disconnected vertices");
}

void testFinishingOrderContainsEveryVertex() {
  DirectedGraph graph(6);
  graph.addEdge(0, 1);
  graph.addEdge(1, 0);
  graph.addEdge(2, 3);
  graph.addEdge(3, 4);

  std::vector<std::size_t> order = finishingOrder(graph);
  std::sort(order.begin(), order.end());

  const std::vector<std::size_t> expected = {0, 1, 2, 3, 4, 5};
  check(order == expected,
        "the finishing order contains every vertex exactly once");
}

void testFinishingOrderOnEmptyGraph() {
  DirectedGraph graph(0);
  check(finishingOrder(graph).empty(),
        "an empty graph has an empty finishing order");
}

void testComponentsFromWorkingExample() {
  DirectedGraph graph(6);
  graph.addEdge(0, 1);
  graph.addEdge(1, 2);
  graph.addEdge(2, 0);
  graph.addEdge(2, 3);
  graph.addEdge(3, 4);
  graph.addEdge(4, 3);
  graph.addEdge(4, 5);

  const SCCResult result = stronglyConnectedComponents(graph);

  check(result.componentCount == 3,
        "the working example contains three components");
  check(result.componentOf.size() == graph.vertexCount(),
        "every vertex receives a component number");
  check(inSameComponent(result, 0, 1) && inSameComponent(result, 1, 2),
        "vertices 0, 1, and 2 are grouped together");
  check(inSameComponent(result, 3, 4),
        "vertices 3 and 4 are grouped together");
  check(!inSameComponent(result, 2, 3),
        "a one-way edge does not join two components");
  check(!inSameComponent(result, 4, 5),
        "vertex 5 remains in its own component");
}

void testComponentsInCycle() {
  DirectedGraph graph(4);
  graph.addEdge(0, 1);
  graph.addEdge(1, 2);
  graph.addEdge(2, 3);
  graph.addEdge(3, 0);

  const SCCResult result = stronglyConnectedComponents(graph);
  check(result.componentCount == 1, "one cycle forms one component");
  check(inSameComponent(result, 0, 3),
        "the first and last cycle vertices are grouped together");
}

void testComponentsInAcyclicGraph() {
  DirectedGraph graph(4);
  graph.addEdge(0, 1);
  graph.addEdge(1, 2);
  graph.addEdge(2, 3);

  const SCCResult result = stronglyConnectedComponents(graph);
  check(result.componentCount == 4,
        "each vertex in a directed path is its own component");
  check(!inSameComponent(result, 0, 1),
        "a one-way path does not make vertices mutually reachable");
}

void testComponentsInEmptyGraph() {
  const SCCResult result = stronglyConnectedComponents(DirectedGraph(0));
  check(result.componentCount == 0, "an empty graph has no components");
  check(result.componentOf.empty(),
        "an empty graph has no component assignments");
}

void testSelfLoopsRepeatedEdgesAndIsolatedVertices() {
  DirectedGraph graph(4);
  graph.addEdge(0, 0);
  graph.addEdge(1, 2);
  graph.addEdge(1, 2);
  graph.addEdge(2, 1);
  graph.addEdge(2, 1);

  const SCCResult result = stronglyConnectedComponents(graph);

  check(result.componentCount == 3,
        "self-loops and repeated edges do not create extra components");
  check(inSameComponent(result, 1, 2),
        "repeated edges preserve the component containing 1 and 2");
  check(!inSameComponent(result, 0, 1),
        "a self-loop does not connect its vertex to another component");
  check(!inSameComponent(result, 0, 3),
        "an isolated vertex remains separate");
}

void testEveryFourVertexGraph() {
  constexpr std::size_t vertexCount = 4;
  constexpr std::size_t possibleEdges = vertexCount * vertexCount;
  const std::uint32_t graphCount = std::uint32_t{1} << possibleEdges;
  bool kosarajuMatched = true;
  bool tarjanMatched = true;
  bool algorithmsAgreed = true;

  // Each bit describes whether one possible directed edge is present.
  for (std::uint32_t mask = 0;
       mask < graphCount && kosarajuMatched && tarjanMatched &&
       algorithmsAgreed;
       ++mask) {
    DirectedGraph graph(vertexCount);

    for (std::size_t from = 0; from < vertexCount; ++from) {
      for (std::size_t to = 0; to < vertexCount; ++to) {
        const std::size_t bit = from * vertexCount + to;
        if ((mask & (std::uint32_t{1} << bit)) != 0) {
          graph.addEdge(from, to);
        }
      }
    }

    const SCCResult kosaraju = stronglyConnectedComponents(graph);
    const SCCResult tarjan = tarjanStronglyConnectedComponents(graph);
    kosarajuMatched = partitionMatchesReachability(graph, kosaraju);
    tarjanMatched = partitionMatchesReachability(graph, tarjan);
    algorithmsAgreed = samePartition(kosaraju, tarjan);
  }

  check(kosarajuMatched,
        "Kosaraju matches reachability on every four-vertex graph");
  check(tarjanMatched,
        "Tarjan matches reachability on every four-vertex graph");
  check(algorithmsAgreed,
        "both algorithms agree on every four-vertex graph");
}

void testLongPath() {
  constexpr std::size_t vertexCount = 100000;
  DirectedGraph graph(vertexCount);

  for (std::size_t vertex = 1; vertex < vertexCount; ++vertex) {
    graph.addEdge(vertex - 1, vertex);
  }

  const SCCResult result = stronglyConnectedComponents(graph);
  check(result.componentCount == vertexCount,
        "every vertex in a long one-way path remains separate");
  check(result.componentOf.size() == vertexCount,
        "the long path assigns every vertex");
}

void testTarjanOnWorkingExample() {
  DirectedGraph graph(6);
  graph.addEdge(0, 1);
  graph.addEdge(1, 2);
  graph.addEdge(2, 0);
  graph.addEdge(2, 3);
  graph.addEdge(3, 4);
  graph.addEdge(4, 3);
  graph.addEdge(4, 5);

  const SCCResult kosaraju = stronglyConnectedComponents(graph);
  const SCCResult tarjan = tarjanStronglyConnectedComponents(graph);

  check(tarjan.componentCount == 3,
        "Tarjan finds three components in the working example");
  check(samePartition(kosaraju, tarjan),
        "both algorithms group the working example in the same way");
}

void testTarjanOnSmallUnusualGraphs() {
  DirectedGraph unusual(4);
  unusual.addEdge(0, 0);
  unusual.addEdge(1, 2);
  unusual.addEdge(1, 2);
  unusual.addEdge(2, 1);

  const SCCResult unusualKosaraju = stronglyConnectedComponents(unusual);
  const SCCResult unusualTarjan = tarjanStronglyConnectedComponents(unusual);
  check(samePartition(unusualKosaraju, unusualTarjan),
        "Tarjan handles self-loops, repeated edges, and isolated vertices");

  const SCCResult empty =
      tarjanStronglyConnectedComponents(DirectedGraph(0));
  check(empty.componentCount == 0 && empty.componentOf.empty(),
        "Tarjan handles an empty graph");
}

void testTarjanOnLongPath() {
  constexpr std::size_t vertexCount = 100000;
  DirectedGraph graph(vertexCount);

  for (std::size_t vertex = 1; vertex < vertexCount; ++vertex) {
    graph.addEdge(vertex - 1, vertex);
  }

  const SCCResult result = tarjanStronglyConnectedComponents(graph);
  check(result.componentCount == vertexCount,
        "Tarjan keeps every long-path vertex in a separate component");
  check(result.componentOf.size() == vertexCount,
        "Tarjan assigns every vertex in the long path");
}

}  // namespace

int main() {
  testDirectedEdges();
  testInvalidVertex();
  testTransposedGraph();
  testFinishingOrderOnPath();
  testFinishingOrderOnCycle();
  testFinishingOrderOnDisconnectedGraph();
  testFinishingOrderContainsEveryVertex();
  testFinishingOrderOnEmptyGraph();
  testComponentsFromWorkingExample();
  testComponentsInCycle();
  testComponentsInAcyclicGraph();
  testComponentsInEmptyGraph();
  testSelfLoopsRepeatedEdgesAndIsolatedVertices();
  testEveryFourVertexGraph();
  testLongPath();
  testTarjanOnWorkingExample();
  testTarjanOnSmallUnusualGraphs();
  testTarjanOnLongPath();

  if (failures == 0) {
    std::cout << "All tests passed\n";
  }

  return failures == 0 ? 0 : 1;
}
