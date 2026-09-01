#include <cstddef>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "graph.hpp"
#include "scc.hpp"

int main(int argumentCount, char* arguments[]) {
  if (argumentCount > 2) {
    std::cerr << "Usage: scc [graph-file]\n";
    return 1;
  }

  std::ifstream graphFile;
  std::istream* input = &std::cin;

  if (argumentCount == 2) {
    graphFile.open(arguments[1]);
    if (!graphFile) {
      std::cerr << "Could not open " << arguments[1] << '\n';
      return 1;
    }
    input = &graphFile;
  }

  std::size_t vertexCount;
  std::size_t edgeCount;
  if (!(*input >> vertexCount >> edgeCount)) {
    std::cerr << "The first line must contain the vertex and edge counts.\n";
    return 1;
  }

  DirectedGraph graph(vertexCount);

  for (std::size_t edge = 0; edge < edgeCount; ++edge) {
    std::size_t from;
    std::size_t to;
    if (!(*input >> from >> to)) {
      std::cerr << "The graph ended before all edges were read.\n";
      return 1;
    }

    try {
      graph.addEdge(from, to);
    } catch (const std::out_of_range&) {
      std::cerr << "Edge " << from << " -> " << to
                << " refers to a missing vertex.\n";
      return 1;
    }
  }

  const SCCResult result = stronglyConnectedComponents(graph);
  std::vector<std::vector<std::size_t> > components(result.componentCount);

  // Group the vertices so the output is easier to read than component numbers.
  for (std::size_t vertex = 0; vertex < vertexCount; ++vertex) {
    components[result.componentOf[vertex]].push_back(vertex);
  }

  std::cout << "Strongly connected components: " << result.componentCount
            << '\n';
  for (std::size_t component = 0; component < components.size(); ++component) {
    std::cout << "Component " << component << ':';
    for (std::size_t vertex : components[component]) {
      std::cout << ' ' << vertex;
    }
    std::cout << '\n';
  }

  return 0;
}
