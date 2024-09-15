/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <algorithm>
#include <fstream>
#include <iostream>

#include "demekgraph/updated/include/graph.hpp"
#include "demekgraph/updated/include/layout.hpp"

#include "gnode/graph.hpp"
#include "gnode/logger.hpp"

namespace gnode
{

std::string Graph::add_node(const std::shared_ptr<Node> &p_node,
                            const std::string           &id)
{
  std::string node_id = id;

  // Use node pointer as ID if none is provided
  if (node_id.empty()) node_id = std::to_string(id_count++);

  // Check if the ID is available
  if (!this->is_node_id_available(node_id))
    throw std::runtime_error("Node ID already used: " + node_id);

  // Add the node to the map and store the ID within the node (in case of)
  this->nodes[node_id] = p_node;
  p_node->set_id(node_id);

  return node_id;
}

void Graph::clear()
{
  this->nodes.clear();
  this->links.clear();
  this->id_count = 0;
}

std::vector<Point> Graph::compute_graph_layout_sugiyama()
{
  std::vector<Point> points;
  const size_t       num_nodes = this->nodes.size();

  // Create an adjacency list for the Sugiyama procedure
  std::vector<std::vector<size_t>> adj(num_nodes);

  // Build a node ID to node index map
  std::map<std::string, size_t> node_idx;
  size_t                        idx = 0;

  for (const auto &[nid, p_node] : this->nodes)
    node_idx[nid] = idx++;

  // Populate the adjacency list using downstream connectivity
  const auto connectivity = this->get_connectivity_downstream();

  for (const auto &[nid, to_ids] : connectivity)
  {
    size_t from_idx = node_idx.at(nid);
    for (const auto &to_id : to_ids)
      adj[from_idx].push_back(node_idx.at(to_id));
  }

  // Build the graph using the adjacency list
  graph_builder gb;

  for (size_t i = 0; i < adj.size(); ++i)
    for (size_t j : adj[i])
      gb.add_edge(i, j);

  graph graph = gb.build();

  // Set Sugiyama layout attributes
  attributes attr;
  attr.node_size = 0;
  attr.node_dist = 1;
  attr.layer_dist = 1;

  // Compute Sugiyama layout
  sugiyama_layout layout(graph, attr);

  // Convert layout vertices to points (reverse x and y for horizontal layout)
  points.reserve(layout.vertices().size());

  for (const auto &vertex : layout.vertices())
    points.emplace_back(vertex.pos.y - 0.5f * layout.height(),
                        vertex.pos.x - 0.5f * layout.width());

  return points;
}

// connect two nodes, note that data types are not verified
bool Graph::new_link(const std::string &from,
                     int                port_from,
                     const std::string &to,
                     int                port_to)
{
  Link new_link(from, port_from, to, port_to);

  // Check if the link already exists
  if (std::find(this->links.begin(), this->links.end(), new_link) !=
      this->links.end())
    return false;

  // Get the output data from the source node
  auto from_node_it = this->nodes.find(from);
  if (from_node_it == this->nodes.end())
    throw std::runtime_error("Source node not found: " + from);

  auto to_node_it = this->nodes.find(to);
  if (to_node_it == this->nodes.end())
    throw std::runtime_error("Destination node not found: " + to);

  std::shared_ptr<BaseData> from_data = from_node_it->second->get_output_data(
      port_from);

  // Set the input data on the destination node
  to_node_it->second->set_input_data(from_data, port_to);

  // Add the new link to the list of links
  this->links.push_back(new_link);

  return true;
}

bool Graph::new_link(const std::string &from,
                     const std::string &port_label_from,
                     const std::string &to,
                     const std::string &port_label_to)
{
  // Check that the 'from' port is an output port
  if (this->nodes.at(from)->get_port_type(port_label_from) != PortType::OUT)
    throw std::invalid_argument("Port '" + port_label_from + "' on node '" +
                                from + "' must be an output port.");

  // Check that the 'to' port is an input port
  if (this->nodes.at(to)->get_port_type(port_label_to) != PortType::IN)
    throw std::invalid_argument("Port '" + port_label_to + "' on node '" + to +
                                "' must be an input port.");

  // Call the existing connect method using the port indices
  return this->new_link(from,
                        this->nodes.at(from)->get_port_index(port_label_from),
                        to,
                        this->nodes.at(to)->get_port_index(port_label_to));
}

bool Graph::remove_link(const std::string &from,
                        int                port_from,
                        const std::string &to,
                        int                port_to)
{
  Link link(from, port_from, to, port_to);

  // Check if the link exists
  auto link_it = std::find(this->links.begin(), this->links.end(), link);
  if (link_it == this->links.end()) return false;

  // Ensure the destination node exists before attempting to disconnect
  auto to_node_it = this->nodes.find(to);
  if (to_node_it == this->nodes.end())
    throw std::runtime_error("Destination node not found: " + to);

  // Disconnect nodes by setting the input data to null
  to_node_it->second->set_input_data(nullptr, port_to);

  // Remove the link from the list of links
  this->links.erase(link_it);

  return true;
}

bool Graph::remove_link(const std::string &from,
                        const std::string &port_label_from,
                        const std::string &to,
                        const std::string &port_label_to)
{
  // Check that the 'from' port is an output port
  if (this->nodes.at(from)->get_port_type(port_label_from) != PortType::OUT)
    throw std::invalid_argument("Port '" + port_label_from + "' on node '" +
                                from + "' must be an output port.");

  // Check that the 'to' port is an input port
  if (this->nodes.at(to)->get_port_type(port_label_to) != PortType::IN)
    throw std::invalid_argument("Port '" + port_label_to + "' on node '" + to +
                                "' must be an input port.");

  // Call the existing connect method using the port indices
  return this->remove_link(
      from,
      this->nodes.at(from)->get_port_index(port_label_from),
      to,
      this->nodes.at(to)->get_port_index(port_label_to));
}

void Graph::export_to_graphviz(const std::string &fname,
                               const std::string &graph_label)
{
  std::ofstream file(fname);

  if (!file.is_open())
    throw std::runtime_error("Failed to open file: " + fname);

  file << "digraph root {\n";
  file << "label=\"" << graph_label << "\";\n";
  file << "labelloc=\"t\";\n";
  file << "rankdir=TD;\n";
  file << "ranksep=0.5;\n";
  file << "node [shape=record];\n";

  // Output nodes with their labels
  for (const auto &[id, p_node] : this->nodes)
    file << id << " [label=\"" << p_node->get_label() << "\"];\n";

  // Output edges
  const auto connectivity = this->get_connectivity_downstream();

  for (const auto &[from_id, to_ids] : connectivity)
    for (const auto &to_id : to_ids)
      file << from_id << " -> " << to_id << ";\n";

  file << "}\n";
}

void Graph::export_to_mermaid(const std::string &fname,
                              const std::string &graph_label)
{
  std::fstream f;
  f.open(fname, std::ios::out);

  f << "---" << std::endl;
  f << "title: " << graph_label << std::endl;
  f << "---" << std::endl;
  f << "flowchart LR" << std::endl;

  // nodes
  for (auto &[id, p_node] : this->nodes)
    f << "    " << id << "([" << p_node->get_label() << "])\n";

  // Output edges
  const auto connectivity = this->get_connectivity_downstream();

  for (const auto &[from_id, to_ids] : connectivity)
    for (const auto &to_id : to_ids)
      f << from_id << " --> " << to_id << ";\n";

  f.close();
}

std::map<std::string, std::vector<std::string>> Graph::
    get_connectivity_downstream()
{
  std::map<std::string, std::vector<std::string>> connectivity;

  // to get all the nodes in the mapping, even if they have no node
  // downstream
  for (auto &[nid, _] : this->nodes)
    connectivity[nid] = {};

  for (auto &link : this->links)
    connectivity[link.from].push_back(link.to);

  return connectivity;
}

std::map<std::string, std::vector<std::string>> Graph::
    get_connectivity_upstream()
{
  std::map<std::string, std::vector<std::string>> connectivity;

  // to get all the nodes in the mapping, even if they have no node
  // upstream
  for (auto &[nid, _] : this->nodes)
    connectivity[nid] = {};

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

void Graph::remove_node(const std::string &id)
{
  if (this->is_node_id_available(id))
    throw std::runtime_error("Unknown node ID: " + id);

  // Disconnect node by clearing input data on connected nodes
  for (const auto &link : this->links)
    if (link.from == id)
    {
      auto node_it = this->nodes.find(link.to);
      if (node_it != this->nodes.end())
      {
        node_it->second->set_input_data(nullptr, link.port_to);
      }
    }

  // Remove links associated with the node
  this->links.erase(std::remove_if(this->links.begin(),
                                   this->links.end(),
                                   [&id](const Link &link) {
                                     return link.from == id || link.to == id;
                                   }),
                    this->links.end());

  // Remove the node from the graph
  this->nodes.erase(id);
}

void Graph::update()
{
  GLOG->trace("Updating graph...");

  // Set all nodes to a "dirty" state
  for (auto &[nid, p_node] : this->nodes)
    p_node->is_dirty = true;

  // Get the upstream connectivity of the graph
  auto connectivity_up = this->get_connectivity_upstream();

  // Trigger nodes with no upstream connections (no inputs)
  for (const auto &[nid, up_ids] : connectivity_up)
    if (up_ids.empty())
    {
      GLOG->trace("Updating node: {}({})",
                  this->get_node_ref_by_id(nid)->get_label(),
                  nid);
      this->update(nid);
    }

  this->post_update();
}

void Graph::update(std::string id)
{
  // --- first check that all the nodes upstream the node to be
  // --- updated are up to date (i.e. no 'dirty'), if not, no need to
  // --- update the node
  std::map<std::string, std::vector<std::string>> connectivity_up =
      this->get_connectivity_upstream();

  for (auto &id_up : connectivity_up[id])
    if (this->get_node_ref_by_id(id_up)->is_dirty)
    {
      GLOG->trace("no update of the graph");
      return;
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

    p_node->is_dirty = true;

    if (!is_discovered[nid])
    {
      is_discovered[nid] = true;

      // add 'downstream' nodes to the DFS stack
      for (auto &to_id : connectivity_dw[nid])
        stack.push_back(to_id);
    }
  }

  GLOG->trace("update queue:");
  for (auto &s : update_queue)
    GLOG->trace("node id: {}", s);

  // --- update the nodes
  while (update_queue.size())
  {
    std::string nid = update_queue.front();
    update_queue.erase(update_queue.begin());

    GLOG->trace("updating: {}({})",
                this->get_node_ref_by_id(nid)->get_label(),
                nid);

    this->get_node_ref_by_id(nid)->update();
  }

  this->post_update();
}

} // namespace gnode
