/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file point.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @date 2023-08-07
 *
 * @copyright Copyright (c) 2023
 */
#pragma once

namespace gnode
{

struct Point
{
  float x;
  float y;
  Point() : x(0), y(0){};
  Point(float x, float y) : x(x), y(y){};
};

} // namespace gnode