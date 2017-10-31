#pragma once

#include "../network-prvt.h"

#include "InternetStack.h"
#include "Device.h"
#include <string>

/** @brief Node */
struct Node {
  int id;
  std::string name;

  std::string config;
};
