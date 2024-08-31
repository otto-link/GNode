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
#include <vector>

namespace gnode
{

struct Link
{
  std::string from; // out
  int         port_from;
  std::string to; // in
  int         port_to;

  Link(std::string from, int port_from, std::string to, int port_to)
      : from(from), port_from(port_from), to(to), port_to(port_to)
  {
  }

  bool operator==(const Link &other_link) const;

  void print();
};

} // namespace gnode