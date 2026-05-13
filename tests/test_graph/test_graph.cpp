#include "gnode.hpp"
#include <iostream>

class Add : public gnode::Node
{
public:
  Add() : gnode::Node("Add")
  {
    this->add_port<float>(gnode::PortType::IN, "a");
    this->add_port<float>(gnode::PortType::IN, "b");
    this->add_port<float>(gnode::PortType::OUT, "a + b");
  };

  void compute()
  {
    float *p_in1 = this->get_value_ref<float>("a");
    float *p_in2 = this->get_value_ref<float>("b");
    float *p_out = this->get_value_ref<float>("a + b");

    if (p_in1 && p_in2)
    {
      *p_out = *p_in1 + *p_in2;
      std::cout << "computed, sum is: " << *p_out << "\n";
    }
    else
      std::cout << "not computed\n";
  }
};

class Value : public gnode::Node
{
public:
  Value() : gnode::Node("Value")
  {
    this->add_port<float>(gnode::PortType::OUT, "value");
  };

  Value(float value) : gnode::Node("Value")
  {
    this->add_port<float>(gnode::PortType::OUT, "value");
    this->set_value<float>("value", value);
  };

  void compute() {}
};

static void check(const std::string &label, bool result, bool expected)
{
  const char *mark = (result == expected) ? "✓" : "✗";
  const char *val = result ? "true" : "false";
  std::cout << "  " << mark << "  " << label << ": " << val << "\n";
}

int main()
{
  gnode::Graph g;

  auto id_add1 = g.add_node<Add>();
  auto id_add2 = g.add_node<Add>();
  auto id_value1 = g.add_node<Value>(5.f);
  auto id_value2 = g.add_node<Value>(1.f);
  auto id_value3 = g.add_node<Value>(2.f);

  g.new_link(id_value1, "value", id_add1, "a");
  g.new_link(id_value2, "value", id_add1, "b");
  g.new_link(id_add1, "a + b", id_add2, "a");
  g.new_link(id_value3, "value", id_add2, "b");

  // overall update to ensure a clean graph state (greedy and inefficient)
  std::cout << "\nOVERALL UPDATE\n";

  g.update();

  // change one node state = only update from this node and propagate
  // changes only where it is necessary
  std::cout << "\nNODE UPDATE\n";

  g.get_node_ref_by_id<Value>(id_value3)->set_value<float>("value", 10.f);
  g.update(id_value3);

  // g.print();
  // g.export_to_graphviz();

  // --- Cycle checking

  std::cout << "cycle detection:\n";
  check("is_reachable(value3 → value3)  [self-loop]",
        g.is_reachable(id_value3, id_value3),
        true);
  check("is_reachable(add1   → value3)  [no path]",
        g.is_reachable(id_add1, id_value3),
        false);
  check("is_reachable(add1   → add2)    [valid path]",
        g.is_reachable(id_add1, id_add2),
        true);

  // Port checking

  auto *value3 = g.get_node_ref_by_id(id_value3);

  std::cout << "\nport checks on Value node:\n";
  check(R"(has_port("value")         [exists])",
        value3->has_port("value"),
        true);
  check(R"(has_port<int>("value")    [wrong type])",
        value3->has_port<int>("value"),
        false);
  check(R"(has_port("a")             [absent])", value3->has_port("a"), false);

  return 0;
}
