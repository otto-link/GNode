/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @mainpage GNode
 *
 * @section intro_sec Introduction
 *
 * **GNode** is a lightweight and extensible C++ framework for building
 * node-based graphs. It allows you to define nodes, connect them through
 * input/output ports, and process data through a directed graph structure.
 *
 * **GNode** is designed to be:
 * - Modular: you create only the nodes you need
 * - Type-safe: strong typing of ports and attributes
 * - Extensible: support custom types as long as they are copyable or movable
 *
 * A graph in GNode is composed of:
 * - Nodes: units of logic or data
 * - Ports: typed inputs/outputs on nodes
 * - Links: connections between ports
 * - Graph (supervisor): manager for node creation, registration and update
 *
 * @section install Installation
 *
 * See the REAMDE in the GitHub repository: https://github.com/otto-link/GNode
 *
 * @section usage_sec Usage Example
 *
 * @code{.cpp}
 * #include "gnode.hpp"
 * #include <iostream>
 *
 * // create a class to show that complex objects can be handled
 * struct Vec2
 * {
 *   float x;
 *   float y;
 *   Vec2() = default;
 *   Vec2(float x, float y) : x(x), y(y) {}
 * };
 *
 * // ----------------------------------------
 * // Nodes definition, all derivates from gnode::Node
 * // ----------------------------------------
 *
 * // holds a float value
 * class Value : public gnode::Node
 * {
 * public:
 *   Value() : gnode::Node("Value")
 *   {
 *     // declare an output port of type float
 *     this->add_port<float>(gnode::PortType::OUT, "value");
 *   }
 *
 *   Value(float value) : gnode::Node("Value")
 *   {
 *     this->add_port<float>(gnode::PortType::OUT, "value");
 *     // directly set initial value of the port
 *     this->set_value<float>("value", value);
 *   }
 *
 *   // leaf node: nothing to compute, only stores a value
 *   void compute() {}
 * };
 *
 * // holds a Vec2 value
 * class ValueVec : public gnode::Node
 * {
 * public:
 *   ValueVec() : gnode::Node("ValueVec")
 *   {
 *     // declare an output port containing Vec2 objects
 *     this->add_port<Vec2>(gnode::PortType::OUT, "value");
 *   }
 *
 *   ValueVec(float x, float y) : gnode::Node("ValueVec")
 *   {
 *     // option 1: pass constructor arguments for Vec2
 *     this->add_port<Vec2>(gnode::PortType::OUT, "value", x, y);
 *
 *     // option 2: set the value manually
 *     // this->add_port<Vec2>(gnode::PortType::OUT, "value");
 *     // this->set_value<Vec2>("value", Vec2(x, y));
 *   }
 *
 *   void compute() {}
 * };
 *
 * // add two floats
 * class Add : public gnode::Node
 * {
 * public:
 *   Add() : gnode::Node("Add")
 *   {
 *     // two float inputs
 *     this->add_port<float>(gnode::PortType::IN, "a");
 *     this->add_port<float>(gnode::PortType::IN, "b");
 *
 *     // one float output
 *     this->add_port<float>(gnode::PortType::OUT, "a + b");
 *   }
 *
 *   void compute()
 *   {
 *     // access input/output values by reference
 *     float *p_in1 = this->get_value_ref<float>("a");
 *     float *p_in2 = this->get_value_ref<float>("b");
 *     float *p_out = this->get_value_ref<float>("a + b");
 *
 *     // compute only if both inputs are connected/valid
 *     if (p_in1 && p_in2) *p_out = *p_in1 + *p_in2;
 *   }
 * };
 *
 * // sum Vec2 components and returns a float
 * class SumVec : public gnode::Node
 * {
 * public:
 *   SumVec() : gnode::Node("SumVec")
 *   {
 *     // Vec2 input
 *     this->add_port<Vec2>(gnode::PortType::IN, "vec");
 *     // float output
 *     this->add_port<float>(gnode::PortType::OUT, "sum");
 *   }
 *
 *   void compute()
 *   {
 *     Vec2  *p_vec = this->get_value_ref<Vec2>("vec");
 *     float *p_sum = this->get_value_ref<float>("sum");
 *
 *     // sum only if input exists
 *     if (p_vec) *p_sum = p_vec->x + p_vec->y;
 *   }
 * };
 *
 * // print a float
 * class Print : public gnode::Node
 * {
 * public:
 *   Print() : gnode::Node("Print")
 *   {
 *     this->add_port<float>(gnode::PortType::IN, "in");
 *   }
 *
 *   void compute()
 *   {
 *     float *p_in = this->get_value_ref<float>("in");
 *
 *     if (p_in) std::cout << "PRINTING: " << *p_in << "\n";
 *   }
 * };
 *
 * int main()
 * {
 *   gnode::Graph g;
 *
 *   // NB - node constructor parameters are pass through when adding a node
 *   // Create a Vec2 node with initial value (1, 2)
 *   auto id_value_vec = g.add_node<ValueVec>(1.f, 2.f);
 *   auto id_sum_vec = g.add_node<SumVec>();
 *
 *   // sum vector components
 *   g.new_link(id_value_vec, "value", id_sum_vec, "vec");
 *
 *   // add the former sum to a float
 *   auto id_value1 = g.add_node<Value>(3.f);
 *   auto id_add1 = g.add_node<Add>();
 *   g.new_link(id_sum_vec, "sum", id_add1, "a");
 *   g.new_link(id_value1, "value", id_add1, "b");
 *
 *   // print this intermediate value
 *   auto id_print1 = g.add_node<Print>();
 *   g.new_link(id_add1, "a + b", id_print1, "in");
 *
 *   // add the former sum to another float...
 *   auto id_value2 = g.add_node<Value>(4.f);
 *   auto id_add2 = g.add_node<Add>();
 *   g.new_link(id_add1, "a + b", id_add2, "a");
 *   g.new_link(id_value2, "value", id_add2, "b");
 *
 *   // and print the results
 *   auto id_print2 = g.add_node<Print>();
 *   g.new_link(id_add2, "a + b", id_print2, "in");
 *
 *   // once the graph is defined, perform an overall update to ensure a
 *   // clean initial graph state (topological greedy update)
 *   std::cout << "\nOVERALL UPDATE\n";
 *   g.update();
 *
 *   // --- change one node state = only update from this node
 *   // --- propagate changes only to nodes that depend on it
 *   std::cout << "\nNODE UPDATE\n";
 *   g.get_node_ref_by_id<Value>(id_value1)->set_value<float>("value", 10.f);
 *
 *   // incremental update from id_value1
 *   g.update(id_value1);
 *
 *   // export for debugging/visualization
 *   // dot export.dot -Tsvg > output.svg
 *   g.export_to_graphviz();
 *   g.export_to_mermaid();
 *
 *   return 0;
 * }
 * @endcode
 *
 * @section repo_sec Repository
 *
 * GitHub: https://github.com/otto-link/GNode
 */

#pragma once

#include "gnode/data.hpp"
#include "gnode/graph.hpp"
#include "gnode/link.hpp"
#include "gnode/node.hpp"
#include "gnode/port.hpp"
