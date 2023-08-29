/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "gnode.hpp"

namespace gnode
{

Node::Node(){};

Node::Node(std::string id) : id(id)
{
  this->label = id_to_label(this->id);
  this->hash_id = id_to_hash(id);
};

//----------------------------------------
// getters / setters
//----------------------------------------

std::string Node::get_category() { return this->category; }

std::string Node::get_node_type()
{
  if (this->node_type == "")
    LOG_INFO("Node type not defined for node [%s])", this->id.c_str());
  return this->node_type;
};

int Node::get_nports_by_direction(int  direction,
                                  bool skip_optional,
                                  bool skip_unconnected)
{
  int n = 0;
  for (auto &[key, p] : this->ports)
    if ((p.direction == direction) & !(skip_optional & p.is_optional))
      if (p.is_connected || !skip_unconnected)
        n++;
  return n;
}

void *Node::get_p_data(const std::string port_id)
{
  return this->get_port_ref_by_id(port_id)->get_p_data();
}

std::map<std::string, Port> Node::get_ports() { return this->ports; }

Port *Node::get_port_ref_by_id(const std::string port_id)
{
  if (this->is_port_id_in_keys(port_id))
    return &this->ports[port_id];
  else
  {
    LOG_ERROR("port id [%s] not found", port_id.c_str());
    throw std::runtime_error("unknown port id");
  }
}

bool Node::get_thru() { return this->thru; }

void Node::set_p_data(const std::string port_id, void *new_p_data)
{
  this->get_port_ref_by_id(port_id)->set_p_data(new_p_data);
}

void Node::set_post_update_callback(
    std::function<void(Node *)> new_post_update_callback)
{
  this->post_update_callback = new_post_update_callback;
}

void Node::set_thru(bool new_thru)
{
  if (this->thru != new_thru)
  {
    this->thru = new_thru;
    this->update_inner_bindings();
  }
}

//----------------------------------------
// ports management
//----------------------------------------

void Node::add_port(const Port port)
{
  if (!this->is_port_id_in_keys(port.id))
  {
    this->ports[port.id] = port;
    this->ports[port.id].hash_id = id_to_hash(this->id + port.id);
    this->ports[port.id].p_node = this;
  }
  else
  {
    LOG_ERROR("port id [%s] already used", port.id.c_str());
    throw std::runtime_error("node id already used");
  }
}

bool Node::are_inputs_ready() const
{
  bool ret = true;
  for (auto &[key, p] : this->ports)
    if (p.direction == direction::in)
      ret = ret & (p.is_connected | p.is_optional);
  return ret;
}

bool Node::are_inputs_up_to_date() const
{
  bool ret = true;
  // check that the nodes "upstream", providing the input data have
  // been updated (NB: this only checks the port that are connected)
  for (auto &[key, p] : this->ports)
    if ((p.direction == direction::in) & p.is_connected)
      ret = ret & p.p_linked_node->is_up_to_date;
  return ret;
}

void Node::connect_port_in(std::string port_id,
                           Node       *p_linked_node,
                           Port       *p_linked_port,
                           void       *ptr_to_incoming_data)
{
  this->get_port_ref_by_id(port_id)->connect_in(p_linked_node,
                                                p_linked_port,
                                                ptr_to_incoming_data);
  this->update_inner_bindings();
}

void Node::connect_port_out(std::string port_id,
                            Node       *p_linked_node,
                            Port       *p_linked_port)
{
  this->get_port_ref_by_id(port_id)->connect_out(p_linked_node, p_linked_port);
  this->update_inner_bindings();
}

void Node::disconnect_port(std::string port_id)
{
  this->get_port_ref_by_id(port_id)->disconnect();
  this->update_inner_bindings();
}

void Node::disconnect_all_ports()
{
  LOG_DEBUG("node [%s], disconnecting all ports", this->id.c_str());
  for (auto &[key, p] : this->ports)
    p.disconnect();
}

bool Node::is_port_id_in_keys(const std::string port_id)
{
  return this->ports.contains(port_id);
}

void Node::remove_port(const std::string port_id) { ports.erase(port_id); }

void Node::update_links()
{
  // update data pointer on this other end of the link
  for (auto &[key, port] : this->ports)
    if ((port.direction == direction::out) & port.is_connected)
      // 'true' to connect even if it is already connected
      port.p_linked_port->connect_in(this, &port, port.get_p_data(), true);
}

//----------------------------------------
// computing
//----------------------------------------

void Node::force_update()
{
  this->is_up_to_date = false;
  if (this->auto_update)
    this->update();
}

void Node::update()
{
  if (!this->is_up_to_date)
  {
    if (!this->are_inputs_ready() | !this->are_inputs_up_to_date())
      LOG_INFO("node id [%s] cannot compute: inputs not up-to-date or set up",
               this->id.c_str());
    else
    {
      if (this->frozen_outputs)
      {
        // just update the current node and do not propagate update
        LOG_DEBUG("output frozen");
        this->compute();
        if (this->post_update_callback)
          this->post_update_callback(this);
        this->is_up_to_date = true;
      }
      else
      {
        // update upstream nodes first (only if the node is 'thru'
        // and does not store data, so that it needs the data
        // upstream to be updated), update current node, then update
        // downstream nodes
        this->is_up_to_date = true;

        if (this->thru)
          for (auto &[key, p] : this->ports)
            if ((p.direction == direction::in) & p.is_connected)
            {
              LOG_DEBUG("node [%s] triggers node [%s]",
                        this->id.c_str(),
                        p.p_linked_node->id.c_str());
              p.p_linked_node->update();
            }

        this->compute();
        if (this->post_update_callback)
          this->post_update_callback(this);

        for (auto &[key, p] : this->ports)
          if ((p.direction == direction::out) & p.is_connected)
          {
            LOG_DEBUG("node [%s] triggers node [%s]",
                      this->id.c_str(),
                      p.p_linked_node->id.c_str());
            p.p_linked_node->is_up_to_date = false;
            p.p_linked_node->update();
          }
      }
    }
  }
  else
    LOG_DEBUG("node [%s] is up-to-date", this->id.c_str());
}

//----------------------------------------
// displaying infos
//----------------------------------------

void Node::infos()
{
  LOG_INFO("node infos");
  LOG_INFO("- id: %s", this->id.c_str());
  LOG_INFO("- label: %s", this->label.c_str());
  LOG_INFO("- hash_id: %d", this->hash_id);
  LOG_INFO("- category: %s", this->category.c_str());
  LOG_INFO("- node_type: %s", this->node_type.c_str());
  LOG_INFO("- frozen_outputs: %d", this->frozen_outputs);
  LOG_INFO("- thru: %d", this->thru);
  LOG_INFO("- state");
  LOG_INFO("  - are_inputs_ready: %d", are_inputs_ready());
  LOG_INFO("- ports");
  for (auto &[key, p] : this->ports)
    p.infos();
}

void Node::print_links()
{
  std::string link_dir = "";
  std::string orient = "";

  for (auto &[key, p] : this->ports)
  {
    if (p.direction == direction::out)
    {
      orient = "out";
      link_dir = "==>";
    }
    else
    {
      orient = "in ";
      link_dir = "<==";
    }

    if (p.is_connected)
    {
      std::cout << " - " << orient << " [" << this->id.c_str() << "] [" << p.id
                << "] " << link_dir << " [" << p.p_linked_node->id.c_str()
                << "] [" << p.p_linked_port->id.c_str() << "]" << std::endl;
    }
    else
      std::cout << " - " << orient << " [" << this->id.c_str() << "] [" << p.id
                << "] . " << std::endl;
  }
}

void Node::treeview()
{
  std::cout << "node (id: " << this->id.c_str() << ")" << std::endl;

  std::cout << "  + inputs " << std::endl;
  for (auto &[key, p] : this->ports)
    if (p.direction == direction::in)
    {
      std::cout << "    - " << p.id << ", is_connected: " << p.is_connected
                << ", is_optional: " << p.is_optional << std::endl;
    }

  std::cout << "  + outputs " << std::endl;
  for (auto &[key, p] : this->ports)
    if (p.direction == direction::out)
    {
      std::cout << "    - " << p.id << ", is_connected: " << p.is_connected
                << ", is_optional: " << p.is_optional << std::endl;
    }
}

} // namespace gnode
