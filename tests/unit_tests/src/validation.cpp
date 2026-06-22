#include <gtest/gtest.h>

#include "nodes.hpp"

TEST(Ports, Validation)
{
  Value value;

  EXPECT_TRUE(value.has_port("value"));

  EXPECT_TRUE(value.has_port<float>("value"));

  EXPECT_FALSE(value.has_port<int>("value"));

  EXPECT_FALSE(value.has_port("missing"));
}

TEST(GraphLinks, InvalidPorts)
{
  gnode::Graph g;

  auto v = g.add_node<Value>();
  auto a = g.add_node<Add>();

  EXPECT_THROW(g.new_link(v, "does_not_exist", a, "a"), std::runtime_error);
}
