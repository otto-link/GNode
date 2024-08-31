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

#include "gnode/data.hpp"

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

  // to force compute to be defined
  virtual void compute() = 0;

  std::string get_label() const { return this->label; }

  virtual std::shared_ptr<Data> get_output_data(int /* port_index */)
  {
    return std::shared_ptr<Data>();
  };

  virtual void set_input_data(std::shared_ptr<Data> /* data */,
                              int /* port_index */)
  {
  }

  void update();

private:
  std::string label = "";
};

} // namespace gnode