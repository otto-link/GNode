#include <gtest/gtest.h>

#include "nodes.hpp"

TEST(GraphCycle, Reachability)
{
  gnode::Graph g;

  auto v1 = g.add_node<Value>();
  auto add1 = g.add_node<Add>();
  auto add2 = g.add_node<Add>();

  g.new_link(v1, "value", add1, "a");
  g.new_link(add1, "a + b", add2, "a");

  EXPECT_TRUE(g.is_reachable(v1, v1));
  EXPECT_TRUE(g.is_reachable(add1, add2));
  EXPECT_FALSE(g.is_reachable(add2, add1));
}
