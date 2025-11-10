/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file port.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Defines the Port, Input, and Output classes, along with the PortType
 * enumeration for handling ports in a graph node structure.
 * @date 2023-08-07
 *
 * @copyright Copyright (c) 2023 Otto Link. Distributed under the terms of the
 * GNU General Public License. The full license is in the file LICENSE,
 * distributed with this software.
 */

#pragma once
#include "gnode/data.hpp"
#include "gnode/logger.hpp"
#include <memory>
#include <string>
#include <typeinfo>

namespace gnode
{

/**
 * @enum PortType
 * @brief Enumeration for port types, indicating whether a port is an input or
 * an output.
 */
enum PortType
{
  IN, ///< Represents an input port.
  OUT ///< Represents an output port.
};

/**
 * @brief Abstract base class representing a port in a node.
 *
 * The Port class provides a common interface for input and output ports. Each
 * port has a label and can hold data.
 */
class Port
{
public:
  /**
   * @brief Default constructor for Port.
   */
  Port() = default;

  /**
   * @brief Constructs a Port with the specified label.
   * @param label A string representing the label of the port.
   */
  Port(std::string label) : label(label) {}

  /**
   * @brief Virtual destructor for Port.
   */
  virtual ~Port() = default;

  /**
   * @brief Retrieves the type name of the data handled by this port.
   * @return A string representing the type name.
   */
  std::string get_data_type() const { return this->data_type; }

  /**
   * @brief Retrieves the label of the port.
   * @return A string representing the port's label.
   */
  std::string get_label() const { return this->label; }

  /**
   * @brief Retrieves a shared pointer to the data associated with the port
   * after downcasting.
   * @return A shared pointer to the BaseData, or nullptr if not applicable.
   */
  virtual std::shared_ptr<BaseData> get_data_shared_ptr_downcasted() const
  {
    return nullptr;
  }

  /**
   * @brief Pure virtual function to get the type of the port (IN or OUT).
   *
   * This function must be implemented by derived classes to indicate whether
   * the port is an input (`PortType::IN`) or an output (`PortType::OUT`).
   *
   * @return The port type (`PortType::IN` or `PortType::OUT`).
   */
  virtual PortType get_port_type() const = 0;

  /**
   * @brief Retrieves a `void*` reference to the data value stored in this
   * output port.
   * @return A `void*` pointer to the data value.
   */
  virtual void *get_value_ref_void() const = 0;

  /**
   * @brief Sets the data associated with the port.
   * @param data A shared pointer to the BaseData to set.
   */
  virtual void set_data(std::shared_ptr<BaseData> /* data */) {}

protected:
  std::string data_type; ///< A string representing the type name.

private:
  std::string label = "no label"; ///< The label of the port.
};

/**
 * @brief Template class for input ports, specialized by data type.
 *
 * The Input class inherits from Port and provides methods for handling data
 * specific to input ports.
 *
 * @tparam T The data type that this input port will handle.
 */
template <typename T> class Input : public Port
{
public:
  /**
   * @brief Default constructor for Input.
   */
  Input() = default;

  /**
   * @brief Constructs an Input port with the specified label.
   * @param label A string representing the label of the input port.
   */
  Input(std::string label) : Port(label) { this->data_type = typeid(T).name(); }

  /**
   * @brief Virtual destructor for Input.
   */
  virtual ~Input() = default;

  /**
   * @brief Returns the type of the port as an input port.
   *
   * This method overrides the base class implementation to indicate that this
   * port is an input (`PortType::IN`).
   *
   * @return The port type, which is always `PortType::IN`.
   */
  PortType get_port_type() const override { return PortType::IN; }

  /**
   * @brief Retrieves a reference to the data value stored in this input port.
   * @return A pointer to the data value, or nullptr if the data is not
   * available.
   */
  T *get_value_ref() const
  {
    return this->data.lock() ? this->data.lock()->get_value_ref() : nullptr;
  }

  /**
   * @brief Retrieves a `void*` reference to the data value stored in this
   * output port.
   * @return A `void*` pointer to the data value.
   */
  void *get_value_ref_void() const override
  {
    return this->data.lock() ? (void *)this->data.lock()->get_value_ref()
                             : nullptr;
  }

  /**
   * @brief Sets the data associated with this input port.
   * @param data A shared pointer to the BaseData to set.
   */
  void set_data(std::shared_ptr<BaseData> data) override
  {
    this->data = std::dynamic_pointer_cast<Data<T>>(std::move(data));
  }

private:
  std::weak_ptr<Data<T>>
      data; ///< A weak pointer to the data associated with this input port.
};

/**
 * @brief Template class for output ports, specialized by data type.
 *
 * The Output class inherits from Port and provides methods for handling data
 * specific to output ports.
 *
 * @tparam T The data type that this output port will handle.
 */
template <typename T> class Output : public Port
{
public:
  /**
   * @brief Default constructor for Output.
   */
  Output() : data(std::make_shared<Data<T>>())
  {
    this->data_type = typeid(T).name();
  }

  /**
   * @brief Constructs an Output port with the specified label and additional
   * arguments.
   *
   * @tparam Args Types of the arguments passed to the Data<T> constructor.
   * @param label A string representing the label of the output port.
   * @param args Additional arguments passed to the Data<T> constructor.
   *
   * This constructor initializes an `Output` port with a given `label` and
   * forwards any additional arguments to the `Data<T>` constructor using
   * `std::make_shared`.
   */
  template <typename... Args>
  explicit Output(std::string label, Args &&...args)
      : Port(label),
        data(std::make_shared<Data<T>>(std::forward<Args>(args)...))
  {
    this->data_type = typeid(T).name();
  }

  /**
   * @brief Virtual destructor for Output.
   */
  virtual ~Output() = default;

  /**
   * @brief Retrieves a shared pointer to the data associated with this output
   * port after downcasting.
   * @return A shared pointer to the BaseData.
   */
  std::shared_ptr<BaseData> get_data_shared_ptr_downcasted() const override
  {
    return std::static_pointer_cast<BaseData>(this->data);
  }

  /**
   * @brief Retrieves the type name of the data handled by this output port.
   * @return A string representing the type name.
   */
  std::string get_data_type() const { return typeid(T).name(); }

  /**
   * @brief Returns the type of the port as an output port.
   *
   * This method overrides the base class implementation to indicate that this
   * port is an output (`PortType::OUT`).
   *
   * @return The port type, which is always `PortType::OUT`.
   */
  PortType get_port_type() const override { return PortType::OUT; }

  /**
   * @brief Retrieves a reference to the data value stored in this output port.
   * @return A pointer to the data value.
   */
  T *get_value_ref() const { return this->data->get_value_ref(); }

  /**
   * @brief Retrieves a `void*` reference to the data value stored in this
   * output port.
   * @return A `void*` pointer to the data value.
   */
  void *get_value_ref_void() const override
  {
    return (void *)this->data->get_value_ref();
  }

private:
  std::shared_ptr<Data<T>>
      data; ///< A shared pointer to the data associated with this output port.
};

} // namespace gnode
