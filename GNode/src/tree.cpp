/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <fstream>

#include "interface.hpp"
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
  {
    this->nodes_map[p_node.get()->id] = p_node;
    p_node->p_tree = this;
  }
  else
  {
    LOG_ERROR("node [%s] already used", p_node.get()->id.c_str());
    throw std::runtime_error("node id alreayd used");
  }
}

std::vector<gnode::Point> Tree::compute_graph_layout_fruchterman_reingold(
    int   iterations,
    float k)
{
  std::vector<std::vector<size_t>> g = this->get_adjacency_list();

  std::vector<nodesoup::Point2D> positions = nodesoup::fruchterman_reingold(
      this->get_adjacency_list(),
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

std::vector<gnode::Point> Tree::compute_graph_layout_sugiyama()
{
  std::vector<gnode::Point> pts(this->size()); // output

  std::vector<std::vector<size_t>> adj = this->get_adjacency_list();

  graph_builder gb = graph_builder();

  for (size_t i = 0; i < adj.size(); i++)
    for (auto &j : adj[i])
      gb.add_edge(i, j);

  graph graph = gb.build();

  attributes attr;
  attr.node_size = 0;
  attr.node_dist = 1;
  attr.layer_dist = 1;

  sugiyama_layout layout(graph, attr);

  int k = 0;
  for (auto &vertex : layout.vertices())
  {
    // reverse x and y to get an horizontal layout by default
    pts[k] = Point(vertex.pos.y - 0.5f * layout.height(),
                   vertex.pos.x - 0.5f * layout.width());
    k++;
  }

  return pts;
}

std::vector<std::vector<size_t>> Tree::get_adjacency_list()
{
  // simple adjacency list graph structure
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

std::shared_ptr<Node> Tree::get_node_sptr_by_id(const std::string node_id) const
{
  if (this->is_node_id_in_keys(node_id))
    return this->get_nodes_map().at(node_id);
  else
  {
    LOG_ERROR("node [%s] not found", node_id.c_str());
    throw std::runtime_error("unknown node id");
  }
}

std::map<std::string, std::shared_ptr<Node>> Tree::get_nodes_map() const
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

bool Tree::is_node_id_in_keys(const std::string node_id) const
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

void Tree::remove_all_nodes() { this->nodes_map.clear(); }

// helper, not a Tree method
bool is_subgraph_cyclic(size_t                                  i,
                        std::vector<bool>                      &visited,
                        std::vector<bool>                      &stack,
                        const std::vector<std::vector<size_t>> &adj)
{
  if (!visited[i])
  {
    // Mark the current node as visited and part of recursion stack
    visited[i] = true;
    stack[i] = true;

    for (auto &k : adj[i])
      if (!visited[k] && is_subgraph_cyclic(k, visited, stack, adj))
        return true;
      else if (stack[k])
        return true;
  }

  // Remove the vertex from recursion stack
  stack[i] = false;
  return false;
}

bool is_graph_cyclic(const std::vector<std::vector<size_t>> &adj)
{
  // https://www.geeksforgeeks.org/detect-cycle-in-a-graph/
  std::vector<bool> visited(adj.size());
  std::vector<bool> stack(adj.size());

  for (size_t i = 0; i < adj.size(); i++)
    if ((!visited[i]) and is_subgraph_cyclic(i, visited, stack, adj))
      return true;

  return false;
}

bool Tree::is_cyclic() { return is_graph_cyclic(this->get_adjacency_list()); }

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
    if (n.get()->get_nports_by_direction(direction::in) == 0)
      n.get()->update_and_propagate();

  this->post_update();
}

void Tree::update_node(std::string node_id)
{
  LOG_INFO("updating tree [%s] from node [%s]...",
           this->id.c_str(),
           node_id.c_str());

  // --- first check that the nodes upstream are up to date, if not
  // --- there is no need to perform the update
  bool upstream_ok = true;

  for (auto &[port_id, port] : this->get_node_ref_by_id(node_id)->get_ports())
    if ((port.direction == direction::in) && port.is_connected &&
        (!port.is_optional))
      upstream_ok &= port.p_linked_node->is_up_to_date;

  if (upstream_ok)
  {
    // --- use a depth-first search to mark the nodes that need to be
    // --- updated (and use Sugiyama layered graph layout to determine
    // --- update order)
    std::vector<std::string>     stack = {node_id};
    std::map<std::string, bool>  is_discovered = {};
    std::map<std::string, float> node_layer = {};
    std::vector<gnode::Point>    pts = this->compute_graph_layout_sugiyama();

    {
      int k = 0;
      for (auto &[key, n] : this->nodes_map)
      {
        is_discovered[key] = false;
        node_layer[key] = pts[k].x;
        k++;
      }
    }

    // storage for the update procedure coming after
    std::vector<std::string> update_queue = {};

    while (stack.size())
    {
      std::string nid = stack.back();
      stack.pop_back();

      Node *p_node = this->get_node_ref_by_id(nid);

      // store and mark for update, but add the node before or after
      // the last added node based on their relative layer level
      if (update_queue.size() > 0 &&
          node_layer[update_queue.back()] >= node_layer[nid])
        update_queue.insert(update_queue.end() - 1, nid);
      else
        update_queue.push_back(nid);

      LOG_DEBUG("QUEUE");
      for (auto &s : update_queue)
        LOG_DEBUG("%s", s.c_str());

      p_node->is_up_to_date = false;

      if (this->get_node_ref_by_id(nid)->are_inputs_ready())
        if (!is_discovered[nid])
        {
          is_discovered[nid] = true;

          // add 'downstream' nodes to the DFS stack
          for (auto &[key, p] : p_node->get_ports())
            if (p.direction == direction::out && p.is_connected &&
                !(p_node->frozen_outputs))
              stack.push_back(p.p_linked_node->id);
        }
    }

    // --- update the nodes (TODO: optimize?)
    while (update_queue.size())
    {
      std::string nid = update_queue.front();
      update_queue.erase(update_queue.begin());

      LOG_DEBUG("UPDATE: %s", nid.c_str());
      this->get_node_ref_by_id(nid)->update();
    }

    this->post_update();
  }
  else
    LOG_DEBUG("no update from node [%], tree not ready", node_id.c_str());
}

void Tree::post_update() { LOG_DEBUG("empty Tree::post_update"); }

//----------------------------------------
// displaying infos
//----------------------------------------

void Tree::export_flowchart_graphviz(std::string fname)
{
  // to convert, command line: dot export.dot -Tsvg > output.svg
  LOG_INFO("graphviz export");

  std::fstream f;
  f.open(fname, std::ios::out);

  f << "digraph root {" << std::endl;
  f << "label=" << this->label << ";" << std::endl;
  f << "labelloc=\"t\"" << std::endl;
  f << "rankdir=TD;" << std::endl;
  f << "ranksep=0.5;" << std::endl;

  for (auto &[key, n] : this->nodes_map)
    for (auto &[key_p, p] : n->get_ports())
      if (p.direction == direction::out && p.is_connected)
        f << "\"" << n->id << "\" -> \"" << p.p_linked_node->id
          << "\" [fontsize=8, label=\"" << p.label << " - "
          << p.p_linked_port->label << "\"]" << std::endl;

  f << "}" << std::endl;

  f.close();
}

void Tree::export_flowchart_mermaid(std::string fname)
{
  LOG_INFO("mermaid export");

  std::fstream f;
  f.open(fname, std::ios::out);

  f << "---" << std::endl;
  f << "title: " << this->label << std::endl;
  f << "---" << std::endl;
  f << "flowchart LR" << std::endl;

  // nodes
  for (auto &[key, n] : this->nodes_map)
    f << "    " << key << "([" << n->label << "])" << std::endl;

  // links
  for (auto &[key, n] : this->nodes_map)
    for (auto &[key_p, p] : n->get_ports())
      if (p.direction == direction::out && p.is_connected)
        f << "    " << key << " --> |" << p.label << " / "
          << p.p_linked_port->label << "| " << p.p_linked_node->id << std::endl;

  f.close();
}

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
