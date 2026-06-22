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

TEST(GraphCycle, DetectSimpleCycle)
{
  gnode::Graph g;

  auto a = g.add_node<Add>();

  g.new_link(a, "a + b", a, "a");
  g.new_link(a, "a + b", a, "b");

  EXPECT_TRUE(g.has_cycle());
}

TEST(GraphCycle, DetectIndirectCycle)
{
  gnode::Graph g;

  auto a = g.add_node<Value>(1.f);
  auto b = g.add_node<Add>();
  auto c = g.add_node<Add>();

  g.new_link(a, "value", b, "a");
  g.new_link(b, "a + b", c, "a");

  // create indirect cycle: c -> b
  g.new_link(c, "a + b", b, "a");

  EXPECT_TRUE(g.has_cycle());
}
