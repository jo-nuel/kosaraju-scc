#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "benchmark.hpp"
#include "graph.hpp"
#include "graph_generators.hpp"

namespace {

struct BenchmarkCase {
  std::string family;
  std::uint64_t seed;
  DirectedGraph graph;
};

void showUsage(std::ostream& output) {
  output << "Usage:\n"
         << "  scc_benchmark path <vertices>\n"
         << "  scc_benchmark cycle <vertices>\n"
         << "  scc_benchmark random <vertices> <edges> <seed>\n"
         << "  scc_benchmark clustered <groups> <group-size> "
            "<extra-edges-per-group> <seed>\n";
}

std::uint64_t parseUnsigned(const char* text, const std::string& name) {
  const std::string value(text);
  if (value.empty() || value.front() == '-') {
    throw std::invalid_argument(name + " must be a non-negative integer");
  }

  std::size_t charactersUsed = 0;
  const unsigned long long parsed = std::stoull(value, &charactersUsed);
  if (charactersUsed != value.size()) {
    throw std::invalid_argument(name + " must be a non-negative integer");
  }
  return static_cast<std::uint64_t>(parsed);
}

std::size_t parseSize(const char* text, const std::string& name) {
  const std::uint64_t parsed = parseUnsigned(text, name);
  if (parsed > std::numeric_limits<std::size_t>::max()) {
    throw std::out_of_range(name + " is too large for this computer");
  }
  return static_cast<std::size_t>(parsed);
}

BenchmarkCase readCase(int argumentCount, char* arguments[]) {
  if (argumentCount < 2) {
    throw std::invalid_argument("a graph family is required");
  }

  const std::string family(arguments[1]);
  if (family == "path" || family == "cycle") {
    if (argumentCount != 3) {
      throw std::invalid_argument(family + " requires a vertex count");
    }
    const std::size_t vertexCount = parseSize(arguments[2], "vertex count");
    DirectedGraph graph = family == "path"
                              ? makeDirectedPath(vertexCount)
                              : makeDirectedCycle(vertexCount);
    return {family, 0, std::move(graph)};
  }

  if (family == "random") {
    if (argumentCount != 5) {
      throw std::invalid_argument(
          "random requires vertex count, edge count, and seed");
    }
    const std::size_t vertexCount = parseSize(arguments[2], "vertex count");
    const std::size_t edgeCount = parseSize(arguments[3], "edge count");
    const std::uint64_t seed = parseUnsigned(arguments[4], "seed");
    return {family, seed, makeRandomGraph(vertexCount, edgeCount, seed)};
  }

  if (family == "clustered") {
    if (argumentCount != 6) {
      throw std::invalid_argument(
          "clustered requires group count, group size, extra edges, and seed");
    }
    const std::size_t groupCount = parseSize(arguments[2], "group count");
    const std::size_t groupSize = parseSize(arguments[3], "group size");
    const std::size_t extraEdges = parseSize(arguments[4], "extra edges");
    const std::uint64_t seed = parseUnsigned(arguments[5], "seed");
    return {family, seed,
            makeClusteredGraph(groupCount, groupSize, extraEdges, seed)};
  }

  throw std::invalid_argument("unknown graph family: " + family);
}

std::size_t countEdges(const DirectedGraph& graph) {
  std::size_t edgeCount = 0;
  for (std::size_t vertex = 0; vertex < graph.vertexCount(); ++vertex) {
    edgeCount += graph.neighbours(vertex).size();
  }
  return edgeCount;
}

}  // namespace

int main(int argumentCount, char* arguments[]) {
  if (argumentCount == 2 && std::string(arguments[1]) == "--help") {
    showUsage(std::cout);
    return 0;
  }

  try {
    const BenchmarkCase benchmarkCase = readCase(argumentCount, arguments);
    const std::size_t edgeCount = countEdges(benchmarkCase.graph);
    const std::vector<TimingResult> results =
        measureAlgorithms(benchmarkCase.graph);

    writeTimingCsvHeader(std::cout);
    writeTimingCsvRows(std::cout, benchmarkCase.family,
                       benchmarkCase.graph.vertexCount(), edgeCount,
                       benchmarkCase.seed, results);
  } catch (const std::exception& error) {
    std::cerr << "Error: " << error.what() << "\n\n";
    showUsage(std::cerr);
    return 1;
  }

  return 0;
}
