/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <fstream>

#include "gnode.hpp"

namespace gnode
{

Tree::Tree(std::string id) : id(id) { this->label = id_to_label(this->id); }

//----------------------------------------
// nodes management
//----------------------------------------

void Tree::add_node(std::shared_ptr<Node> p_node)
{
  this->p_nodes[p_node.get()->id] = p_node;
}

std::shared_ptr<Node> Tree::get_node_sptr_by_id(const std::string node_id)
{
  if (this->is_node_id_in_keys(node_id))
    return this->p_nodes[node_id];
  else
  {
    LOG_ERROR("node [%s] not found", node_id.c_str());
    throw std::runtime_error("unknown node id");
  }
}

Node *Tree::get_node_ref_by_id(const std::string node_id)
{
  return this->get_node_sptr_by_id(node_id).get();
}

std::map<std::string, std::shared_ptr<Node>> Tree::get_p_nodes()
{
  return p_nodes;
}

bool Tree::is_node_id_in_keys(const std::string node_id)
{
  return this->p_nodes.contains(node_id);
}

void Tree::remove_node(std::string node_id)
{
  LOG_DEBUG("removing node [%s]", node_id.c_str());
  if (this->is_node_id_in_keys(node_id))
  {
    this->get_node_ref_by_id(node_id)->disconnect_all_ports();
    this->p_nodes.erase(node_id);
  }
  else
  {
    LOG_ERROR("node [%s] not found", node_id.c_str());
    throw std::runtime_error("unknown node id");
  }
}

//----------------------------------------
// linking
//----------------------------------------

void Tree::link(const std::string node_id_from, // out
                const std::string port_id_from,
                const std::string node_id_to, // in
                const std::string port_id_to)
{
  LOG_DEBUG("connecting [%s]/[%s] to [%s]/[%s]",
            node_id_from.c_str(),
            port_id_from.c_str(),
            node_id_to.c_str(),
            port_id_to.c_str());

  Node *p_node_from = this->get_node_ref_by_id(node_id_from);
  Node *p_node_to = this->get_node_ref_by_id(node_id_to);

  Port *p_port_from = p_node_from->get_port_ref_by_id(port_id_from);
  Port *p_port_to = p_node_to->get_port_ref_by_id(port_id_to);

  // TODO check if output or input are already connected and
  // disconnect if needed

  void *ptr = p_node_from->get_p_data(port_id_from);
  p_node_from->connect_port_out(port_id_from, p_node_to, p_port_to);
  p_node_to->connect_port_in(port_id_to, p_node_from, p_port_from, ptr);
}

void Tree::unlink(const std::string node_id_from, // out
                  const std::string port_id_from,
                  const std::string node_id_to, // in
                  const std::string port_id_to)
{
  LOG_DEBUG("unlinking: [%s] / [%s] ==> [%s] / [%s]",
            node_id_from.c_str(),
            port_id_from.c_str(),
            node_id_to.c_str(),
            port_id_to.c_str());

  this->get_node_ref_by_id(node_id_from)->disconnect_port(port_id_from);
  this->get_node_ref_by_id(node_id_to)->disconnect_port(port_id_to);
}

//----------------------------------------
// computing
//----------------------------------------

void Tree::update()
{
  LOG_INFO("updating tree [%s]...", this->id.c_str());

  // set all the nodes to a "to be updated" state
  for (auto &[key, n] : this->p_nodes)
    n->is_up_to_date = false;

  // trigger nodes with no inputs

  // TODO fix: actually first trigger the nodes with no inputs, or
  // only unconnected optional inputs
  for (auto &[key, n] : this->p_nodes)
  {
    LOG_DEBUG("[%s] ninputs: %d",
              key.c_str(),
              n.get()->get_nports_by_direction(direction::in));
    if (n.get()->get_nports_by_direction(direction::in) == 0)
      n.get()->update();
  }
}

void Tree::update_node(std::string node_id)
{
  LOG_INFO("updating tree [%s] from node [%s]...",
           this->id.c_str(),
           node_id.c_str());

  this->get_node_ref_by_id(node_id)->is_up_to_date = false;
  this->get_node_ref_by_id(node_id)->update();
}

//----------------------------------------
// displaying infos
//----------------------------------------

void Tree::infos()
{
  LOG_INFO("tree infos");
  LOG_INFO("- label: %s", this->label.c_str());
  for (auto &[key, n] : this->p_nodes)
    n.get()->infos();
}

void Tree::print_node_links()
{
  std::cout << "tree node links" << std::endl;
  for (auto &[key, n] : this->p_nodes)
    n.get()->print_links();
}

void Tree::print_node_list()
{
  std::cout << "tree nodes" << std::endl;
  for (auto &[key, n] : this->p_nodes)
    std::cout << " - " << key << ": " << n.get()->id << std::endl;
}

void Tree::export_csv(std::string fname_nodes, std::string fname_links)
{
  std::fstream f;

  f.open(fname_nodes, std::ios::out);
  for (auto &[kn, n] : this->p_nodes)
    for (auto &[kp, p] : n.get()->get_ports())
      f << n.get()->id.c_str() << "," << p.direction << "," << p.id.c_str()
        << "\n";
  f.close();

  f.open(fname_links, std::ios::out);

  for (auto &[kn, n] : this->p_nodes)
    for (auto &[kp, p] : n.get()->get_ports())
      if (p.is_connected & (p.direction == direction::out))
        f << n.get()->id.c_str() << "," << p.id.c_str() << ","
          << p.p_linked_node->id.c_str() << "," << p.p_linked_port->id.c_str()
          << "\n";

  f.close();
}

} // namespace gnode
