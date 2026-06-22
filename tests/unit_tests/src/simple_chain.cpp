#include <gtest/gtest.h>

#include "nodes.hpp"

TEST(GraphUpdate, SimpleChain)
{
  gnode::Graph g;

  auto v1 = g.add_node<Value>(5.f);
  auto v2 = g.add_node<Value>(1.f);
  auto v3 = g.add_node<Value>(2.f);

  auto add1 = g.add_node<Add>();
  auto add2 = g.add_node<Add>();

  g.new_link(v1, "value", add1, "a");
  g.new_link(v2, "value", add1, "b");

  g.new_link(add1, "a + b", add2, "a");
  g.new_link(v3, "value", add2, "b");

  g.update();

  auto *node = g.get_node_ref_by_id<Add>(add2);

  float *result = node->get_value_ref<float>("a + b");

  ASSERT_NE(result, nullptr);
  EXPECT_FLOAT_EQ(*result, 8.f);
}
