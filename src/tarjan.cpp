#include "scc.hpp"

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

namespace {

struct SearchFrame {
  std::size_t vertex;
  std::size_t nextNeighbour;
};

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
        visitFrom(vertex);
      }
    }

    return {std::move(componentOf_), componentCount_};
  }

 private:
  void beginVisit(std::size_t vertex) {
    index_[vertex] = nextIndex_;
    lowLink_[vertex] = nextIndex_;
    ++nextIndex_;

    active_.push_back(vertex);
    onStack_[vertex] = true;
  }

  void closeComponent(std::size_t root) {
    // A root closes one complete component at the top of the active stack.
    while (true) {
      const std::size_t member = active_.back();
      active_.pop_back();
      onStack_[member] = false;
      componentOf_[member] = componentCount_;

      if (member == root) {
        break;
      }
    }
    ++componentCount_;
  }

  void visitFrom(std::size_t start) {
    std::vector<SearchFrame> pending;
    pending.push_back({start, 0});
    beginVisit(start);

    while (!pending.empty()) {
      SearchFrame& current = pending.back();
      const std::vector<std::size_t>& neighbours =
          graph_.neighbours(current.vertex);

      if (current.nextNeighbour < neighbours.size()) {
        const std::size_t neighbour = neighbours[current.nextNeighbour];
        ++current.nextNeighbour;

        if (index_[neighbour] == unvisited_) {
          beginVisit(neighbour);
          pending.push_back({neighbour, 0});
        } else if (onStack_[neighbour]) {
          // Completed vertices cannot lead back into the current component.
          lowLink_[current.vertex] =
              std::min(lowLink_[current.vertex], index_[neighbour]);
        }
        continue;
      }

      const std::size_t vertex = current.vertex;
      if (lowLink_[vertex] == index_[vertex]) {
        closeComponent(vertex);
      }

      pending.pop_back();
      if (!pending.empty()) {
        // This is the update that a recursive call makes when it returns.
        const std::size_t parent = pending.back().vertex;
        lowLink_[parent] = std::min(lowLink_[parent], lowLink_[vertex]);
      }
    }
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
