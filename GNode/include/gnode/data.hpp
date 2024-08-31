/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file data.hpp
 * @author Otto Link
 * @brief This file defines the `Data` class, an abstract representation of data
 * within the `gnode` namespace.
 * @date 2023-08-07
 *
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <string>

namespace gnode
{

/**
 * @class Data
 * @brief A base class representing abstract data with a type identifier.
 *
 * The `Data` class serves as an abstract representation of data that includes a
 * type identifier. It provides basic functionality to compare types between
 * instances.
 */
class Data
{
public:
  /**
   * @brief Constructs a new Data object with the specified type.
   *
   * @param type A string representing the type of the data.
   */
  explicit Data(std::string type) : type(std::move(type)) {}

  /**
   * @brief Virtual destructor for the Data class.
   *
   * Ensures that derived classes can clean up resources properly.
   */
  virtual ~Data() = default;

  /**
   * @brief Compares the type of this Data object with another.
   *
   * @param data A reference to another Data object to compare with.
   * @return true if both Data objects have the same type, false otherwise.
   */
  bool is_same_type(const Data &data) const
  {
    return this->get_type() == data.get_type();
  }

  /**
   * @brief Gets the type of the Data object.
   *
   * @return A string representing the type of the Data object.
   */
  std::string get_type() const { return this->type; }

private:
  std::string type; ///< A string representing the type of the Data object.
};

} // namespace gnode