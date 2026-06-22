#include <random>

#include <gtest/gtest.h>

#include "nodes.hpp"

#include <iostream>

TEST(GraphStress, RandomDAG_Correctness)
{
  gnode::Graph g;

  constexpr int N = 5000;
  constexpr int NV = 20;

  std::vector<std::string> ids(N);
  std::vector<float>       expected(N);

  std::mt19937                       rng(42);
  std::uniform_int_distribution<int> dist(0, NV);

  // create initial values
  for (int i = 0; i < N; ++i)
  {
    if (i < 20)
    {
      float v = (float)dist(rng);
      ids[i] = g.add_node<Value>(v);
      expected[i] = v;
    }
    else
    {
      ids[i] = g.add_node<Add>();
      expected[i] = 0.f;
    }
  }

  // build DAG + compute reference
  for (int i = NV; i < N; ++i)
  {
    int p1 = std::uniform_int_distribution<>(0, NV - 1)(rng);
    int p2 = std::uniform_int_distribution<>(0, NV - 1)(rng);

    g.new_link(ids[p1], "value", ids[i], "a");
    g.new_link(ids[p2], "value", ids[i], "b");

    expected[i] = expected[p1] + expected[p2];
  }

  EXPECT_NO_THROW(g.update());

  // verify last node
  auto *node = g.get_node_ref_by_id<Add>(ids.back());
  ASSERT_NE(node, nullptr);

  auto *out = node->get_value_ref<float>("a + b");
  ASSERT_NE(out, nullptr);

  EXPECT_FLOAT_EQ(*out, expected.back());
}
