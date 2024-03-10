/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <functional>

#include "gnode.hpp"

namespace gnode
{

int id_to_hash(const std::string &id)
{
  // https://stackoverflow.com/questions/8094790
  int h = 0;
  for (size_t i = 0; i < id.size(); ++i)
    h = h * 31 + static_cast<int>(id[i]);
  return h;
}

std::string id_to_label(const std::string &id)
{
  std::size_t pos = id.find("##");
  return id.substr(0, pos);
}

} // namespace gnode
