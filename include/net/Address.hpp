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

#include <json.hpp>

class AddressValue {
  using T = uint32_t;
  T m_local;
  T m_mask;
  void parse_and_set(const std::string& value);
public:
  AddressValue(const std::string& value);
  /// 
  // GetNext: return the address increased that its host-address
  // note: do not return, network/bloadcast address
  AddressValue GetNextHost() const;
  /// 
  // GetNextNetwork: return the address increased that its network-address
  // note: the local-address is initialied that host address "0.0.0.1"
  AddressValue GetNextNetwork() const;
  ///
  // GetLocal: return netmask
  // e.g. "192.168.1.2/24" -> "192.168.1.2"
  std::string GetLocal() const;
  ///
  // GetMask: return netmask
  // e.g. "192.168.1.2/24" -> "192.168.1.0"
  std::string GetNetworkAddress() const;
  ///
  // GetHost: return part of host
  // e.g. "192.168.1.2/24" -> "0.0.0.2"
  std::string GetHost() const;
  ///
  // GetMask: return netmask
  // e.g. "192.168.1.2/24" -> "255.255.255.0"
  std::string GetMask() const;
  bool operator==(const AddressValue& value) const;
};

enum struct AddressType {
  ChannelUnique,
  NetworkUnique,
  //GlobalUnique,
};

class AddrAllocCell {
  size_t offset_;
  size_t size_;
  AddressType addr_type_;
  AddressValue addr_base_;
public:
  AddrAllocCell(size_t offset, size_t size, AddressType type, AddressValue base);
  size_t GetOffset() const ;
  size_t GetSize() const ;
  AddressType GetType() const;
  AddressValue GetBase() const;
  AddressValue At(size_t idx) const;
};

// 10.0.0.0 ~ 10.255.255.255
class AddressAllocator {
  /// now allocated cell list
  static std::list<AddrAllocCell> cell_list_;
  /// find the cell, return reference
  static std::optional<std::reference_wrapper<AddrAllocCell>> FindCell(AddressValue base_addr);
  /// check address
  static bool IsConsistent(AddrAllocCell cell);
public:
  /// allocate a AddressCell
  static AddrAllocCell Alloc(size_t size,
      AddressType type = AddressType::NetworkUnique, AddressValue base = AddressValue("10.0.0.0/8"));
  /// 
  // config: channel config
  static AddrAllocCell Alloc(size_t size, nlohmann::json config);
};
