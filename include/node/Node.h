#pragma once

#include "../network-prvt.h"

#include "InternetStack.h"
#include "Device.h"

/** @brief Node */
class Node {
  // internet stack config
  InternetStack internet_stack;
  // installed devices
  Device device;

  int m_id;
public:
  int SetId(int id) { m_id = id; }
  int GetId() const { return m_id; }
};
