/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file port.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @date 2023-08-07
 *
 * @copyright Copyright (c) 2023
 */
#pragma once
#include <typeinfo>

#include "gnode/data.hpp"
#include "gnode/logger.hpp"

namespace gnode
{

// abstract port class
class Port
{
public:
  Port(){};
  Port(std::string label) : label(label){};
  virtual ~Port() = default;

  std::string get_label() const { return this->label; }

  virtual std::shared_ptr<BaseData> get_data_shared_ptr_downcasted()
  {
    return nullptr;
  };

  virtual void set_data(std::shared_ptr<BaseData> /* data */) {}

private:
  std::string label = "no label";
};

// abstract input class
template <typename T> class Input : public Port
{
public:
  Input(){};
  Input(std::string label) : Port(label){};
  virtual ~Input() = default;

  std::string get_type() { return typeid(*this->data->get_value_ref()).name(); }

  T *get_value_ref() const
  {
    return this->data.lock() ? this->data.lock()->get_value_ref() : nullptr;
  }

  void set_data(std::shared_ptr<BaseData> data)
  {
    this->data = std::dynamic_pointer_cast<Data<T>>(data);
  }

private:
  std::weak_ptr<Data<T>> data;
};

// abstract output class
template <typename T> class Output : public Port
{
public:
  Output() { this->data = std::make_shared<Data<T>>(this->get_type()); };

  Output(std::string label) : Port(label)
  {
    this->data = std::make_shared<Data<T>>(this->get_type());
  };

  virtual ~Output() = default;

  std::shared_ptr<BaseData> get_data_shared_ptr_downcasted() override
  {
    return std::static_pointer_cast<BaseData>(this->data);
  }

  std::string get_type() { return typeid(*this->data->get_value_ref()).name(); }

  T *get_value_ref() const { return this->data->get_value_ref(); }

private:
  std::shared_ptr<Data<T>> data;
};

} // namespace gnode