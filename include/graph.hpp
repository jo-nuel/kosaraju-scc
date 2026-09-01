#ifndef GRAPH_HPP_
#define GRAPH_HPP_

#include <cstddef>
#include <vector>

class DirectedGraph {
 public:
  explicit DirectedGraph(std::size_t vertexCount);

  std::size_t vertexCount() const;
  void addEdge(std::size_t from, std::size_t to);
  const std::vector<std::size_t>& neighbours(std::size_t vertex) const;
  DirectedGraph transpose() const;

 private:
  void checkVertex(std::size_t vertex) const;

  std::vector<std::vector<std::size_t> > adjacencyList_;
};

#endif  // GRAPH_HPP_
