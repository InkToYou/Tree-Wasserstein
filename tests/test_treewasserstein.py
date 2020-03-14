import pytest
import numpy as np
import ot
import networkx as nx
import tw


class TestBuildValidTreeMetric(object):
  @pytest.mark.parametrize("num_node, edges", [
      (5, [(i % 5, (i + 1) % 5, i + 1) for i in range(5)]),
      (3, [(i % 3, (i + 1) % 3, i + 1) for i in range(3)]),
  ])
  def test_invalid_tree(self, num_node, edges):
    with pytest.raises(ValueError):
      first_prob = np.zeros(num_node)
      second_prob = np.zeros(num_node)
      first_prob[0] = 1.
      second_prob[-1] = 1.
      tw.distance(first_prob, second_prob, edges)


class TestTreeWasserstein(object):
  def test_tree_wasserstein(self):
    for i in range(100):
      num_node = np.random.randint(10, 200)
      G = nx.generators.random_tree(num_node)
      edges = [(fr, to, 1)
               for (fr, to) in list(G.edges())]
      first_prob = np.random.rand(num_node)
      first_prob = first_prob / first_prob.sum()
      second_prob = np.random.rand(num_node)
      second_prob = second_prob / second_prob.sum()
      twd = tw.distance(first_prob.tolist(), second_prob.tolist(), edges)

      adj_dict = dict(nx.all_pairs_shortest_path_length(G))
      metric = np.array([[adj_dict[i][j]
                          for i in range(num_node)] for j in range(num_node)])
      ans = ot.lp.emd2(first_prob, second_prob, metric)

      assert np.allclose([twd], [ans]), f"TW : {twd}, WD : {ans}"
