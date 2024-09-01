#include "gnode.hpp"
#include <iostream>

class Add : public gnode::Node
{
public:
  Add() : gnode::Node("Add")
  {
    this->inputs = {std::make_shared<gnode::Input<float>>("a"),
                    std::make_shared<gnode::Input<float>>("b")};

    this->outputs = {std::make_shared<gnode::Output<float>>("a + b")};
  };

  void compute()
  {
    float *p_in1 = get_input_value_ref<float>(0);
    float *p_in2 = get_input_value_ref<float>(1);
    float *p_out = get_output_value_ref<float>(0);

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
    this->outputs = {std::make_shared<gnode::Output<float>>("value")};
  };

  void compute()
  {
    float *p_out = this->get_output_value_ref<float>(0);
    *p_out = 1.f;
  }
};

int main()
{
  gnode::Graph g;

  g.print();

  auto id_add1 = g.add_node(std::make_shared<Add>());
  auto id_add2 = g.add_node(std::make_shared<Add>());
  auto id_value1 = g.add_node(std::make_shared<Value>());
  auto id_value2 = g.add_node(std::make_shared<Value>());
  auto id_value3 = g.add_node(std::make_shared<Value>());

  g.get_node_ref_by_id<Value>(id_value1)->compute();
  g.get_node_ref_by_id<Value>(id_value2)->compute();
  g.get_node_ref_by_id<Value>(id_value3)->compute();

  g.connect(id_value1, 0, id_add1, 0);
  g.connect(id_value2, 0, id_add1, 1);
  g.connect(id_add1, 0, id_add2, 0);
  g.connect(id_value3, 0, id_add2, 1);

  g.update();

  g.print();
  g.export_to_graphviz();

  return 0;
}
