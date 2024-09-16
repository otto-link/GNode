/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "gnode/node.hpp"
#include "gnode/logger.hpp"

namespace gnode
{

std::shared_ptr<BaseData> Node::get_base_data(int port_index)
{
  // Range check for the port index
  if (port_index < 0 || port_index >= static_cast<int>(this->ports.size()))
    throw std::out_of_range("Invalid port index");

  return this->ports[port_index]->get_data_shared_ptr_downcasted();
}

std::shared_ptr<BaseData> Node::get_base_data(const std::string &port_label)
{
  for (const auto &port : this->ports)
    if (port->get_label() == port_label)
      return port->get_data_shared_ptr_downcasted();

  // If no matching port is found, throw an exception
  throw std::runtime_error("Port with label '" + port_label + "' not found.");
}

std::string Node::get_data_type(int port_index) const
{
  if (port_index < 0 || port_index >= static_cast<int>(this->ports.size()))
    throw std::out_of_range("Invalid output port index");

  return this->ports[port_index]->get_data_type();
}

int Node::get_nports() const { return static_cast<int>(this->ports.size()); }

int Node::get_nports(PortType port_type) const
{
  int count = 0;
  for (auto &port : this->ports)
    if (port->get_port_type() == port_type) count++;
  return count;
}

std::shared_ptr<BaseData> Node::get_output_data(int port_index) const
{
  if (port_index < 0 || port_index >= static_cast<int>(this->ports.size()))
    throw std::out_of_range("Invalid output port index");

  if (this->ports[port_index]->get_port_type() != PortType::OUT)
    throw std::invalid_argument("Invalid port type, should be an output");

  return this->ports[port_index]->get_data_shared_ptr_downcasted();
}

int Node::get_port_index(const std::string &port_label) const
{
  for (size_t i = 0; i < this->ports.size(); ++i)
    if (this->ports[i]->get_label() == port_label) return (int)i;

  throw std::runtime_error("Port label '" + port_label + "' not found.");
}

std::string Node::get_port_label(int port_index) const
{
  if (port_index < 0 || port_index >= static_cast<int>(this->ports.size()))
    throw std::out_of_range("Port index is out of range.");

  return this->ports[port_index]->get_label();
}

PortType Node::get_port_type(const std::string &port_label) const
{
  return this->ports.at(this->get_port_index(port_label))->get_port_type();
}

void Node::set_input_data(std::shared_ptr<BaseData> data, int port_index)
{
  if (port_index < 0 || port_index >= static_cast<int>(this->ports.size()))
    throw std::out_of_range("Invalid input port index");

  if (this->ports[port_index]->get_port_type() != PortType::IN)
    throw std::invalid_argument("Invalid port type, should be an input");

  this->ports[port_index]->set_data(std::move(data));
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
