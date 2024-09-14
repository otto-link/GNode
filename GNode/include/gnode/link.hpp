/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file data.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Defines the `Link` struct used to represent connections between nodes.
 * @date 2023-08-07
 *
 * This file contains the definition of the `Link` struct, which represents a
 * connection between two nodes in a graph. The `Link` struct includes
 * information about the source and destination nodes, as well as the ports
 * involved in the connection.
 *
 * @copyright Copyright (c) 2023 Otto Link. Distributed under the terms of the
 * GNU General Public License. See the file LICENSE for the full license.
 */

#pragma once
#include <string>
#include <vector>

namespace gnode
{

/**
 * @struct Link
 * @brief Represents a connection between two nodes in a graph.
 *
 * The `Link` struct holds the details of a connection between two nodes,
 * including the identifiers of the nodes and the ports involved in the
 * connection. This information is used to define and manage the connections
 * within a graph.
 */
struct Link
{
  std::string from;      ///< Identifier of the source node.
  int         port_from; ///< Port number on the source node.
  std::string to;        ///< Identifier of the destination node.
  int         port_to;   ///< Port number on the destination node.

  /**
   * @brief Constructs a `Link` instance.
   * @param from Identifier of the source node.
   * @param port_from Port number on the source node.
   * @param to Identifier of the destination node.
   * @param port_to Port number on the destination node.
   */
  Link(std::string from, int port_from, std::string to, int port_to)
      : from(from), port_from(port_from), to(to), port_to(port_to)
  {
  }

  /**
   * @brief Equality operator for `Link`.
   * @param other_link Another `Link` instance to compare against.
   * @return `true` if this `Link` is equal to `other_link`, otherwise `false`.
   */
  bool operator==(const Link &other_link) const;

  /**
   * @brief Prints the details of the `Link`.
   *
   * This function outputs the details of the `Link` to the standard output.
   */
  void print();
};

} // namespace gnode
