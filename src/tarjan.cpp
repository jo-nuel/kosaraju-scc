#include "scc.hpp"

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

namespace {

class TarjanSearch {
 public:
  explicit TarjanSearch(const DirectedGraph& graph)
      : graph_(graph),
        unvisited_(graph.vertexCount()),
        index_(graph.vertexCount(), unvisited_),
        lowLink_(graph.vertexCount(), unvisited_),
        onStack_(graph.vertexCount(), false),
        componentOf_(graph.vertexCount(), unvisited_) {}

  SCCResult run() {
    for (std::size_t vertex = 0; vertex < graph_.vertexCount(); ++vertex) {
      if (index_[vertex] == unvisited_) {
        visit(vertex);
      }
    }

    return {std::move(componentOf_), componentCount_};
  }

 private:
  void visit(std::size_t vertex) {
    index_[vertex] = nextIndex_;
    lowLink_[vertex] = nextIndex_;
    ++nextIndex_;

    active_.push_back(vertex);
    onStack_[vertex] = true;

    for (std::size_t neighbour : graph_.neighbours(vertex)) {
      if (index_[neighbour] == unvisited_) {
        visit(neighbour);
        lowLink_[vertex] =
            std::min(lowLink_[vertex], lowLink_[neighbour]);
      } else if (onStack_[neighbour]) {
        // This edge returns to a vertex in the unfinished search path.
        lowLink_[vertex] = std::min(lowLink_[vertex], index_[neighbour]);
      }
    }

    if (lowLink_[vertex] != index_[vertex]) {
      return;
    }

    // A root closes one complete component at the top of the active stack.
    while (true) {
      const std::size_t member = active_.back();
      active_.pop_back();
      onStack_[member] = false;
      componentOf_[member] = componentCount_;

      if (member == vertex) {
        break;
      }
    }
    ++componentCount_;
  }

  const DirectedGraph& graph_;
  const std::size_t unvisited_;
  std::vector<std::size_t> index_;
  std::vector<std::size_t> lowLink_;
  std::vector<bool> onStack_;
  std::vector<std::size_t> active_;
  std::vector<std::size_t> componentOf_;
  std::size_t nextIndex_ = 0;
  std::size_t componentCount_ = 0;
};

}  // namespace

SCCResult tarjanStronglyConnectedComponents(const DirectedGraph& graph) {
  return TarjanSearch(graph).run();
}
