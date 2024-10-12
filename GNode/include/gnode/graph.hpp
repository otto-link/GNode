/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file graph.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief This file defines the Graph class used for managing a collection of
 *        nodes and links in a directed graph. It supports operations for adding
 *        nodes, connecting nodes, exporting the graph, and more.
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

typedef unsigned int uint;

namespace gnode
{

/**
 * @brief The Graph class provides methods for manipulating nodes and
 * connections in a directed graph structure.
 */
class Graph
{
public:
  /**
   * @brief Construct a new Graph object.
   */
  Graph() = default;

  /**
   * @brief Destroy the Graph object.
   */
  virtual ~Graph() = default;

  /**
   * @brief Add a new node to the graph.
   *
   * @param p_node Shared pointer to the node to be added.
   * @param id Optional ID for the node. If empty, an ID will be generated.
   * @return std::string The ID of the added node.
   */
  std::string add_node(const std::shared_ptr<Node> &p_node,
                       const std::string           &id = "");

  /**
   * @brief Add a new node of a specific type to the graph.
   *
   * @tparam U Node type derived from Node class.
   * @tparam Args Arguments for the constructor of the node.
   * @param args Arguments to pass to the node constructor.
   * @return std::string The ID of the added node.
   */
  template <typename U, typename... Args> std::string add_node(Args... args)
  {
    return this->add_node(std::make_shared<U>(args...));
  }

  /**
   * @brief Clear the graph, remove all the nodes and the links.
   */
  void clear();

  /**
   * @brief Compute the layout of the graph using the Sugiyama algorithm.
   *
   * @return std::vector<Point> A vector of points representing the node
   * positions.
   */
  std::vector<Point> compute_graph_layout_sugiyama();

  /**
   * @brief Connect two nodes in the graph using port indices.
   *
   * @param from ID of the source node.
   * @param port_from Index of the source node's output port.
   * @param to ID of the destination node.
   * @param port_to Index of the destination node's input port.
   * @return true If the connection was successful.
   * @return false If the connection failed.
   */
  bool new_link(const std::string &from,
                int                port_from,
                const std::string &to,
                int                port_to);

  /**
   * @brief Connect two nodes in the graph using port labels.
   *
   * @param from ID of the source node.
   * @param port_label_from Label of the source node's output port.
   * @param to ID of the destination node.
   * @param port_label_to Label of the destination node's input port.
   * @return true If the connection was successful.
   * @return false If the connection failed.
   */
  bool new_link(const std::string &from,
                const std::string &port_label_from,
                const std::string &to,
                const std::string &port_label_to);

  /**
   * @brief Disconnect two nodes in the graph using port indices.
   *
   * @param from ID of the source node.
   * @param port_from Index of the source node's output port.
   * @param to ID of the destination node.
   * @param port_to Index of the destination node's input port.
   * @return true If the disconnection was successful.
   * @return false If the disconnection failed.
   */
  bool remove_link(const std::string &from,
                   int                port_from,
                   const std::string &to,
                   int                port_to);

  /**
   * @brief Disconnect two nodes in the graph using port labels.
   *
   * @param from The label of the source node.
   * @param port_label_from The label of the source node's output port.
   * @param to The label of the destination node.
   * @param port_label_to The label of the destination node's input port.
   * @return true If the disconnection was successful.
   * @return false If the disconnection failed.
   */
  bool remove_link(const std::string &from,
                   const std::string &port_label_from,
                   const std::string &to,
                   const std::string &port_label_to);

  /**
   * @brief Export the graph to a Graphviz DOT file.
   *
   * @param fname Filename of the DOT file.
   * @param graph_label Label for the graph.
   */
  void export_to_graphviz(const std::string &fname = "export.dot",
                          const std::string &graph_label = "graph");

  /**
   * @brief Export the graph to a Mermaid file.
   *
   * @param fname Filename of the Mermaid file.
   * @param graph_label Label for the graph.
   */
  void export_to_mermaid(const std::string &fname = "export.mmd",
                         const std::string &graph_label = "graph");

  /**
   * @brief Get the downstream connectivity of the graph.
   *
   * @return std::map<std::string, std::vector<std::string>> A map of node IDs
   *         and their downstream connections.
   */
  std::map<std::string, std::vector<std::string>> get_connectivity_downstream();

  /**
   * @brief Get the upstream connectivity of the graph.
   *
   * @return std::map<std::string, std::vector<std::string>> A map of node IDs
   *         and their upstream connections.
   */
  std::map<std::string, std::vector<std::string>> get_connectivity_upstream();

  /**
   * @brief Get the current count of unique identifiers.
   *
   * @return An available unique identifier.
   * */
  uint get_id_count() const { return this->id_count; }

  /**
   * @brief Get a pointer to a node by its ID.
   *
   * @tparam T Node type, default is Node.
   * @param node_id ID of the node.
   * @return T* Pointer to the node (returns `nullptr` if the node ID is not
   * found).
   * @throws std::runtime_error If casting the node to the specified type fails.
   */
  template <typename T = Node>
  T *get_node_ref_by_id(const std::string &node_id) const
  {
    auto it = nodes.find(node_id);
    if (it == nodes.end()) return nullptr;

    T *ptr = dynamic_cast<T *>(it->second.get());
    if (!ptr)
      throw std::runtime_error("Failed to cast node with ID: " + node_id +
                               " to the specified type.");

    return ptr;
  }

  /**
   * @brief Check if a node ID is available in the graph.
   *
   * @param id Node ID to check.
   * @return true If the ID is available.
   * @return false If the ID is already taken.
   */
  bool is_node_id_available(std::string id);

  /**
   * @brief Method called after the graph update process is completed.
   *
   * This method can be overridden in derived classes for custom post-update
   * behavior.
   */
  virtual void post_update() {}

  /**
   * @brief Print the current graph structure.
   */
  void print();

  /**
   * @brief Remove a node from the graph by its ID.
   *
   * @param id ID of the node to remove.
   */
  void remove_node(const std::string &id);

  /**
   * @brief Set the current count of unique identifiers.
   *
   * @param new_id An available unique identifier.
   * */
  void set_id_count(uint new_id_count) { this->id_count = new_id_count; }

  /**
   * @brief Mark all nodes as dirty and update the entire graph.
   *
   * This method forces a complete update of the graph, which may be inefficient
   * for large graphs.
   */
  void update();

  /**
   * @brief Update a specific node by its ID.
   *
   * @param id ID of the node to update.
   * @return true If the update was successful.
   */
  void update(std::string id);

protected:
  /**
   * @brief A map of node IDs to shared pointers of Node objects.
   */
  std::map<std::string, std::shared_ptr<Node>> nodes;

  /**
   * @brief A list of links between nodes in the graph.
   */
  std::vector<Link> links;

private:
  /**
   * @brief Keep track of unique identifiers.
   */
  uint id_count = 0;
};

} // namespace gnode
