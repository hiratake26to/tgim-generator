#pragma once

#include "../network-prvt.h"

#define NODE_T_BASIC 0
#define NODE_T_IFACE 1

/** @brief Node */
struct Node {
  int id;
  std::string name;
  std::string config;
  int type;
  std::string subnet_name;  // name as subnet instance
  std::string subnet_class; // subnet class name
  std::string subnet_node_id; // id for subnet class
};
