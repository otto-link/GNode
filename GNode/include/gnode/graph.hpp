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
#include <map>
#include <memory>

#include "gnode/link.hpp"
#include "gnode/node.hpp"
#include "gnode/point.hpp"

namespace gnode
{

class Graph
{
public:
  Graph(){};

  virtual ~Graph() = default;

  std::string add_node(const std::shared_ptr<Node> &p_node,
                       std::string                  id = "");

  std::vector<Point> compute_graph_layout_sugiyama();

  // connect two nodes, note that data types are not verified
  bool connect(const std::string &from,
               int                port_from,
               const std::string &to,
               int                port_to);

  bool disconnect(const std::string &from,
                  int                port_from,
                  const std::string &to,
                  int                port_to);

  // after export: to convert, command line: dot export.dot -Tsvg > output.svg
  void export_to_graphviz(const std::string &fname = "export.dot",
                          const std::string &graph_label = "graph");

  // downstream means only output to input, i.e which nodes have any input
  // connected to a given node, ie. those nodes take informations from a given
  // node
  std::map<std::string, std::vector<std::string>> get_connectivity_downstream();

  // upstream means only input to output, i.e which nodes have any output
  // connected to a given node, ie. those nodes give informations to a given
  // node
  std::map<std::string, std::vector<std::string>> get_connectivity_upstream();

  template <class T = Node>
  T *get_node_ref_by_id(const std::string node_id) const
  {
    T *ptr = dynamic_cast<T *>(this->nodes.at(node_id).get());
    if (ptr)
      return ptr;
    else
      throw std::runtime_error("wrong type in dynamic cast");
  }

  bool is_node_id_available(std::string id);

  // Method called at the end of the tree update.
  virtual void post_update() {}

  void print();

  void remove_node(const std::string &id);

  // set all the nodes to dirty and update everything (can be usefull to reach a
  // clean state of the graph). algo is really inefficient, very slow
  void update();

  bool update_node(std::string id);

private:
  // Nodes of the tree, as a mapping (Id, Node shared pointer).
  std::map<std::string, std::shared_ptr<Node>> nodes = {};

  std::vector<Link> links = {};
};

} // namespace gnode