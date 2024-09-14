/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file data.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief This file defines the Node class used for managing input and output
 * ports and interacting with data in a graph-based system.
 * @date 2023-08-07
 *
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <memory>
#include <stdexcept>
#include <vector>

#include "gnode/data.hpp"
#include "gnode/port.hpp"

namespace gnode
{

/**
 * @brief Abstract Node class that represents a basic building block in a
 * graph-based system. Nodes contain ports, which can be connected to other
 * nodes for data flow.
 */
class Node
{
public:
  /**
   * @brief Flag indicating whether the node is marked as dirty (requiring an
   * update).
   */
  bool is_dirty = false;

  /**
   * @brief Default constructor for Node.
   */
  Node() = default;

  /**
   * @brief Construct a new Node object with a specific label.
   *
   * @param label The label for the node.
   */
  Node(std::string label) : label(label) {}

  /**
   * @brief Construct a new Node object with a specific label and identifier.
   *
   * @param label The label for the node.
   * @param id The unique identifier for the node.
   *
   * This constructor initializes a `Node` object with both a label and a unique
   * identifier. The label is used to describe or name the node, while the id
   * uniquely distinguishes the node from others in the system (storage in the
   * node itself is generally optional but may be handy).
   */
  Node(std::string label, std::string id) : label(label), id(id) {}

  /**
   * @brief Virtual destructor for Node.
   */
  virtual ~Node() = default;

  /**
   * @brief Add a port to the node, specifying whether it is an input or output.
   *
   * @tparam T The data type for the port.
   * @tparam Args Types of the arguments passed to the port constructor.
   * @param port_type The type of port (input or output).
   * @param port_label The label for the port.
   * @param args Additional arguments passed to the output port constructor if
   * the port type is output.
   *
   * This function adds a port of type `T` to the node. If the port type is
   * `PortType::IN`, it creates an `Input` port with the given `port_label`. If
   * the port type is `PortType::OUT`, it creates an `Output` port with the
   * given `port_label` and forwards additional arguments to the `Output` port
   * constructor.
   */
  template <typename T, typename... Args>
  void add_port(PortType           port_type,
                const std::string &port_label,
                Args &&...args)
  {
    if (port_type == PortType::IN)
      this->ports.push_back(std::make_shared<gnode::Input<T>>(port_label));
    else
      this->ports.push_back(
          std::make_shared<gnode::Output<T>>(port_label,
                                             std::forward<Args>(args)...));
  }

  /**
   * @brief Pure virtual function that forces derived classes to implement the
   * compute method, which updates the node's state.
   */
  virtual void compute() = 0;

  /**
   * @brief Get the label of the node.
   *
   * @return std::string The label of the node.
   */
  std::string get_label() const { return this->label; }

  /**
   * @brief Get the ID of the node.
   *
   * @return std::string The ID of the node.
   */
  std::string get_id() const { return this->id; }

  /**
   * @brief Get the total number of ports on the node.
   *
   * @return int The number of ports.
   */
  int get_nports() const;

  /**
   * @brief Get the number of ports of a specific type (input or output).
   *
   * @param port_type The type of port to count (input or output).
   * @return int The number of ports of the specified type.
   */
  int get_nports(PortType port_type) const;

  /**
   * @brief Get the output data from a specific port index.
   *
   * @param port_index The index of the output port.
   * @return std::shared_ptr<BaseData> The data from the specified port.
   */
  std::shared_ptr<BaseData> get_output_data(int port_index) const;

  /**
   * @brief Get the index of a port by its label.
   *
   * @param port_label The label of the port.
   * @return int The index of the port.
   */
  int get_port_index(const std::string &port_label) const;

  /**
   * @brief Get the label of a port by its index.
   *
   * @param port_index The index of the port.
   * @return std::string The label of the port.
   */
  std::string get_port_label(int port_index) const;

  /**
   * @brief Get the type of a port (input or output) by its label.
   *
   * @param port_label The label of the port.
   * @return PortType The type of the port.
   */
  PortType get_port_type(const std::string &port_label) const;

  /**
   * @brief Get a reference to the value stored in a port by its label.
   *
   * @tparam T The type of the value.
   * @param port_label The label of the port.
   * @return T* A pointer to the value.
   * @throws std::runtime_error If the port with the given label is not found,
   * or if casting the port to the appropriate type fails.
   */
  template <typename T> T *get_value_ref(const std::string &port_label) const
  {
    // Search for the port in the inputs vector
    for (const auto &port : this->ports)
      if (port->get_label() == port_label)
      {
        if (port->get_port_type() == PortType::IN)
        {
          auto inputPort = std::dynamic_pointer_cast<Input<T>>(port);
          if (inputPort) return inputPort->get_value_ref();
        }
        else
        {
          auto outputPort = std::dynamic_pointer_cast<Output<T>>(port);
          if (outputPort) return outputPort->get_value_ref();
        }
      }

    // If no matching port is found, throw an exception
    throw std::runtime_error("Port with label '" + port_label + "' not found.");
  }

  /**
   * @brief Get a reference to the value stored in a port by its index.
   *
   * @tparam T The type of the value.
   * @param port_index The index of the port.
   * @return T* A pointer to the value.
   * @throws std::out_of_range If the port index is invalid.
   */
  template <typename T> T *get_value_ref(int port_index) const
  {
    // Range check for the port index
    if (port_index < 0 || port_index >= static_cast<int>(this->ports.size()))
      throw std::out_of_range("Invalid port index");

    // Dynamic cast to the appropriate port type (Input or Output) and return
    // the value reference if the port is valid, otherwise return nullptr
    if (this->ports[port_index]->get_port_type() == PortType::IN)
    {
      auto port = std::dynamic_pointer_cast<Input<T>>(this->ports[port_index]);
      return port ? port->get_value_ref() : nullptr;
    }
    else
    {
      auto port = std::dynamic_pointer_cast<Output<T>>(this->ports[port_index]);
      return port ? port->get_value_ref() : nullptr;
    }
  }

  /**
   * @brief Set a new identifier for the node.
   *
   * @param new_id The new unique identifier to be assigned to the node.
   *
   * This method updates the node's identifier with the provided `new_id`. The
   * identifier is used to uniquely distinguish the node from others in the
   * system.
   */
  void set_id(std::string new_id) { this->id = new_id; }

  /**
   * @brief Set input data on a specific port by its index.
   *
   * @param data The data to set on the port.
   * @param port_index The index of the port.
   */
  void set_input_data(std::shared_ptr<BaseData> data, int port_index);

  /**
   * @brief Set the value of a port by its label.
   *
   * @tparam T The type of the value.
   * @param port_label The label of the port.
   * @param new_value The new value to set on the port.
   */
  template <typename T>
  void set_value(const std::string &port_label, T new_value)
  {
    T *p_value = this->get_value_ref<T>(port_label);
    *p_value = new_value;
  }

  /**
   * @brief Update the node, which involves processing its input and output
   * ports.
   */
  void update();

private:
  /**
   * @brief The label of the node.
   */
  std::string label;

  /**
   * @brief The ID of the node.
   */
  std::string id;

  /**
   * @brief A vector of shared pointers to the node's ports.
   */
  std::vector<std::shared_ptr<Port>> ports;
};

} // namespace gnode
