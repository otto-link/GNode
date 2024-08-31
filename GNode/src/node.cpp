/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "gnode/node.hpp"

namespace gnode
{

void Node::update()
{
  if (this->is_dirty)
  {
    // if (this->pre_update_callback)
    // 	this->pre_update_callback(this);

    this->compute();
    LOG_DEBUG("node updated");
    // if (this->post_update_callback)
    //   this->post_update_callback(this);

    this->is_dirty = false;
  }
  else
    LOG_DEBUG("node not updated");
}

} // namespace gnode
