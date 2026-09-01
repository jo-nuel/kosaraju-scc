#include <algorithm>
#include <cstddef>
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

  if (failures == 0) {
    std::cout << "All tests passed\n";
  }

  return failures == 0 ? 0 : 1;
}
