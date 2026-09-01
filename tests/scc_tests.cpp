#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "graph.hpp"

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

}  // namespace

int main() {
  testDirectedEdges();
  testInvalidVertex();

  if (failures == 0) {
    std::cout << "All graph tests passed\n";
  }

  return failures == 0 ? 0 : 1;
}
