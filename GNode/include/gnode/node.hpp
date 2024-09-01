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

  Node(){};

  Node(std::string label) : label(label){};

  virtual ~Node() = default;

  // to force compute to be defined
  virtual void compute() = 0;

  std::string get_label() const { return this->label; }

  void set_input_data(std::shared_ptr<BaseData> data, int port_index)
  {
    this->inputs[port_index]->set_data(data);
  }

  std::shared_ptr<BaseData> get_output_data(int port_index)
  {
    return this->outputs[port_index]->get_data_shared_ptr_downcasted();
  }

  template <typename T> T *get_input_value_ref(int port_index)
  {
    return std::dynamic_pointer_cast<Input<T>>(this->inputs[port_index])
        ->get_value_ref();
  }

  template <typename T> T *get_output_value_ref(int port_index)
  {
    return std::dynamic_pointer_cast<Output<T>>(this->outputs[port_index])
        ->get_value_ref();
  }

  void update();

protected:
  std::vector<std::shared_ptr<Port>> inputs;
  std::vector<std::shared_ptr<Port>> outputs;

private:
  std::string label = "";
};

} // namespace gnode