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
#include <string>

namespace gnode
{

// abstract data class
class Data
{
public:
  explicit Data(std::string type) : type(std::move(type)) {}

  virtual ~Data() = default;

  bool is_same_type(const Data &data) const
  {
    return this->get_type() == data.get_type();
  }

  std::string get_type() const { return this->type; }

private:
  std::string type;
};

} // namespace gnode