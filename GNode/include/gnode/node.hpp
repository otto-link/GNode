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

  // to force compute to be defined
  virtual void compute() = 0;

  std::string get_label() const { return this->label; }

  // Set input data on a specific port index
  void set_input_data(std::shared_ptr<BaseData> data, int port_index)
  {
    if (port_index < 0 || port_index >= static_cast<int>(inputs.size()))
      throw std::out_of_range("Invalid input port index");

    inputs[port_index]->set_data(std::move(data));
  }

  // Get output data from a specific port index
  std::shared_ptr<BaseData> get_output_data(int port_index) const
  {
    if (port_index < 0 || port_index >= static_cast<int>(outputs.size()))
      throw std::out_of_range("Invalid output port index");

    return outputs[port_index]->get_data_shared_ptr_downcasted();
  }

  // Get a pointer to the input value at a specific port index
  template <typename T> T *get_input_value_ref(int port_index) const
  {
    if (port_index < 0 || port_index >= static_cast<int>(inputs.size()))
      throw std::out_of_range("Invalid input port index");

    auto input = std::dynamic_pointer_cast<Input<T>>(inputs[port_index]);
    return input ? input->get_value_ref() : nullptr;
  }

  // Get a pointer to the output value at a specific port index
  template <typename T> T *get_output_value_ref(int port_index) const
  {
    if (port_index < 0 || port_index >= static_cast<int>(outputs.size()))
      throw std::out_of_range("Invalid output port index");

    auto output = std::dynamic_pointer_cast<Output<T>>(outputs[port_index]);
    return output ? output->get_value_ref() : nullptr;
  }

  void update();

protected:
  std::vector<std::shared_ptr<Port>> inputs;
  std::vector<std::shared_ptr<Port>> outputs;

private:
  std::string label;
};

} // namespace gnode