/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "gnode/logger.hpp"

namespace gnode
{

// Initialize the static member
std::shared_ptr<spdlog::logger> Logger::instance = nullptr;

std::shared_ptr<spdlog::logger> &Logger::log()
{
  if (!instance)
  {
    instance = spdlog::stdout_color_mt("console_gnode");
    instance->set_pattern("[gnode-] [%H:%M:%S] [%^---%L---%$] %v");
    instance->set_level(spdlog::level::trace);
  }
  return instance;
}

} // namespace gnode
