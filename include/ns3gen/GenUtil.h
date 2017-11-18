#pragma once

#include <string>

class AddressValue {
  std::string m_local;
  std::string m_mask;
  void parse_and_set(const std::string& value);
public:
  AddressValue(const std::string& value);
  std::string GetLocal();
  std::string GetMask();
};
