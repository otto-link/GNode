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
  Port() = default;
  Port(std::string label) : label(std::move(label)){};
  virtual ~Port() = default;

  std::string get_label() const { return this->label; }

  virtual std::shared_ptr<BaseData> get_data_shared_ptr_downcasted() const
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
  Input() = default;

  Input(std::string label) : Port(std::move(label)){};

  virtual ~Input() = default;

  std::string get_type() { return typeid(T).name(); }

  T *get_value_ref() const
  {
    return this->data.lock() ? this->data.lock()->get_value_ref() : nullptr;
  }

  void set_data(std::shared_ptr<BaseData> data)
  {
    this->data = std::dynamic_pointer_cast<Data<T>>(std::move(data));
  }

private:
  std::weak_ptr<Data<T>> data;
};

// abstract output class
template <typename T> class Output : public Port
{
public:
  Output() : data(std::make_shared<Data<T>>()) {}

  explicit Output(std::string label)
      : Port(std::move(label)), data(std::make_shared<Data<T>>())
  {
  }

  virtual ~Output() = default;

  std::shared_ptr<BaseData> get_data_shared_ptr_downcasted() const override
  {
    return std::static_pointer_cast<BaseData>(this->data);
  }

  std::string get_type() const { return typeid(T).name(); }

  T *get_value_ref() const { return this->data->get_value_ref(); }

private:
  std::shared_ptr<Data<T>> data;
};

} // namespace gnode