/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <functional>

#include "gnode.hpp"

namespace gnode
{

int id_to_hash(const std::string &id)
{
  return (int)std::hash<std::string>{}(id); // TODO check int conversion
}

std::string id_to_label(const std::string &id)
{
  std::size_t pos = id.find("##");
  return id.substr(0, pos);
}
} // namespace gnode
