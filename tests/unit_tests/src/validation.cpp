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

TEST(GraphExceptions, GetNodeRefMissing)
{
  gnode::Graph g;
  EXPECT_THROW(g.get_node_ref_by_id("does_not_exist"), std::runtime_error);
}

TEST(NodeExceptions, GetValueRefExceptions)
{
  Value value;
  EXPECT_THROW(value.get_value_ref<float>("does_not_exist"),
               std::runtime_error);
  EXPECT_THROW(value.get_value_ref<int>("value"), std::runtime_error);
}

TEST(GraphExceptions, UpdateMissingNode)
{
  gnode::Graph g;
  EXPECT_THROW(g.update("does_not_exist"), std::runtime_error);
  EXPECT_THROW(g.update(std::vector<std::string>{"does_not_exist"}),
               std::runtime_error);
}
