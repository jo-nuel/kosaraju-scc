#ifndef SCC_HPP_
#define SCC_HPP_

#include <cstddef>
#include <vector>

#include "graph.hpp"

std::vector<std::size_t> finishingOrder(const DirectedGraph& graph);

#endif  // SCC_HPP_
