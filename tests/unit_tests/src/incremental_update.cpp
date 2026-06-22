#include <gtest/gtest.h>

#include "nodes.hpp"

#include <spdlog/spdlog.h>

TEST(GraphUpdate, IncrementalUpdate)
{
  gnode::Graph g;

  auto v1 = g.add_node<Value>(5.f);
  auto v2 = g.add_node<Value>(1.f);

  auto add = g.add_node<Add>();

  g.new_link(v1, "value", add, "a");
  g.new_link(v2, "value", add, "b");

  g.update();

  auto *out = g.get_node_ref_by_id<Add>(add)->get_value_ref<float>("a + b");

  EXPECT_FLOAT_EQ(*out, 6.f);

  g.get_node_ref_by_id<Value>(v2)->set_value<float>("value", 10.f);

  g.update(v2);

  EXPECT_FLOAT_EQ(*out, 15.f);
}

#include <iostream>

TEST(GraphStress, IncrementalUpdate5000Nodes)
{
  gnode::Graph g;

  constexpr int N = 5000;

  auto root = g.add_node<Value>(1.f);

  std::vector<std::string> adds;

  for (int i = 0; i < N; ++i)
    adds.push_back(g.add_node<Add>());

  g.new_link(root, "value", adds[0], "a");
  g.new_link(root, "value", adds[0], "b");

  for (int i = 1; i < N; ++i)
  {
    g.new_link(adds[i - 1], "a + b", adds[i], "a");
    g.new_link(root, "value", adds[i], "b");
  }

  EXPECT_NO_THROW(g.update());

  float result = *g.get_node_ref_by_id<Add>(adds.back())
                      ->get_value_ref<float>("a + b");
  EXPECT_FLOAT_EQ(result, 5001.f);

  g.get_node_ref_by_id<Value>(root)->set_value<float>("value", 2.f);
  EXPECT_NO_THROW(g.update(root));

  result = *g.get_node_ref_by_id<Add>(adds.back())
                ->get_value_ref<float>("a + b");
  EXPECT_FLOAT_EQ(result, 10002.f);
}
