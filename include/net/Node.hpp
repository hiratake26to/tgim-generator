/*
The MIT License

Copyright (c) 2017 Hiratake Lab.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Author: hiratake26to@gmail.com
*/

/**
 * \file Node.hpp
 * \brief Network Node Model.
 * \author hiratake26to@gmail
 * \date 2017
 */

#pragma once

#include "network-prvt.hpp"

enum class node_role_type : int {
  normal,
  csma_switch,
  wifi_adhoc,
  wifi_ap,
  wifi_sta,
};

struct Netif {
  std::string connect;
  node_role_type as;
};


enum class node_type : int {
  basic,
  subnet,
};

/** @brief Node */
struct Node {
  // id, name, config
  int id;
  std::string name;
  std::string config;

  // type (Basic, Subnet)
  node_type type;

  // point.x .y .z
  int x; int y; int z;

  // netifs
  std::vector<Netif> netifs;
  
  //
  std::string subnet_name;  // name as subnet instance
  std::string subnet_class; // subnet class name
  std::string subnet_node_id; // id for subnet class

  //
  Node() = default;
  Node(int id, std::string name, std::string config = "");
  void SetPoint(Vector3D vec);
};
