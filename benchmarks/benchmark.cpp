#include "benchmark.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "scc.hpp"

namespace {

using ComponentAlgorithm = SCCResult (*)(const DirectedGraph&);

struct NamedAlgorithm {
  std::string name;
  ComponentAlgorithm run;
};

TimingResult measureOne(const NamedAlgorithm& algorithm,
                        const DirectedGraph& graph,
                        std::size_t repetition) {
  const auto start = std::chrono::steady_clock::now();
  const SCCResult result = algorithm.run(graph);
  const auto finish = std::chrono::steady_clock::now();
  const auto elapsed =
      std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start);

  return {algorithm.name, repetition, elapsed.count(), result.componentCount};
}

}  // namespace

std::vector<TimingResult> measureAlgorithms(const DirectedGraph& graph,
                                            std::size_t warmUpRuns,
                                            std::size_t timedRuns) {
  const NamedAlgorithm kosaraju = {"kosaraju", stronglyConnectedComponents};
  const NamedAlgorithm tarjan = {"tarjan", tarjanStronglyConnectedComponents};

  for (std::size_t run = 0; run < warmUpRuns; ++run) {
    const SCCResult kosarajuResult = kosaraju.run(graph);
    const SCCResult tarjanResult = tarjan.run(graph);
    if (kosarajuResult.componentCount != tarjanResult.componentCount) {
      throw std::logic_error("the algorithms disagree during warm-up");
    }
  }

  std::vector<TimingResult> results;
  results.reserve(timedRuns * 2);

  for (std::size_t repetition = 0; repetition < timedRuns; ++repetition) {
    // Alternating the order avoids always giving one algorithm the first run.
    const NamedAlgorithm& firstAlgorithm =
        repetition % 2 == 0 ? kosaraju : tarjan;
    const NamedAlgorithm& secondAlgorithm =
        repetition % 2 == 0 ? tarjan : kosaraju;
    const TimingResult first =
        measureOne(firstAlgorithm, graph, repetition);
    const TimingResult second =
        measureOne(secondAlgorithm, graph, repetition);

    if (first.componentCount != second.componentCount) {
      throw std::logic_error("the algorithms disagree during measurement");
    }
    results.push_back(first);
    results.push_back(second);
  }

  return results;
}

void writeTimingCsvHeader(std::ostream& output) {
  output << "graph_family,vertices,edges,seed,algorithm,run,nanoseconds,"
            "components\n";
}

void writeTimingCsvRows(std::ostream& output,
                        const std::string& graphFamily,
                        std::size_t vertexCount, std::size_t edgeCount,
                        std::uint64_t seed,
                        const std::vector<TimingResult>& results) {
  for (const TimingResult& result : results) {
    output << graphFamily << ',' << vertexCount << ',' << edgeCount << ','
           << seed << ',' << result.algorithm << ','
           << result.repetition + 1 << ',' << result.nanoseconds << ','
           << result.componentCount << '\n';
  }
}
