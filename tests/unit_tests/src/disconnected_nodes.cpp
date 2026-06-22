#include <gtest/gtest.h>

#include "nodes.hpp"

TEST(GraphUpdate, DisconnectedNodes)
{
  gnode::Graph g;

  auto add = g.add_node<Add>();

  EXPECT_NO_THROW(g.update());

  auto *result = g.get_node_ref_by_id<Add>(add)->get_value_ref<float>("a + b");

  EXPECT_FLOAT_EQ(*result, 0.f);
}
