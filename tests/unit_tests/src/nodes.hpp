// test_nodes.hpp
#pragma once

#include "gnode.hpp"

class Add : public gnode::Node
{
public:
  Add() : gnode::Node("Add")
  {
    add_port<float>(gnode::PortType::IN, "a");
    add_port<float>(gnode::PortType::IN, "b");
    add_port<float>(gnode::PortType::OUT, "a + b");
  }

  void compute() override
  {
    auto *a = get_value_ref<float>("a");
    auto *b = get_value_ref<float>("b");
    auto *out = get_value_ref<float>("a + b");

    if (a && b) *out = *a + *b;
  }
};

class Value : public gnode::Node
{
public:
  Value() : gnode::Node("Value")
  {
    add_port<float>(gnode::PortType::OUT, "value");
  }

  explicit Value(float value) : Value() { set_value<float>("value", value); }

  void compute() override {}
};
