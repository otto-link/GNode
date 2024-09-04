/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "gnode/node.hpp"
#include "gnode/logger.hpp"

namespace gnode
{

int Node::get_nports(PortType port_type) const
{
  const auto &ports = (port_type == PortType::IN) ? this->inputs
                                                  : this->outputs;
  return static_cast<int>(ports.size());
}

std::shared_ptr<BaseData> Node::get_output_data(int port_index) const
{
  if (port_index < 0 || port_index >= static_cast<int>(outputs.size()))
    throw std::out_of_range("Invalid output port index");

  return outputs[port_index]->get_data_shared_ptr_downcasted();
}

std::pair<int, PortType> Node::get_port_info_by_label(
    const std::string &port_label) const
{
  for (int i = 0; i < static_cast<int>(inputs.size()); ++i)
    if (inputs[i]->get_label() == port_label) return {i, PortType::IN};

  for (int i = 0; i < static_cast<int>(outputs.size()); ++i)
    if (outputs[i]->get_label() == port_label) return {i, PortType::OUT};

  throw std::runtime_error("Port with label '" + port_label + "' not found.");
}

std::string Node::get_port_label(PortType port_type, int port_index) const
{
  const auto &ports = (port_type == PortType::IN) ? this->inputs
                                                  : this->outputs;

  if (port_index < 0 || port_index >= static_cast<int>(ports.size()))
    throw std::out_of_range("Port index is out of range.");

  return ports[port_index]->get_label();
}

void Node::set_input_data(std::shared_ptr<BaseData> data, int port_index)
{
  if (port_index < 0 || port_index >= static_cast<int>(inputs.size()))
    throw std::out_of_range("Invalid input port index");

  inputs[port_index]->set_data(std::move(data));
}

void Node::update()
{
  if (this->is_dirty)
  {
    this->compute();
    this->is_dirty = false;
  }
}

} // namespace gnode
