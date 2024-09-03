#include "gnode.hpp"
#include <iostream>

class Add : public gnode::Node
{
public:
  Add() : gnode::Node("Add")
  {
    std::cout << "here\n";
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

  void compute()
  {
    // float *p_out = this->get_value_ref<float>("value");
    // *p_out = 1.f;
  }
};

int main()
{
  gnode::Graph g;

  g.print();

  auto id_add1 = g.add_node<Add>();
  auto id_add2 = g.add_node<Add>();
  auto id_value1 = g.add_node<Value>(5.f);
  auto id_value2 = g.add_node<Value>(1.f);
  auto id_value3 = g.add_node<Value>(2.f);

  // std::cout << g.get_node_ref_by_id<Add>(id_add1)->get_nports(
  //                  gnode::PortType::IN)
  //           << "\n";
  // std::cout << g.get_node_ref_by_id<Add>(id_add1)->get_nports(
  //                  gnode::PortType::OUT)
  //           << "\n";

  // // // g.get_node_ref_by_id<Value>(id_value1)->compute();
  // // g.get_node_ref_by_id<Value>(id_value2)->compute();
  // // g.get_node_ref_by_id<Value>(id_value3)->compute();

  g.connect(id_value1, "value", id_add1, "a");
  g.connect(id_value2, "value", id_add1, "b");
  g.connect(id_add1, "a + b", id_add2, "a");
  g.connect(id_value3, "value", id_add2, "b");

  g.update();

  g.print();
  g.export_to_graphviz();

  return 0;
}
