#include "treewasserstein.hpp"
#include <pybind11/pybind11.h>

bool VerifyTree(const int64_t num_node, EdgeList& edges,
                const int64_t root_idx) {
  if (edges.size() != num_node - 1) return false;
  Tree tree;
  std::vector<bool> visited_node(num_node, false);
  std::vector<bool> visited_edge(edges.size(), false);

  // Create the adjacency list
  for (int64_t i = 0; i < edges.size(); ++i) {
    int64_t from, to;
    double weight;
    std::tie(from, to, weight) = edges[i];
    tree[from].push_back(std::make_tuple(to, weight, i));
    tree[to].push_back(std::make_tuple(from, weight, i));
  }

  // Judge whether that is a tree and Normalize it
  edges.clear();
  std::queue<int64_t> que;
  que.push(root_idx);
  visited_node[root_idx] = true;
  while (!que.empty()) {
    int64_t from = que.front();
    que.pop();
    for (auto e : tree[from]) {
      int64_t to, edge_idx;
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
};

TreeMetric::TreeMetric(const int64_t num_node, EdgeList& edges,
                       int64_t root_idx = 0)
    : num_node(num_node), root_idx(root_idx) {
  for (int64_t i = 0; i < num_node; ++i)
    nodes.push_back(Node(i, i == root_idx));
  // Check whether tree or not and Convert to(parent, child, weight)
  if (VerifyTree(num_node, edges, root_idx))
    throw std::invalid_argument("Not tree");

  // Set parents
  for (auto e : edges) {
    int64_t parent, child;
    double weight;
    std::tie(parent, child, weight) = e;
    nodes[parent].child_list.push_back(child);
    nodes[child].parent_idx = parent;
    nodes[child].parent_weight = weight;
  }

  // Set children
  for (Node& n : nodes) {
    if (n.child_list.empty()) leaves.push_back(n.idx);
    if (!n.parent_idx.has_value()) root_idx = n.idx;
  }
};

void TreeMetric::accumulateNodeProb() {
  std::vector<int64_t> visited_count(num_node, 0);
  std::queue<int64_t> que;
  for (int64_t l : leaves) que.push(l);
  while (!que.empty()) {
    Node& node = nodes[que.front()];
    que.pop();

    if (node.is_root) {
      visited_count[root_idx]++;
      continue;
    }

    Node& parent = nodes[node.parent_idx.value()];
    parent.cumul_first_prob += node.cumul_first_prob;
    parent.cumul_second_prob += node.cumul_second_prob;

    visited_count[parent.idx]++;
    if (visited_count[parent.idx] == parent.child_list.size())
      que.push(node.parent_idx.value());
  }
};

void TreeMetric::accumulateEdgeWeight() {
  std::queue<int64_t> que;
  que.push(root_idx);

  while (!que.empty()) {
    Node& node = nodes[que.front()];
    que.pop();
    for (int64_t c : node.child_list) {
      Node& child = nodes[c];
      double prob_diff = child.cumul_first_prob - child.cumul_second_prob;
      child.cumul_edge_weight +=
          std::signbit(prob_diff) * child.parent_weight.value();
      que.push(c);
    }
  }
};

void TreeMetric::setProb(const Prob& first_prob, const Prob& second_prob) {
  for (int64_t idx = 0; idx < num_node; ++idx) {
    nodes[idx].first_prob = first_prob[idx];
    nodes[idx].second_prob = second_prob[idx];
    nodes[idx].cumul_first_prob = first_prob[idx];
    nodes[idx].cumul_second_prob = second_prob[idx];
  }
  accumulateNodeProb();
  accumulateEdgeWeight();
};

double TreeMetric::TWDistance(const Prob& frist_prob, const Prob& second_prob) {
  setProb(frist_prob, second_prob);
  double twd = 0;
  for (Node& n : nodes)
    twd += n.cumul_edge_weight * (n.first_prob.value() - n.second_prob.value());
  return twd;
}

double tw(const Prob& first_prob, const Prob& second_prob, EdgeList& edges) {
  int64_t num_node = first_prob.size();
  TreeMetric tm(num_node, edges);
  return tm.TWDistance(first_prob, second_prob);
}

PYBIND11_MODULE(tw, m) {
  m.doc() = "pybind11";
  m.def("tw", &tw, "Tree Wasserstein");
}
