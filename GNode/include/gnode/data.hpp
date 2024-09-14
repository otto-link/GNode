/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file data.hpp
 * @author Otto Link
 * @brief Defines the BaseData and Data classes for handling typed data in the
 * gnode namespace.
 * @date 2023-08-07
 *
 * @copyright Copyright (c) 2023 Otto Link. Distributed under the terms of the
 * GNU General Public License. The full license is in the file LICENSE,
 * distributed with this software.
 */

#pragma once
#include <memory>
#include <string>
#include <typeinfo>

namespace gnode
{

/**
 * @brief Abstract base class representing generic data with type information.
 *
 * The BaseData class provides a common interface for handling data of different
 * types. It stores a type identifier as a string and requires derived classes
 * to implement a method to retrieve a pointer to the stored value.
 */
class BaseData
{
public:
  /**
   * @brief Constructs a BaseData object with the specified type.
   * @param type A string representing the type of the data.
   */
  explicit BaseData(std::string type) : type(type) {}

  /**
   * @brief Virtual destructor for BaseData.
   */
  virtual ~BaseData() = default;

  /**
   * @brief Checks if the type of this data matches the type of another BaseData
   * object.
   * @param data The other BaseData object to compare with.
   * @return True if the types match, false otherwise.
   */
  bool is_same_type(const BaseData &data) const
  {
    return this->get_type() == data.get_type();
  }

  /**
   * @brief Retrieves the type of the data as a string.
   * @return A string representing the type of the data.
   */
  std::string get_type() const { return this->type; }

  /**
   * @brief Pure virtual method to retrieve a pointer to the stored value.
   * @return A void pointer to the value.
   */
  virtual void *get_value_ptr() const = 0;

private:
  std::string type; ///< A string representing the type of the data.
};

/**
 * @brief Template class for holding data of a specific type.
 *
 * The Data class template inherits from BaseData and provides a concrete
 * implementation for storing and accessing data of type T. It allows retrieval
 * of the stored value as a pointer.
 *
 * @tparam T The type of data to be stored.
 */
template <typename T> class Data : public BaseData
{
public:
  /**
   * @brief Constructs a Data object with the type name of T and initializes its
   * value.
   *
   * @tparam Args Types of the arguments passed to the T constructor.
   * @param args Arguments forwarded to the constructor of T to initialize the
   * value.
   *
   * This constructor initializes the base class `BaseData` with the type name
   * of `T` using `typeid(T).name()`. It then constructs `this->value` of type
   * `T` using the forwarded arguments `args`.
   */
  template <typename... Args>
  explicit Data(Args &&...args) : BaseData(typeid(T).name())
  {
    this->value = T(std::forward<Args>(args)...);
  }

  /**
   * @brief Retrieves a reference to the stored value.
   * @return A pointer to the stored value.
   */
  T *get_value_ref() { return &value; }

  /**
   * @brief Retrieves a pointer to the stored value.
   * @return A void pointer to the stored value.
   */
  void *get_value_ptr() const override { return const_cast<T *>(&value); }

private:
  T value{}; ///< The value of type T stored in this object.
};

} // namespace gnode
