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
  Add() : gnode::Node() { this->out = std::make_shared<FloatData>(); };

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

int main()
{

  auto a = std::make_shared<FloatData>(5.f);
  auto b = std::make_shared<FloatData>(3.f);

  Add node_add;
  node_add.set_input_data(a, 0);
  node_add.compute();

  node_add.set_input_data(b, 1);
  node_add.compute();

  std::cout << std::boolalpha;
  std::cout << "a and b are of same type: " << a->is_same_type(*b) << "\n";
  std::cout << "type name: " << a->get_type() << "\n";

  return 0;
}
