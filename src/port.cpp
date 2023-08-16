/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "gnode.hpp"

namespace gnode
{

Port::Port(){};

Port::Port(std::string id, int direction, int dtype)
    : id(id), direction(direction), dtype(dtype)
{
  this->label = id_to_label(this->id);
}

Port::Port(std::string id, int direction, int dtype, bool is_optional)
    : id(id), direction(direction), dtype(dtype), is_optional(is_optional)
{
  this->label = id_to_label(this->id);
}

void *Port::get_p_data()
{
  if (!p_data)
    LOG_DEBUG("warning: null pointer, port id [%s]", this->id.c_str());
  return this->p_data;
}

void Port::set_p_data(void *new_p_data) { this->p_data = new_p_data; }

void Port::connect_in(Node *p_linked_node,
                      Port *p_linked_port,
                      void *ptr_to_incoming_data)
{
  if (this->is_connected)
  {
    LOG_ERROR("port id [%s] is already connected", this->id.c_str());
    throw std::runtime_error("port already connected");
  }

  if (this->direction == direction::in)
  {
    this->set_p_data(ptr_to_incoming_data);
    this->p_linked_node = p_linked_node;
    this->p_linked_port = p_linked_port;
    this->is_connected = true;
  }
  else
  {
    LOG_ERROR("port direction mismatch while linking: port id [%s] is not "
              "an input",
              this->id.c_str());
    throw std::runtime_error("port direction mismatch while linking");
  }
}

void Port::connect_out(Node *p_linked_node, Port *p_linked_port)
{
  if (this->is_connected)
  {
    LOG_ERROR("port id [%s] is already connected", this->id.c_str());
    throw std::runtime_error("port already connected");
  }

  if (this->direction == direction::out)
  {
    this->p_linked_node = p_linked_node;
    this->p_linked_port = p_linked_port;
    this->is_connected = true;
  }
  else
  {
    LOG_ERROR("port direction mismatch while linking: port id [%s] is not "
              "an output",
              this->id.c_str());
    throw std::runtime_error("port direction mismatch while linking");
  }
}

void Port::disconnect()
{
  // disconnect both ends of the link
  if (this->p_linked_port)
  {
    LOG_DEBUG("linked node / port: [%s] / [%s]",
              this->p_linked_node->id.c_str(),
              this->p_linked_port->id.c_str());
    this->p_linked_port->reset();
  }
  this->reset();
}

void Port::reset()
{
  LOG_DEBUG("reset port [%s]", this->id.c_str());
  this->is_connected = false;
  this->p_node = nullptr;
  this->p_linked_node = nullptr;
  this->p_linked_port = nullptr;

  // reset the data only if it is an input port. For output ports,
  // the inner bindings still holds even if the port is not
  // connected
  if (this->direction == direction::in)
    this->set_p_data(nullptr);
}

void Port::infos()
{
  LOG_INFO("port infos");
  LOG_INFO("+ id: %s", this->id.c_str());
  LOG_INFO("+ label: %s", this->label.c_str());
  LOG_INFO("+ hash_id: %d", this->hash_id);
  LOG_INFO("+ direction: %d", this->direction);
  LOG_INFO("+ dtype: %d", this->dtype);
  LOG_INFO("+ is_connected: %d", this->is_connected);
  LOG_INFO("+ is_optional: %d", this->is_optional);
  LOG_INFO("+ p_data: %p", this->p_data);
}

} // namespace gnode
