/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file data.hpp
 * @author Otto Link
 * @brief
 * @date 2023-08-07
 *
 * @copyright Copyright (c) 2023
 */
#pragma once
#include <memory>
#include <string>

namespace gnode
{

// BaseData class with type and value access
class BaseData
{
public:
  explicit BaseData(std::string type) : type(std::move(type)) {}
  virtual ~BaseData() = default;

  bool is_same_type(const BaseData &data) const
  {
    return this->get_type() == data.get_type();
  }

  std::string get_type() const { return this->type; }

  // Pure virtual method to get the value
  virtual void *get_value_ptr() const = 0;

private:
  std::string type;
};

// Template Data class that holds a value of type T
template <typename T> class Data : public BaseData
{
public:
  explicit Data() : BaseData(typeid(T).name()) {}

  explicit Data(std::string type) : BaseData(type) {}

  T    *get_value_ref() { return &value; }
  void *get_value_ptr() const override { return const_cast<T *>(&value); }

private:
  T value{};
};

} // namespace gnode