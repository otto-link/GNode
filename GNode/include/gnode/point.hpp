/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file point.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Defines the `Point` struct used to represent a 2D point in space.
 * @date 2023-08-07
 *
 * This file contains the definition of the `Point` struct, which represents a
 * point in a two-dimensional space. The `Point` struct includes two
 * floating-point values representing the coordinates of the point.
 *
 * @copyright Copyright (c) 2023 Otto Link. Distributed under the terms of the
 * GNU General Public License. See the file LICENSE for the full license.
 */

#pragma once

namespace gnode
{

/**
 * @struct Point
 * @brief Represents a point in a 2D space.
 *
 * The `Point` struct contains two floating-point coordinates, `x` and `y`, that
 * specify the location of the point in a 2D space. It provides constructors to
 * initialize the point with default or specified coordinates.
 */
struct Point
{
  /**
   * @brief X-coordinate of the point.
   */
  float x;

  /**
   * @brief Y-coordinate of the point.
   */
  float y;

  /**
   * @brief Default constructor that initializes the point to the origin (0,0).
   */
  Point() : x(0), y(0) {}

  /**
   * @brief Constructs a `Point` with specified coordinates.
   * @param x The X-coordinate of the point.
   * @param y The Y-coordinate of the point.
   */
  Point(float x, float y) : x(x), y(y) {}
};

} // namespace gnode
