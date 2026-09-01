#ifndef SCC_HPP_
#define SCC_HPP_

#include <cstddef>
#include <vector>

#include "graph.hpp"

struct SCCResult {
  std::vector<std::size_t> componentOf;
  std::size_t componentCount;
};

std::vector<std::size_t> finishingOrder(const DirectedGraph& graph);
SCCResult stronglyConnectedComponents(const DirectedGraph& graph);
SCCResult tarjanStronglyConnectedComponents(const DirectedGraph& graph);

#endif  // SCC_HPP_
