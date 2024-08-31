/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <algorithm>
#include <fstream>
#include <iostream>

#include "demekgraph/updated/include/graph.hpp"
#include "demekgraph/updated/include/layout.hpp"
#include "macrologger.h"

#include "gnode/graph.hpp"

namespace gnode
{

std::string Graph::add_node(std::shared_ptr<Node> p_node, std::string id)
{
  // use node pointer as id if none is provided
  if (id == "") id = std::to_string((unsigned long long)(void **)p_node.get());

  if (this->is_node_id_available(id))
    this->nodes[id] = p_node;
  else
    throw std::runtime_error("node id already used");

  return id;
}

std::vector<Point> Graph::compute_graph_layout_sugiyama()
{
  std::vector<Point> points;

  // create an adjacency list in order to build a dummy graph for the
  // Sugiyama procedure
  std::vector<std::vector<size_t>> adj = {}; // this->get_adjacency_list();

  // first build up a node id to node index correspondence
  std::map<std::string, size_t> node_idx = {};
  int                           idx = 0;
  for (auto &[nid, p_node] : this->nodes)
    node_idx[nid] = idx++;

  std::map<std::string, std::vector<std::string>> connectivity =
      this->get_connectivity_downstream();

  adj.resize(this->nodes.size());
  for (auto &[nid, to_ids] : connectivity)
    for (auto &to_id : to_ids)
      adj[node_idx[nid]].push_back(node_idx[to_id]);

  // compute Sugiyama layout
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

  for (auto &vertex : layout.vertices())
  {
    // reverse x and y to get an horizontal layout by default
    points.push_back(Point(vertex.pos.y - 0.5f * layout.height(),
                           vertex.pos.x - 0.5f * layout.width()));
  }

  return points;
}

// connect two nodes, note that data types are not verified
bool Graph::connect(std::string from,
                    int         port_from,
                    std::string to,
                    int         port_to)
{
  Link new_link(from, port_from, to, port_to);

  // check if link already exists
  if (std::find(this->links.begin(), this->links.end(), new_link) !=
      this->links.end())
    return false;
  else
  {
    std::shared_ptr<Data> from_data = this->nodes.at(from)->get_output_data(
        port_from);
    this->nodes.at(to)->set_input_data(from_data, port_to);

    this->links.push_back(new_link);
  }

  return true;
}

bool Graph::disconnect(std::string from,
                       int         port_from,
                       std::string to,
                       int         port_to)
{
  Link link(from, port_from, to, port_to);

  if (std::find(this->links.begin(), this->links.end(), link) ==
      this->links.end())
    return false;
  else
  {
    // disconnect nodes
    std::shared_ptr<Data> from_data = std::shared_ptr<Data>();
    this->nodes.at(to)->set_input_data(from_data, port_to);

    // remove the link from storage
    this->links.erase(std::remove(this->links.begin(), this->links.end(), link),
                      this->links.end());

    return true;
  }
}

void Graph::export_to_graphviz(std::string fname, std::string graph_label)
{
  std::fstream f;
  f.open(fname, std::ios::out);

  f << "digraph root {" << std::endl;
  f << "label=\"" << graph_label << "\";" << std::endl;
  f << "labelloc=\"t\";" << std::endl;
  f << "rankdir=TD;" << std::endl;
  f << "ranksep=0.5;" << std::endl;
  f << "node [shape=record];" << std::endl;

  for (auto &[id, p_node] : this->nodes)
    f << id << "[label=\"" << this->nodes[id]->get_label() << "\"]"
      << std::endl;

  std::map<std::string, std::vector<std::string>> connectivity =
      this->get_connectivity_downstream();

  for (auto &[from_id, to_ids] : connectivity)
    for (auto &to_id : to_ids)
      f << from_id << " -> " << to_id << std::endl;

  f << "}" << std::endl;

  f.close();
}

std::map<std::string, std::vector<std::string>> Graph::
    get_connectivity_downstream()
{
  std::map<std::string, std::vector<std::string>> connectivity;

  for (auto &link : this->links)
    connectivity[link.from].push_back(link.to);

  return connectivity;
}

std::map<std::string, std::vector<std::string>> Graph::
    get_connectivity_upstream()
{
  std::map<std::string, std::vector<std::string>> connectivity;

  for (auto &link : this->links)
    connectivity[link.to].push_back(link.from);

  return connectivity;
}

bool Graph::is_node_id_available(std::string id)
{
  return !this->nodes.contains(id);
}

void Graph::print()
{
  std::cout << "Nodes:" << std::endl;
  for (auto &[id, p_node] : this->nodes)
  {
    std::cout << std::boolalpha;
    std::cout << "id: " << id << ", label: " << p_node->get_label()
              << ", is_dirty: " << p_node->is_dirty << std::endl;
  }

  std::cout << "Links:" << std::endl;
  for (auto &link : this->links)
    link.print();
}

void Graph::remove_node(std::string id)
{
  if (this->is_node_id_available(id))
    throw std::runtime_error("unknown node id");
  else
  {
    // disconnect node
    for (auto &link : this->links)
      if (link.from == id)
        this->nodes.at(link.to)->set_input_data(std::shared_ptr<Data>(),
                                                link.port_to);

    // remove links
    this->links.erase(std::remove_if(this->links.begin(),
                                     this->links.end(),
                                     [&id](Link link) {
                                       return link.from == id || link.to == id;
                                     }),
                      this->links.end());

    // remove node
    this->nodes.erase(id);
  }
}

void Graph::update()
{
  // set all the nodes to a "dirty" state
  for (auto &[nid, p_node] : this->nodes)
    p_node->is_dirty = true;

  // trigger nodes with no inputs
  std::map<std::string, std::vector<std::string>> connectivity_up =
      this->get_connectivity_upstream();

  for (auto &[nid, up_ids] : connectivity_up)
    if (up_ids.size() == 0) this->update_node(nid);

  this->post_update();
}

bool Graph::update_node(std::string id)
{
  // --- first check that all the nodes upstream the node to be
  // --- updated are up to date (i.e. no 'dirty'), if not, no need to
  // --- update the node
  std::map<std::string, std::vector<std::string>> connectivity_up =
      this->get_connectivity_upstream();

  for (auto &id_up : connectivity_up[id])
    if (this->get_node_ref_by_id(id_up)->is_dirty)
    {
      LOG_DEBUG("no update of the graph");
      return false;
    }

  // --- use a depth-first search to mark the nodes that need to be
  // --- updated (and use Sugiyama layered graph layout to determine
  // --- update order)
  std::vector<std::string>     stack = {id};
  std::map<std::string, bool>  is_discovered = {};
  std::map<std::string, float> node_layer = {};
  std::vector<gnode::Point>    points = this->compute_graph_layout_sugiyama();

  auto points_it = points.begin(); // iterator
  for (const auto &[nid, n] : this->nodes)
  {
    is_discovered[nid] = false;
    node_layer[nid] = points_it->x;
    ++points_it;
  }

  // storage for the update procedure coming after
  std::vector<std::string> update_queue = {};

  std::map<std::string, std::vector<std::string>> connectivity_dw =
      this->get_connectivity_downstream();

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

    p_node->is_dirty = true;

    if (!is_discovered[nid])
    {
      is_discovered[nid] = true;

      // add 'downstream' nodes to the DFS stack
      for (auto &to_id : connectivity_dw[nid])
        stack.push_back(to_id);
    }
  }

  // --- update the nodes
  while (update_queue.size())
  {
    std::string nid = update_queue.front();
    update_queue.erase(update_queue.begin());

    LOG_DEBUG("UPDATING: %s(%s)",
              this->get_node_ref_by_id(nid)->get_label().c_str(),
              nid.c_str());

    this->get_node_ref_by_id(nid)->update();
  }

  this->post_update();

  return true;
}

} // namespace gnode
