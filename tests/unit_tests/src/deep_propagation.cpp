#include "nodes.hpp"

#include <gtest/gtest.h>

TEST(GraphUpdate, DeepPropagation)
{
  gnode::Graph g;
  auto         value = g.add_node<Value>(1.f);

  std::vector<std::string> adds;

  for (int i = 0; i < 100; ++i)
    adds.push_back(g.add_node<Add>());

  g.new_link(value, "value", adds[0], "a");
  g.new_link(value, "value", adds[0], "b");

  for (size_t i = 1; i < adds.size(); ++i)
  {
    g.new_link(adds[i - 1], "a + b", adds[i], "a");
    g.new_link(value, "value", adds[i], "b");
  }

  g.update();

  auto *result = g.get_node_ref_by_id<Add>(adds.back())
                     ->get_value_ref<float>("a + b");

  EXPECT_NE(result, nullptr);
}
