#pragma once

#include <cinttypes>
#include <cmath>
#include <queue>
#include <stdexcept>
#include <tuple>
#include <vector>
using EdgeList =
    std::vector<std::tuple<uint32_t, uint32_t, double>>;  //(from, to, weight)
using Tree = std::vector<std::vector<
    std::tuple<uint32_t, double, uint32_t>>>;  //(to, weight, edge_idx)
using Prob = std::vector<double>;

bool isTree(const uint32_t num_node, EdgeList& edges, const uint32_t root_idx) {
  if (edges.size() != num_node - 1) return false;
  Tree tree(num_node);
  std::vector<bool> visited_node(num_node, false);
  std::vector<bool> visited_edge(edges.size(), false);

  // Create the adjacency list
  for (size_t i = 0; i < edges.size(); ++i) {
    uint32_t from, to;
    double weight;
    std::tie(from, to, weight) = edges[i];
    tree[from].push_back(std::make_tuple(to, weight, i));
    tree[to].push_back(std::make_tuple(from, weight, i));
  }

  // Judge whether that is a tree and Normalize it
  edges.clear();
  std::queue<uint32_t> que;
  que.push(root_idx);
  visited_node[root_idx] = true;
  while (!que.empty()) {
    uint32_t from = que.front();
    que.pop();
    for (auto e : tree[from]) {
      uint32_t to, edge_idx;
      double weight;
      std::tie(to, weight, edge_idx) = e;

      if (visited_edge[edge_idx]) continue;
      if (visited_node[to]) return false;
      visited_node[to] = true;
      visited_edge[edge_idx] = true;
      que.push(to);
      edges.push_back(std::make_tuple(from, to, weight));
    }
  }
  return true;
}

int32_t sign(const double x) { return x >= 0 ? 1 : -1; };

class Node {
 public:
  Node(const uint32_t idx, const bool is_root);
  Node(Node&& node);
  int32_t idx;
  int32_t parent_idx;
  bool is_root;
  double parent_weight;
  double first_prob;
  double second_prob;
  double cumul_first_prob;
  double cumul_second_prob;
  double cumul_edge_weight;
  std::vector<uint32_t> child_list;
};
using NodeList = std::vector<Node>;

inline Node::Node(const uint32_t idx, const bool is_root)
    : idx(idx),
      is_root(is_root),
      parent_idx(-1),
      parent_weight(0),
      first_prob(0),
      second_prob(0),
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

TreeMetric::TreeMetric(const uint32_t num_node, EdgeList& edges,
                       uint32_t root_idx = 0)
    : num_node(num_node), root_idx(root_idx) {
  for (size_t i = 0; i < num_node; ++i) nodes.push_back(Node(i, i == root_idx));
  // Check whether tree or not and Convert to(parent, child, weight)
  if (!isTree(num_node, edges, root_idx))
    throw std::invalid_argument("Not tree");
  // Set parents
  for (auto e : edges) {
    uint32_t parent, child;
    double weight;
    std::tie(parent, child, weight) = e;
    this->nodes[parent].child_list.push_back(child);
    this->nodes[child].parent_idx = parent;
    this->nodes[child].parent_weight = weight;
  }

  // Set children
  for (Node& n : this->nodes) {
    if (n.child_list.empty()) this->leaves.push_back(n.idx);
    if (!n.parent_idx) this->root_idx = n.idx;
  }
}

void TreeMetric::accumulateNodeProb() {
  std::vector<uint32_t> visited_count(num_node, 0);
  std::queue<uint32_t> que;
  for (uint32_t l : this->leaves) que.push(l);
  while (!que.empty()) {
    Node& node = this->nodes[que.front()];
    que.pop();

    if (node.is_root) {
      visited_count[this->root_idx]++;
      continue;
    }

    Node& parent = this->nodes[node.parent_idx];
    parent.cumul_first_prob += node.cumul_first_prob;
    parent.cumul_second_prob += node.cumul_second_prob;

    visited_count[parent.idx]++;
    if (visited_count[parent.idx] == parent.child_list.size())
      que.push(parent.idx);
  }
}

void TreeMetric::accumulateEdgeWeight() {
  std::queue<uint32_t> que;
  que.push(this->root_idx);

  while (!que.empty()) {
    Node& node = this->nodes[que.front()];
    que.pop();
    for (uint32_t c : node.child_list) {
      Node& child = this->nodes[c];
      double prob_diff = child.cumul_first_prob - child.cumul_second_prob;
      child.cumul_edge_weight =
          node.cumul_edge_weight + sign(prob_diff) * child.parent_weight;
      que.push(c);
    }
  }
}

void TreeMetric::setProb(const Prob& first_prob, const Prob& second_prob) {
  for (size_t idx = 0; idx < this->num_node; ++idx) {
    this->nodes[idx].first_prob = first_prob[idx];
    this->nodes[idx].second_prob = second_prob[idx];
    this->nodes[idx].cumul_first_prob = first_prob[idx];
    this->nodes[idx].cumul_second_prob = second_prob[idx];
  }
  accumulateNodeProb();
  accumulateEdgeWeight();
}

double TreeMetric::TWDistance(const Prob& frist_prob, const Prob& second_prob) {
  setProb(frist_prob, second_prob);
  double twd = 0;
  for (Node& n : this->nodes)
    twd += n.cumul_edge_weight * (n.first_prob - n.second_prob);
  return twd;
}

double distance(const Prob& first_prob, const Prob& second_prob,
                EdgeList& edges) {
  uint32_t num_node = first_prob.size();
  TreeMetric tm(num_node, edges);
  return tm.TWDistance(first_prob, second_prob);
}
