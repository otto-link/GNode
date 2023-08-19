/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <fstream>

#include "nodesoup.hpp"

#include "gnode.hpp"

namespace gnode
{

Tree::Tree(std::string id) : id(id) { this->label = id_to_label(this->id); }

//----------------------------------------
// nodes management
//----------------------------------------

void Tree::add_node(std::shared_ptr<Node> p_node)
{
  if (!this->is_node_id_in_keys(p_node.get()->id))
    this->nodes_map[p_node.get()->id] = p_node;
  else
  {
    LOG_ERROR("node [%s] already used", p_node.get()->id.c_str());
    throw std::runtime_error("node id alreayd used");
  }
}

std::vector<gnode::Point> Tree::compute_graph_layout(int iterations, float k)
{
  std::vector<std::vector<size_t>> g = this->get_adjacency_matrix();

  std::vector<nodesoup::Point2D> positions = nodesoup::fruchterman_reingold(
      this->get_adjacency_matrix(),
      1,
      1,
      (double)k,
      iterations);

  // convert to gnode::Point2d
  std::vector<gnode::Point> pts(positions.size());

  for (size_t k = 0; k < positions.size(); k++)
  {
    pts[k].x = positions[k].x;
    pts[k].y = positions[k].y;
  }

  return pts;
}

std::vector<std::vector<size_t>> Tree::get_adjacency_matrix()
{
  // simple adjacency list graph structure (see nodesoup for
  // example https://github.com/olvb/nodesoup)
  std::vector<std::vector<size_t>> g = {};

  // for each node scan outputs to seek connections with other nodes
  for (auto &[id, node] : this->get_nodes_map())
  {
    g.push_back({});
    for (auto &[port_id, port] : node.get()->get_ports())
    {
      if (port.is_connected & (port.direction == direction::out))
      {
        std::string linked_node_id = port.p_linked_node->id;
        size_t      node_idx = 0;
        for (auto &[search_id, search_node] : this->get_nodes_map())
        {
          if (linked_node_id == search_id)
            break;
          node_idx++;
        }
        g.back().push_back(node_idx);
      }
    }
  }

  return g;
}

std::string Tree::get_node_id_by_hash_id(int node_hash_id)
{
  gnode::Node *p_node = this->get_node_ref_by_hash_id(node_hash_id);
  return p_node->id;
}

Node *Tree::get_node_ref_by_hash_id(int node_hash_id)
{
  Node *p_node = nullptr;

  // scan control nodes and their ports to find the
  for (auto &[id, node] : this->get_nodes_map())
    if (node.get()->hash_id == node_hash_id)
    {
      p_node = node.get();
      break;
    }

  if (!p_node)
  {
    LOG_ERROR("node hash id [%d] is not known", node_hash_id);
    throw std::runtime_error("unknonw node hash_id");
  }

  return p_node;
}

std::shared_ptr<Node> Tree::get_node_sptr_by_id(const std::string node_id)
{
  if (this->is_node_id_in_keys(node_id))
    return this->nodes_map[node_id];
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

std::map<std::string, std::shared_ptr<Node>> Tree::get_nodes_map()
{
  return nodes_map;
}

void Tree::get_ids_by_port_hash_id(int          port_hash_id,
                                   std::string &node_id,
                                   std::string &port_id)
{
  // scan control nodes and their ports to find the
  for (auto &[nid, node] : this->get_nodes_map())
    for (auto &[pid, port] : node.get()->get_ports())
      if (port.hash_id == port_hash_id)
      {
        node_id = nid;
        port_id = pid;
        break;
      }
}

bool Tree::is_node_id_in_keys(const std::string node_id)
{
  return this->nodes_map.contains(node_id);
}

void Tree::remove_node(std::string node_id)
{
  LOG_DEBUG("removing node [%s]", node_id.c_str());
  if (this->is_node_id_in_keys(node_id))
  {
    this->get_node_ref_by_id(node_id)->disconnect_all_ports();
    this->nodes_map.erase(node_id);
  }
  else
  {
    LOG_ERROR("node [%s] not found", node_id.c_str());
    throw std::runtime_error("unknown node id");
  }
}

size_t Tree::size() { return this->get_nodes_map().size(); }

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
  for (auto &[key, n] : this->nodes_map)
    n->is_up_to_date = false;

  // trigger nodes with no inputs
  for (auto &[key, n] : this->nodes_map)
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
  for (auto &[key, n] : this->nodes_map)
    n.get()->infos();
}

void Tree::print_node_links()
{
  std::cout << "tree node links" << std::endl;
  for (auto &[key, n] : this->nodes_map)
    n.get()->print_links();
}

void Tree::print_node_list()
{
  std::cout << "tree nodes" << std::endl;
  for (auto &[key, n] : this->nodes_map)
    std::cout << " - " << key << ": " << n.get()->id << std::endl;
}

void Tree::export_csv(std::string fname_nodes, std::string fname_links)
{
  std::fstream f;

  f.open(fname_nodes, std::ios::out);
  for (auto &[kn, n] : this->nodes_map)
    for (auto &[kp, p] : n.get()->get_ports())
      f << n.get()->id.c_str() << "," << p.direction << "," << p.id.c_str()
        << "\n";
  f.close();

  f.open(fname_links, std::ios::out);

  for (auto &[kn, n] : this->nodes_map)
    for (auto &[kp, p] : n.get()->get_ports())
      if (p.is_connected & (p.direction == direction::out))
        f << n.get()->id.c_str() << "," << p.id.c_str() << ","
          << p.p_linked_node->id.c_str() << "," << p.p_linked_port->id.c_str()
          << "\n";

  f.close();
}

} // namespace gnode
