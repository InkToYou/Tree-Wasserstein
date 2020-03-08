#ifndef __TREEWASSERSTEIN_H_INCLUDED__
#define __TREEWASSERSTEIN_H_INCLUDED__

#include <cinttypes>
#include <cmath>
#include <optional>
#include <queue>
#include <stdexcept>
#include <tuple>
#include <vector>
using EdgeList =
    std::vector<std::tuple<int64_t, int64_t, double>>;  //(from, to, weight)
using Tree = std::vector<std::vector<
    std::tuple<int64_t, double, int64_t>>>;  //(to, weight, edge_idx)
using Prob = std::vector<double>;

class Node {
 public:
  Node(const int64_t idx, const bool is_root);
  Node(Node&& node);
  int64_t idx;
  bool is_root;
  std::optional<int64_t> parent_idx;
  std::optional<double> parent_weight;
  std::optional<double> first_prob;
  std::optional<double> second_prob;
  double cumul_first_prob;
  double cumul_second_prob;
  double cumul_edge_weight;
  std::vector<int64_t> child_list;
};
using NodeList = std::vector<Node>;

inline Node::Node(const int64_t idx, const bool is_root)
    : idx(idx),
      is_root(is_root),
      parent_idx(std::nullopt),
      parent_weight(std::nullopt),
      first_prob(std::nullopt),
      second_prob(std::nullopt),
      cumul_first_prob(0),
      cumul_second_prob(0),
      cumul_edge_weight(0),
      child_list({}){};

inline Node::Node(Node&& node)
    : idx(node.idx),
      is_root(node.is_root),
      parent_idx(node.parent_idx),
      parent_weight(node.parent_weight),
      first_prob(node.first_prob),
      second_prob(node.second_prob),
      cumul_first_prob(node.cumul_first_prob),
      cumul_second_prob(node.cumul_second_prob),
      cumul_edge_weight(node.cumul_edge_weight),
      child_list({}){};

class TreeMetric {
 public:
  TreeMetric(const int64_t num_node, EdgeList& edges, const int64_t root_idx);
  int64_t num_node;
  int64_t root_idx;
  NodeList nodes;
  std::vector<int64_t> leaves;
  double TWDistance(const Prob& first_prob, const Prob& second_prob);

 private:
  void accumulateNodeProb();
  void accumulateEdgeWeight();
  void setProb(const Prob& first_prob, const Prob& second_prob);
  double getCumulEdgeWeight(const Prob& first_prob, const Prob& second_prob);
};

#endif
