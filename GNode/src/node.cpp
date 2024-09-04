/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "gnode/node.hpp"
#include "gnode/logger.hpp"

namespace gnode
{

void Node::update()
{
  if (this->is_dirty)
  {
    this->compute();
    this->is_dirty = false;
  }
}

} // namespace gnode
