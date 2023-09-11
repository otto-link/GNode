/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file gnode.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief A generic node-based data structure for node graph programming.
 * @version 0.1
 * @date 2023-08-07
 *
 * @copyright Copyright (c) 2023
 */
#pragma once
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>

#include "macrologger.h"

/**
 * @brief Library namespace (GNode for "Generic Node").
 */
namespace gnode
{

enum dtype : int
{
  dint,
  dfloat,
};

/**
 * @brief Port direction, input or output.
 */
enum direction : int
{
  in,
  out
};

/**
 * @brief Port mandate, optional or not.
 */
enum optional : bool
{
  yes = true,
  no = false
};

/**
 * @brief Return an integer hash based on an `id`.
 *
 * @param id Input Id.
 * @return int Hash.
 */
int id_to_hash(const std::string &id);

/**
 * @brief Return a `label` based on an `id`.
 *
 * Label is the Id with eveything after the two characters `##` removed.
 *
 * @param id Input Id.
 * @return std::string Output label.
 */
std::string id_to_label(const std::string &id);

struct Point
{
  float x;
  float y;
  Point() : x(0), y(0){};
  Point(float x, float y) : x(x), y(y){};
};

class Node; // forward declaration for Port class

/**
 * @brief Port class, to handle the node "pins" (inputs and outputs).
 */
class Port
{
public:
  /**
   * @brief Port Id.
   */
  std::string id;

  /**
   * @brief Port label.
   *
   * Label is the Id with eveything after the two characters `##` removed.
   */
  std::string label;

  /**
   * @brief Integer Id based on the string Id hash.
   */
  int hash_id = 0;

  /**
   * @brief Port direction: input or output.
   *
   * @see {@link direction}.
   */
  int direction;

  /**
   * @brief Port direction: input or output.
   *
   * @see {@link type} or any other enum.
   */
  int dtype;

  /**
   * @brief Port mandate, optional or not
   *
   * @see {@link optional}.
   */
  bool is_optional = false;

  /**
   * @brief Is the port connected.
   */
  bool is_connected = false;

  /**
   * @brief Reference to the node carrying the port.
   */
  Node *p_node = nullptr;

  /**
   * @brief Reference to the node on the other end of any link connected to the
   * current port.
   */
  Node *p_linked_node = nullptr;

  /**
   * @brief Reference to the port on the other end of any link connected to the
   * current port.
   */
  Port *p_linked_port = nullptr;

  /**
   * @brief Construct a new Port object.
   *
   * @param id Id.
   * @param direction Direction.
   * @param dtype Data type.
   * @param is_optional Mandatory or not.
   */
  Port(std::string id, int direction, int dtype, bool is_optional);
  Port(std::string id, int direction, int dtype); /// @overload
  Port();                                         /// @overload

  /**
   * @brief Return a reference to the data carried out by the port.
   *
   * @return void* Data pointer.
   */
  void *get_p_data();

  /**
   * @brief Set the reference to the data carried out by the port.
   *
   * @param new_p_data New data pointer.
   */
  void set_p_data(void *new_p_data);

  /**
   * @brief Connect port, as an input, to another port.
   *
   * @param p_linked_node Reference to the node on the other end of the link.
   * @param p_linked_port Reference to the port on the other end of the link.
   * @param ptr_to_incoming_data Reference to the incoming data.
   * @param ignore_is_connected Connect even if it is already connected.
   */
  void connect_in(Node *p_linked_node,
                  Port *p_linked_port,
                  void *ptr_to_incoming_data,
                  bool  ignore_is_connected = false);

  /**
   * @brief Connect port, as an output, to another port.
   *
   * @param p_linked_node Reference to the node on the other end of the link.
   * @param p_linked_port Reference to the port on the other end of the link.
   */
  void connect_out(Node *p_linked_node, Port *p_linked_port);

  /**
   * @brief Disconnect the port.
   */
  void disconnect();

  /**
   * @brief Reset the port (used for instance during a disconnection).
   */
  void reset();

  /**
   * @brief Display some informations on the port.
   */
  void infos();

private:
  /**
   * @brief Reference to the data carried out by the port.
   */
  void *p_data = nullptr;
};

/**
 * @brief Node class, to handle nodes.
 */
class Node
{
public:
  /**
   * @brief Node Id.
   */
  std::string id;

  /**
   * @brief Node label.
   */
  std::string label;

  /**
   * @brief Integer Id based on the string Id hash.
   */
  int hash_id;

  /**
   * @brief Node category.
   */
  std::string category = "";

  /**
   * @brief Defined whether the outputs are frozen, i.e. they are not triggering
   * downstream nodes when updated.
   */
  bool frozen_outputs = false;

  /**
   * @brief Defined whether the node "auto-update", i.e. it's update can be
   * triggered by an update of any upstream node.
   */
  bool auto_update = true;

  /**
   * @brief Defined whether the node is up to date or not (i.e. needs to be
   * recomputed).
   */
  bool is_up_to_date = false;

  /**
   * @brief Construct a new Node object.
   *
   * @param id Node Id.
   */
  Node(std::string id);
  Node(); /// @overload

  // for debugging
  ~Node() { std::cout << "Node::~Node() " << this->id.c_str() << "\n"; }

  //----------------------------------------
  // getters / setters
  //----------------------------------------

  /**
   * @brief Return the node category.
   *
   * @return std::string
   */
  std::string get_category();

  /**
   * @brief Return the node type.
   *
   * @return std::string
   */
  std::string get_node_type();

  /**
   * @brief Return the number of ports by direction (input or output).
   *
   * @param direction Direction (in or out).
   * @param skip_optional Count optional or not.
   * @param skip_unconnected Count unconnected or not.
   * @return int Number of ports.
   */
  int get_nports_by_direction(int  direction,
                              bool skip_optional = true,
                              bool skip_connected = false);

  /**
   * @brief Return the reference to the data carried out by the port 'Id'.
   *
   * @param port_id Port Id.
   * @return void* Data pointer.
   */
  void *get_p_data(const std::string port_id);

  /**
   * @brief Get all the ports.
   *
   * @return std::map<std::string, Port> Port mapping (Id, Port).
   */
  std::map<std::string, Port> get_ports();

  /**
   * @brief Return the reference to the port 'Id'.
   *
   * @param port_id Port Id.
   * @return Port* Pointer.
   */
  Port *get_port_ref_by_id(const std::string port_id);

  /**
   * @brief Set the reference to the data carried out by the port 'Id'.
   *
   * @param port_id Port Id.
   * @param new_p_data Data pointer.
   */
  void set_p_data(const std::string port_id, void *new_p_data);

  /**
   * @brief Set the post update callback function.
   *
   * @param new_post_update_callback Callback.
   */
  void set_post_update_callback(
      std::function<void(Node *)> new_post_update_callback);

  //----------------------------------------
  // ports management
  //----------------------------------------

  /**
   * @brief Add a port the node.
   *
   * @param port Port object.
   */
  void add_port(const Port port);

  /**
   * @brief Return true if the mandatory inputs for node computation are
   * connected.
   *
   * @return true
   * @return false
   */
  bool are_inputs_ready() const;

  /**
   * @brief Return true if the upstream nodes connected at the inputs are up to
   * date.
   *
   * @return true
   * @return false
   */
  bool are_inputs_up_to_date() const;

  /**
   * @brief Connect a port, as an input, to another node/port.
   *
   * @param port_id Port Id to connect.
   * @param p_linked_node Reference to the node on the other end of the link.
   * @param p_linked_port Reference to the port on the other end of the link.
   * @param ptr_to_incoming_data Reference to the incoming data.
   */
  void connect_port_in(std::string port_id,
                       Node       *p_linked_node,
                       Port       *p_linked_port,
                       void       *ptr_to_incoming_data = nullptr);

  /**
   * @brief Connect a port, as an output, to another node/port.
   *
   * @param port_id Port Id to connect.
   * @param p_linked_node Reference to the node on the other end of the link.
   * @param p_linked_port Reference to the port on the other end of the link.
   */
  void connect_port_out(std::string port_id,
                        Node       *p_linked_node,
                        Port       *p_linked_port);

  /**
   * @brief Disconnect port 'Id'.
   *
   * @param port_id Port Id.
   */
  void disconnect_port(std::string port_id);

  /**
   * @brief Disconnect all the ports.
   */
  void disconnect_all_ports();

  /**
   * @brief Return true if port 'Id' is in the port mapping (Id, Port).
   *
   * @param port_id Port Id.
   * @return true
   * @return false
   */
  bool is_port_id_in_keys(const std::string port_id);

  /**
   * @brief Remove port 'Id.
   *
   * @param port_id Port Id.
   */
  void remove_port(const std::string port_id);

  /**
   * @brief Update links by refreshing data pointer of the outputs.
   */
  void update_links();

  //----------------------------------------
  // computing
  //----------------------------------------

  /**
   * @brief Compute current node.
   */
  virtual void compute()
  {
    // throw an error if the 'compute' method is not defined in a
    // derived class
    LOG_ERROR("Compute not defined for node [%s])", this->id.c_str());
    throw std::runtime_error("undefined 'compute' method");
  }

  /**
   * @brief Force update of the current node by updating not matter
   * the state of "is_up_to_date" (can be useful when a node parameter
   * if changed).
   *
   * @note `auto_update` can still prevent the update.
   */
  void force_update();

  /**
   * @brief Update current node: first trigger the update of surrounding nodes
   * before being able to compute.
   */
  void update();

  /**
   * @brief Update inner bindings (e.g. links between inputs and outputs of the
   * node).
   */
  virtual void update_inner_bindings()
  {
    LOG_DEBUG("No inner bindings for node [%s]", this->id.c_str());
  }

  //----------------------------------------
  // displaying infos
  //----------------------------------------

  /**
   * @brief Display some informations on the node.
   */
  void infos();

  /**
   * @brief Print node links.
   */
  void print_links();

  /**
   * @brief Print node treeview.
   */
  void treeview();

protected:
  /**
   * @brief Node type.
   */
  std::string node_type = "";

private:
  /**
   * @brief Ports of the node, as a mapping (Id, Port).
   */
  std::map<std::string, Port> ports = {};

  /**
   * @brief Reference to the post-update callback.
   */
  std::function<void(Node *)> post_update_callback;
};

/**
 * @brief Tree object, to manipulate collection of nodes connected between each
 * other.
 */
class Tree
{
public:
  /**
   * @brief Tree Id.
   */
  std::string id;

  /**
   * @brief Tree label.
   */
  std::string label;

  /**
   * @brief Construct a new Tree object.
   *
   * @param id Tree Id.
   */
  Tree(std::string id);

  /**
   * @brief Return the adjacency list of the tree.
   *
   * @return std::vector<std::vector<size_t>> List of node indices (index
   * corresponds to the node index in the tree node mapping)
   */
  std::vector<std::vector<size_t>> get_adjacency_list();

  /**
   * @brief Get the node id corresponding to a hash id.
   *
   * @param node_hash_id Hash id.
   * @return std::string Node id.
   */
  std::string get_node_id_by_hash_id(int node_hash_id);

  /**
   * @brief Return the reference of the node with the given hash id.
   *
   * @param node_hash_id Hash Id.
   * @return Node* Pointer.
   */
  Node *get_node_ref_by_hash_id(int node_hash_id);

  /**
   * @brief Return the reference of the node 'Id'.
   *
   * @param node_id Node Id.
   * @return Node* Pointer.
   */
  Node *get_node_ref_by_id(const std::string node_id) const;

  /**
   * @brief Return all the nodes, as a mapping.
   *
   * @return std::map<std::string, std::shared_ptr<Node>> Nodes.
   */
  std::map<std::string, std::shared_ptr<Node>> get_nodes_map() const;

  /**
   * @brief Get the node and port Id corresponding to a given port hash Id.
   *
   * @param port_hash_id Port hash Id.
   * @param node_id Node Id.
   * @param port_id Port Id.
   */
  void get_ids_by_port_hash_id(int          port_hash_id,
                               std::string &node_id,
                               std::string &port_id);

  /**
   * @brief Return true if the tree graph is cyclic.
   */
  bool is_cyclic();

  /**
   * @brief Return the number of nodes.
   */
  size_t size();

  //----------------------------------------
  // nodes management
  //----------------------------------------

  /**
   * @brief Add a node.
   *
   * @param p_node Shared reference to the node.
   */
  void add_node(std::shared_ptr<Node> p_node);

  /**
   * @brief Return the graph layout using the Fruchterman-Reingold algorithm
   * (https://github.com/olvb/nodesoup)
   *
   * @param iterations Maximum number of iterations.
   * @param k Force coefficient.
   * @return std::vector<gnode::Point> Node positions.
   */
  std::vector<gnode::Point> compute_graph_layout_fruchterman_reingold(
      int   iterations = 300,
      float k = 15.f);

  /**
   * @brief Return the graph layout using the Sugiyama's algorithm for
   * layered graph layout
   *
   * @return std::vector<gnode::Point> Node positions.
   */
  std::vector<gnode::Point> compute_graph_layout_sugiyama();

  /**
   * @brief Return true if the 'Id' is in the node mapping.
   *
   * @param node_id Node Id.
   * @return true
   * @return false
   */
  bool is_node_id_in_keys(const std::string node_id) const;

  /**
   * @brief Remove node 'Id'.
   *
   * @param node_id Node Id.
   */
  void remove_node(std::string node_id);

  /**
   * @brief Remove all the nodes.
   */
  void remove_all_nodes();

  //----------------------------------------
  // linking
  //----------------------------------------

  /**
   * @brief Link a node/port to another node/port.
   *
   * @param node_id_from Node Id of the link start.
   * @param port_id_from Port Id of the link start.
   * @param node_id_to Node Id of the link end.
   * @param port_id_to Port Id of the link end.
   */
  void link(const std::string node_id_from, // out
            const std::string port_id_from,
            const std::string node_id_to, // in
            const std::string port_id_to);

  /**
   * @brief Un-link a node/port to another node/port.
   *
   * @param node_id_from Node Id of the link start.
   * @param port_id_from Port Id of the link start.
   * @param node_id_to Node Id of the link end.
   * @param port_id_to Port Id of the link end.
   */
  void unlink(const std::string node_id_from, // out
              const std::string port_id_from,
              const std::string node_id_to, // in
              const std::string port_id_to);

  //----------------------------------------
  // computing
  //----------------------------------------

  /**
   * @brief Update the whole tree.
   */
  void update();

  /**
   * @brief Update the node 'Id'.
   *
   * @param node_id Node Id.
   */
  void update_node(std::string node_id);

  //----------------------------------------
  // displaying infos
  //----------------------------------------

  /**
   * @brief Display some informations of the tree.
   */
  void infos();

  /**
   * @brief Print node links.
   */
  void print_node_links();

  /**
   * @brief Print node list.
   */
  void print_node_list();

  /**
   * @brief Export tree as a csv.
   *
   * @param fname_nodes Filename for the nodes.
   * @param fname_links Filename for the links.
   */
  void export_csv(std::string fname_nodes, std::string fname_links);

protected:
  /**
   * @brief Nodes of the tree, as a mapping (Id, Node shared pointer).
   */
  std::map<std::string, std::shared_ptr<Node>> nodes_map = {};

private:
  /**
   * @brief Return the shared pointer the node 'Id'.
   *
   * @param node_id Node Id.
   * @return std::shared_ptr<Node> Shared pointer.
   */
  std::shared_ptr<Node> get_node_sptr_by_id(const std::string node_id) const;
};

// helper
bool is_graph_cyclic(const std::vector<std::vector<size_t>> &adj);

} // namespace gnode
