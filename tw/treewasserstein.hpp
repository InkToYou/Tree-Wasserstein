#ifndef __TREEWASSERSTEIN_H
#define __TREEWASSERSTEIN_H

#include <cinttypes>
#include <cmath>
#include <optional>
#include <queue>
#include <stdexcept>
#include <tuple>
#include <vector>
using EdgeList =
    std::vector<std::tuple<uint32_t, uint32_t, double>>;  //(from, to, weight)
using Tree = std::vector<std::vector<
    std::tuple<uint32_t, double, uint32_t>>>;  //(to, weight, edge_idx)
using Prob = std::vector<double>;

class Node {
 public:
  Node(const uint32_t idx, const bool is_root);
  Node(Node&& node);
  uint32_t idx;
  bool is_root;
  std::optional<uint32_t> parent_idx;
  std::optional<double> parent_weight;
  std::optional<double> first_prob;
  std::optional<double> second_prob;
  double cumul_first_prob;
  double cumul_second_prob;
  double cumul_edge_weight;
  std::vector<uint32_t> child_list;
};
using NodeList = std::vector<Node>;

inline Node::Node(const uint32_t idx, const bool is_root)
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
  TreeMetric(const uint32_t num_node, EdgeList& edges, const uint32_t root_idx);
  uint32_t num_node;
  uint32_t root_idx;
  NodeList nodes;
  std::vector<uint32_t> leaves;
  double TWDistance(const Prob& first_prob, const Prob& second_prob);

 private:
  void accumulateNodeProb();
  void accumulateEdgeWeight();
  void setProb(const Prob& first_prob, const Prob& second_prob);
  double getCumulEdgeWeight(const Prob& first_prob, const Prob& second_prob);
};

double tw(const Prob& first_prob, const Prob& second_prob, EdgeList& edges);

#endif
