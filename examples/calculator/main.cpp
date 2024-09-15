#include "gnode.hpp"
#include <iostream>

// create a class to show that complex objects can be handled
struct Vec2
{
  float x;
  float y;
  Vec2() = default;
  Vec2(float x, float y) : x(x), y(y) {}
};

// ----------------------------------------
// Nodes definition, all derivates from gnode::Node
// ----------------------------------------

// holds a float value
class Value : public gnode::Node
{
public:
  Value() : gnode::Node("Value")
  {
    this->add_port<float>(gnode::PortType::OUT, "value");
  }

  Value(float value) : gnode::Node("Value")
  {
    this->add_port<float>(gnode::PortType::OUT, "value");
    this->set_value<float>("value", value);
  }

  void compute() {}
};

// holds a Vec2 value
class ValueVec : public gnode::Node
{
public:
  ValueVec() : gnode::Node("ValueVec")
  {
    this->add_port<Vec2>(gnode::PortType::OUT, "value");
  }

  ValueVec(float x, float y) : gnode::Node("ValueVec")
  {
    // option 1: pass the object constructor arguments
    this->add_port<Vec2>(gnode::PortType::OUT, "value", x, y);

    // option 2: set the value manually
    // this->add_port<Vec2>(gnode::PortType::OUT, "value");
    // this->set_value<Vec2>("value", Vec2(x, y));
  }

  void compute() {}
};

// add two floats
class Add : public gnode::Node
{
public:
  Add() : gnode::Node("Add")
  {
    this->add_port<float>(gnode::PortType::IN, "a");
    this->add_port<float>(gnode::PortType::IN, "b");
    this->add_port<float>(gnode::PortType::OUT, "a + b");
  }

  void compute()
  {
    float *p_in1 = this->get_value_ref<float>("a");
    float *p_in2 = this->get_value_ref<float>("b");
    float *p_out = this->get_value_ref<float>("a + b");

    if (p_in1 && p_in2) *p_out = *p_in1 + *p_in2;
  }
};

// sum Vec2 components and returns a float
class SumVec : public gnode::Node
{
public:
  SumVec() : gnode::Node("SumVec")
  {
    this->add_port<Vec2>(gnode::PortType::IN, "vec");
    this->add_port<float>(gnode::PortType::OUT, "sum");
  }

  void compute()
  {
    Vec2  *p_vec = this->get_value_ref<Vec2>("vec");
    float *p_sum = this->get_value_ref<float>("sum");

    if (p_vec) *p_sum = p_vec->x + p_vec->y;
  }
};

// print a float
class Print : public gnode::Node
{
public:
  Print() : gnode::Node("Print")
  {
    this->add_port<float>(gnode::PortType::IN, "in");
  }

  void compute()
  {
    float *p_in = this->get_value_ref<float>("in");

    if (p_in) std::cout << "PRINTING: " << *p_in << "\n";
  }
};

int main()
{
  gnode::Graph g;

  // NB - node constructor parameters are pass through when adding a node
  auto id_value_vec = g.add_node<ValueVec>(1.f, 2.f);
  auto id_sum_vec = g.add_node<SumVec>();

  // sum vector components
  g.new_link(id_value_vec, "value", id_sum_vec, "vec");

  // add the former sum to a float
  auto id_value1 = g.add_node<Value>(3.f);
  auto id_add1 = g.add_node<Add>();
  g.new_link(id_sum_vec, "sum", id_add1, "a");
  g.new_link(id_value1, "value", id_add1, "b");

  // print this intermediate value
  auto id_print1 = g.add_node<Print>();
  g.new_link(id_add1, "a + b", id_print1, "in");

  // add the former sum to another float...
  auto id_value2 = g.add_node<Value>(4.f);
  auto id_add2 = g.add_node<Add>();
  g.new_link(id_add1, "a + b", id_add2, "a");
  g.new_link(id_value2, "value", id_add2, "b");

  // and print the results
  auto id_print2 = g.add_node<Print>();
  g.new_link(id_add2, "a + b", id_print2, "in");

  // once the graph is defined, perform an overall update to ensure a
  // clean graph state at the beginning (greedy and inefficient, don't
  // use it after to update the nodes when a single node state is
  // modified)
  std::cout << "\nOVERALL UPDATE\n";

  g.update();

  // --- change one node state = only update from this node and
  // --- propagate changes only where it is necessary
  std::cout << "\nNODE UPDATE\n";

  g.get_node_ref_by_id<Value>(id_value1)->set_value<float>("value", 10.f);
  g.update(id_value1);

  // export to check: dot export.dot -Tsvg > output.svg
  g.export_to_graphviz();
  g.export_to_mermaid();

  return 0;
}
