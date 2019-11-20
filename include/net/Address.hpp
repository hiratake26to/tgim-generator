/*
The MIT License

Copyright (c) 2017 Hiratake Lab.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Author: hiratake26to@gmail.com
*/

/**
 * \file Address.hpp
 * \brief Network Address
 * \author hiratake26to@gmail
 * \date 2019
 */

#pragma once

#include "network-prvt.hpp"
#include <optional>

class AddressValue {
  uint32_t m_local;
  uint32_t m_mask;
  void parse_and_set(const std::string& value);
public:
  AddressValue(const std::string& value);
  AddressValue GetNext() const;
  std::string GetLocal() const;
  std::string GetNetworkAddress() const;
  std::string GetHost() const;
  std::string GetMask() const;
  bool operator==(const AddressValue& value) const;
};

enum struct AddressType {
  ChannelUnique,
  NetworkUnique,
  //GlobalUnique,
};

class AddrGenCell {
  AddressValue base_addr_;
  AddressValue addr_;
  AddressType type_;
public:
  AddrGenCell(AddressValue base_addr, AddressType type);
  AddressValue GetBase() const;
  AddressValue GetLast() const;
  AddressType GetType() const;
  void Next();
};

class AddressGenerator {
  static std::vector<AddrGenCell> gen_cell_list_;
  static std::optional<std::reference_wrapper<AddrGenCell>> gen_cell_;
  //static AddrGenCell temp;
  static std::optional<std::reference_wrapper<AddrGenCell>> FindGenCell(AddressValue base_value);
  static bool IsConsistent(AddrGenCell cell);
  // 10.0.0.0 ~ 10.255.255.255
  //static uint32_t address_net;
  //static uint32_t address_mask;
  //static uint32_t address_last;
public:
  static std::string GetLocal();
  static std::string GetNetworkAddress();
  static std::string GetHost();
  static std::string GetMask();
  static void SetBase(AddressValue value, AddressType type);
  static void SetDefault();
  static void Next();
};
