#ifndef BENCHMARK_HPP_
#define BENCHMARK_HPP_

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <string>
#include <vector>

#include "graph.hpp"

struct TimingResult {
  std::string algorithm;
  std::size_t repetition;
  std::int64_t nanoseconds;
  std::size_t componentCount;
};

inline constexpr std::size_t kWarmUpRuns = 1;
inline constexpr std::size_t kTimedRuns = 7;

std::vector<TimingResult> measureAlgorithms(
    const DirectedGraph& graph, std::size_t warmUpRuns = kWarmUpRuns,
    std::size_t timedRuns = kTimedRuns);

void writeTimingCsvHeader(std::ostream& output);
void writeTimingCsvRows(std::ostream& output, const std::string& graphFamily,
                        std::size_t vertexCount, std::size_t edgeCount,
                        std::uint64_t seed,
                        const std::vector<TimingResult>& results);

#endif  // BENCHMARK_HPP_
