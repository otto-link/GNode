/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file logger.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Defines the `Logger` class for managing logging functionality.
 * @date 2023-08-07
 *
 * This file contains the definition of the `Logger` class, which provides a
 * singleton instance of a logger using the spdlog library. The `Logger` class
 * is designed to facilitate logging messages with color support to the standard
 * output. It ensures that only one instance of the logger exists throughout the
 * application.
 *
 * @copyright Copyright (c) 2023 Otto Link. Distributed under the terms of the
 * GNU General Public License. See the file LICENSE for the full license.
 */
#pragma once

#include <memory>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace gnode
{

/**
 * @class Logger
 * @brief Manages logging functionality using the spdlog library.
 *
 * The `Logger` class provides a singleton instance of a logger that can be used
 * throughout the application. It utilizes the spdlog library for logging
 * messages to the standard output with color support.
 *
 * The class is designed to be a singleton, ensuring that there is only one
 * instance of the logger throughout the application. It provides a static
 * method to access this instance.
 */
class Logger
{
public:
  /**
   * @brief Retrieves the singleton instance of the logger.
   * @return A reference to the shared pointer of the logger instance.
   *
   * This method returns the singleton logger instance. It is used to access
   * the logger for logging messages.
   */
  static std::shared_ptr<spdlog::logger> &log();

private:
  // Private constructor to prevent direct instantiation
  Logger() = default;

  // Disable copy constructor
  Logger(const Logger &) = delete;

  // Disable assignment operator
  Logger &operator=(const Logger &) = delete;

  // Static member to hold the singleton instance
  static std::shared_ptr<spdlog::logger> instance;
};

} // namespace gnode
