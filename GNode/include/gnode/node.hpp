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
      this->inputs.push_back(std::make_shared<gnode::Input<T>>(port_label));
    else
      this->outputs.push_back(std::make_shared<gnode::Output<T>>(port_label));
  }

  // to force compute to be defined
  virtual void compute() = 0;

  std::string get_label() const { return this->label; }

  int get_nports(PortType port_type) const
  {
    const auto &ports = (port_type == PortType::IN) ? this->inputs
                                                    : this->outputs;
    return static_cast<int>(ports.size());
  }

  // Get output data from a specific port index
  std::shared_ptr<BaseData> get_output_data(int port_index) const
  {
    if (port_index < 0 || port_index >= static_cast<int>(outputs.size()))
      throw std::out_of_range("Invalid output port index");

    return outputs[port_index]->get_data_shared_ptr_downcasted();
  }

  std::pair<int, PortType> get_port_info_by_label(
      const std::string &port_label) const
  {
    // Search in the inputs vector
    for (int i = 0; i < static_cast<int>(inputs.size()); ++i)
      if (inputs[i]->get_label() == port_label) return {i, PortType::IN};

    // Search in the outputs vector
    for (int i = 0; i < static_cast<int>(outputs.size()); ++i)
      if (outputs[i]->get_label() == port_label) return {i, PortType::OUT};

    // If no matching port is found, throw an exception
    throw std::runtime_error("Port with label '" + port_label + "' not found.");
  }

  std::string get_port_label(PortType port_type, int port_index) const
  {
    const auto &ports = (port_type == PortType::IN) ? this->inputs
                                                    : this->outputs;

    if (port_index < 0 || port_index >= static_cast<int>(ports.size()))
      throw std::out_of_range("Port index is out of range.");

    return ports[port_index]->get_label();
  }

#include <stdexcept> // Include for std::runtime_error

  template <typename T> T *get_value_ref(const std::string &port_label) const
  {
    // Search for the port in the inputs vector
    for (const auto &port : inputs)
      if (port->get_label() == port_label)
      {
        auto inputPort = std::dynamic_pointer_cast<Input<T>>(port);
        if (inputPort) return inputPort->get_value_ref();
      }

    // Search for the port in the outputs vector if not found in inputs
    for (const auto &port : outputs)
      if (port->get_label() == port_label)
      {
        auto outputPort = std::dynamic_pointer_cast<Output<T>>(port);
        if (outputPort) return outputPort->get_value_ref();
      }

    // If no matching port is found, throw an exception
    throw std::runtime_error("Port with label '" + port_label + "' not found.");
  }

  template <typename T>
  T *get_value_ref(PortType port_type, int port_index) const
  {
    // Select the appropriate ports vector
    const auto &ports = (port_type == PortType::IN) ? inputs : outputs;

    // Range check for the port index
    if (port_index < 0 || port_index >= static_cast<int>(ports.size()))
      throw std::out_of_range("Invalid port index");

    // Dynamic cast to the appropriate port type (Input or Output) and returns
    // the value reference if the port is valid, otherwise return nullptr
    if (port_type == PortType::IN)
    {
      auto port = std::dynamic_pointer_cast<Input<T>>(ports[port_index]);
      return port ? port->get_value_ref() : nullptr;
    }
    else
    {
      auto port = std::dynamic_pointer_cast<Output<T>>(ports[port_index]);
      return port ? port->get_value_ref() : nullptr;
    }
  }

  // Set input data on a specific port index
  void set_input_data(std::shared_ptr<BaseData> data, int port_index)
  {
    if (port_index < 0 || port_index >= static_cast<int>(inputs.size()))
      throw std::out_of_range("Invalid input port index");

    inputs[port_index]->set_data(std::move(data));
  }

  template <typename T>
  void set_value(const std::string &port_label, T new_value)
  {
    T *p_value = this->get_value_ref<T>(port_label);
    *p_value = new_value;
  }

  void update();

private:
  std::string                        label;
  std::vector<std::shared_ptr<Port>> inputs;
  std::vector<std::shared_ptr<Port>> outputs;
};

} // namespace gnode