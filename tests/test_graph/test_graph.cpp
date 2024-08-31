#include "gnode.hpp"
#include <iostream>

class FloatData : public gnode::Data
{
public:
  explicit FloatData(float value = 0.f) : gnode::Data("float"), value(value) {}

  float  get_value() const { return value; }
  float *get_value_ref() { return &value; }

private:
  float value;
};

class IntData : public gnode::Data
{
public:
  explicit IntData(int value = 0) : gnode::Data("int"), value(value) {}

  int  get_value() const { return value; }
  int *get_value_ref() { return &value; }

private:
  int value;
};

class Add : public gnode::Node
{
public:
  Add() : gnode::Node("Add") { this->out = std::make_shared<FloatData>(); };

  void compute()
  {
    float *p_in1 = GN_GET_POINTER(this->in1);
    float *p_in2 = GN_GET_POINTER(this->in2);
    float *p_out = this->out->get_value_ref();

    if (p_in1 && p_in2)
    {
      *p_out = *p_in1 + *p_in2;
      std::cout << "computed, sum is: " << *p_out << "\n";
    }
    else
      std::cout << "not computed\n";
  }

  std::shared_ptr<gnode::Data> get_output_data(int port_index)
  {
    switch (port_index)
    {
    case 0: return std::static_pointer_cast<gnode::Data>(this->out); break;
    }
  }

  void set_input_data(std::shared_ptr<gnode::Data> data, int port_index)
  {
    switch (port_index)
    {
    case 0: this->in1 = std::dynamic_pointer_cast<FloatData>(data); break;
    case 1: this->in2 = std::dynamic_pointer_cast<FloatData>(data);
    }
  }

private:
  std::weak_ptr<FloatData>   in1, in2;
  std::shared_ptr<FloatData> out;
};

class Value : public gnode::Node
{
public:
  Value() : gnode::Node("Value") { this->out = std::make_shared<FloatData>(); };

  void compute()
  {
    float *p_out = this->out->get_value_ref();
    *p_out = 1.f;
  }

  std::shared_ptr<gnode::Data> get_output_data(int port_index)
  {
    switch (port_index)
    {
    case 0: return std::static_pointer_cast<gnode::Data>(this->out); break;
    }
  }

private:
  std::shared_ptr<FloatData> out;
};

int main()
{

  auto a = std::make_shared<FloatData>(5.f);
  auto b = std::make_shared<FloatData>(3.f);

  // Add node_add;
  // node_add.set_input_data(a, 0);
  // node_add.compute();

  // node_add.set_input_data(b, 1);
  // node_add.compute();

  gnode::Graph g;

  auto id_add1 = g.add_node(std::make_shared<Add>());
  auto id_add2 = g.add_node(std::make_shared<Add>());
  auto id_value1 = g.add_node(std::make_shared<Value>());
  auto id_value2 = g.add_node(std::make_shared<Value>());
  auto id_value3 = g.add_node(std::make_shared<Value>());

  g.get_node_ref_by_id<Value>(id_value1)->compute();
  g.get_node_ref_by_id<Value>(id_value2)->compute();
  g.get_node_ref_by_id<Value>(id_value3)->compute();

  g.get_node_ref_by_id<Add>(id_add1)->set_input_data(a, 0);
  g.get_node_ref_by_id<Add>(id_add1)->set_input_data(b, 1);
  g.get_node_ref_by_id<Add>(id_add1)->update();

  g.connect(id_value1, 0, id_add1, 0);
  g.connect(id_value2, 0, id_add1, 1);
  g.connect(id_add1, 0, id_add2, 0);
  g.connect(id_value3, 0, id_add2, 1);

  g.update();

  g.print();
  g.export_to_graphviz();

  // g.get_node_ref_by_id<Add>(id_add1)->compute();

  g.update_node(id_value1);

  g.print();

  return 0;
}
