/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file data.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @date 2023-08-07
 *
 * @copyright Copyright (c) 2023
 */
#pragma once
#include <memory>
#include <stdexcept>
#include <vector>

#include "gnode/data.hpp"
#include "gnode/port.hpp"

#define GN_GET_POINTER(X) X.lock() ? X.lock()->get_value_ref() : nullptr

namespace gnode
{

// abstract node class
class Node
{
public:
  bool is_dirty = false;

  Node() = default;

  explicit Node(std::string label) : label(std::move(label)) {}

  virtual ~Node() = default;

  template <typename T>
  void add_port(PortType port_type, const std::string &port_label)
  {
    if (port_type == PortType::IN)
      this->ports.push_back(std::make_shared<gnode::Input<T>>(port_label));
    else
      this->ports.push_back(std::make_shared<gnode::Output<T>>(port_label));
  }

  // to force compute to be defined
  virtual void compute() = 0;

  std::string get_label() const { return this->label; }

  int get_nports() const;

  // Get output data from a specific port index
  std::shared_ptr<BaseData> get_output_data(int port_index) const;

  int get_port_index(const std::string &port_label) const;

  std::string get_port_label(int port_index) const;

  PortType get_port_type(const std::string &port_label) const;

  template <typename T> T *get_value_ref(const std::string &port_label) const
  {
    // Search for the port in the inputs vector
    for (const auto &port : this->ports)
      if (port->get_label() == port_label)
      {
        if (port->get_port_type() == PortType::IN)
        {
          auto inputPort = std::dynamic_pointer_cast<Input<T>>(port);
          if (inputPort) return inputPort->get_value_ref();
        }
        else
        {
          auto outputPort = std::dynamic_pointer_cast<Output<T>>(port);
          if (outputPort) return outputPort->get_value_ref();
        }
      }

    // If no matching port is found, throw an exception
    throw std::runtime_error("Port with label '" + port_label + "' not found.");
  }

  template <typename T> T *get_value_ref(int port_index) const
  {
    // Range check for the port index
    if (port_index < 0 || port_index >= static_cast<int>(this->ports.size()))
      throw std::out_of_range("Invalid port index");

    // Dynamic cast to the appropriate port type (Input or Output) and returns
    // the value reference if the port is valid, otherwise return nullptr
    if (this->ports[port_index]->get_port_type() == PortType::IN)
    {
      auto port = std::dynamic_pointer_cast<Input<T>>(this->ports[port_index]);
      return port ? port->get_value_ref() : nullptr;
    }
    else
    {
      auto port = std::dynamic_pointer_cast<Output<T>>(this->ports[port_index]);
      return port ? port->get_value_ref() : nullptr;
    }
  }

  // Set input data on a specific port index
  void set_input_data(std::shared_ptr<BaseData> data, int port_index);

  template <typename T>
  void set_value(const std::string &port_label, T new_value)
  {
    T *p_value = this->get_value_ref<T>(port_label);
    *p_value = new_value;
  }

  void update();

private:
  std::string                        label;
  std::vector<std::shared_ptr<Port>> ports;
};

} // namespace gnode