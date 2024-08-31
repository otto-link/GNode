/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <iostream>

#include "gnode/link.hpp"

namespace gnode
{

bool Link::operator==(const Link &other_link) const
{
  return ((this->from == other_link.from) && (this->to == other_link.to) &&
          (this->port_from == other_link.port_from) &&
          (this->port_to == other_link.port_to));
}

void Link::print()
{
  std::cout << "from: " << from << "(" << port_from << ") to: " << to << "("
            << port_to << ")" << std::endl;
}

} // namespace gnode
