/*
The MIT License

Copyright (c) 2017 Hiratake Lab.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Author: hiratake26to@gmail.com
*/

/**
 * \file Address.cpp
 * \author hiratake26to@gmail
 * \date 2019
 */

#include "net/Address.hpp"

#include <iostream>
#include <string>
#include <boost/algorithm/string.hpp>
#include <tao/pegtl.hpp>
using std::string;
using std::optional;

static string toStrAddr(uint32_t addr) {
  uint8_t oct[4];
  // [0].[1].[2].[3]
  oct[0] = 0xFF & (addr >> 24);
  oct[1] = 0xFF & (addr >> 16);
  oct[2] = 0xFF & (addr >>  8);
  oct[3] = 0xFF & (addr);

  string str_addr
    = std::to_string(oct[0]) + "."
    + std::to_string(oct[1]) + "."
    + std::to_string(oct[2]) + "."
    + std::to_string(oct[3]);

  return str_addr;
}

static uint32_t toBinAddr(string s) {
  uint32_t ret = 0;
  string buf = "";
  int i = 0;
  for (const char& c : s) {
    if (c >= '0' && c <= '9') buf += c;
    else {
      int num = std::atoi(buf.c_str());
      ret |= num << (24-8*i);
      buf = "";
      ++i;
    }
  }
  int num = std::atoi(buf.c_str());
  ret |= num;
  return ret;
}


namespace pegtl = tao::pegtl;

namespace Address {
  using std::string;
  using namespace tao::pegtl;

  struct octet
    : pegtl::rep_min_max<1, 3, pegtl::digit> {};
  struct dot
    : pegtl::one<'.'> {};
  struct local
    : pegtl::seq<octet, dot, octet, dot, octet, dot, octet> {};
  struct mask_cidr
    : pegtl::rep_min_max<1, 2, pegtl::digit> {};
  struct cidr
    : pegtl::seq<local, one<'/'>, mask_cidr> {};
  struct mask
    : pegtl::seq<octet, dot, octet, dot, octet, dot, octet> {};
  struct address_with_mask
    : pegtl::seq<local, one<','>, mask> {};
  struct grammer
    : pegtl::sor<pegtl::seq<cidr,eof>, pegtl::seq<address_with_mask,eof>> {};

  const char *cidr_mask_tbl[] = {
    "000.000.000.000",
    "128.000.000.000",
    "192.000.000.000",
    "224.000.000.000",
    "240.000.000.000",
    "248.000.000.000",
    "252.000.000.000",
    "254.000.000.000",
    "255.000.000.000",
    "255.128.000.000",
    "255.192.000.000",
    "255.224.000.000",
    "255.240.000.000",
    "255.248.000.000",
    "255.252.000.000",
    "255.254.000.000",
    "255.255.000.000",
    "255.255.128.000",
    "255.255.192.000",
    "255.255.224.000",
    "255.255.240.000",
    "255.255.248.000",
    "255.255.252.000",
    "255.255.254.000",
    "255.255.255.000",
    "255.255.255.128",
    "255.255.255.192",
    "255.255.255.224",
    "255.255.255.240",
    "255.255.255.248",
    "255.255.255.252",
    "255.255.255.254",
    "255.255.255.255"
  };

  template< typename Rule >
  struct action
    : pegtl::nothing< Rule >
  {};

  template<>
  struct action< local >
  {
    template< typename Input >
    static void apply( const Input& in, string& local, string& mask, bool &ok)
    {
      local = in.string();
    }
  };

  template<>
  struct action< mask_cidr >
  {
    template< typename Input >
    static void apply( const Input& in, string& local, string& mask, bool &ok)
    {
      int n = std::stol(in.string());
      if (n > 32 || n < 0) {
        //mask = "255.255.255.255";
        throw std::runtime_error("Invalid mask number: " + in.string() + ", CIDR");
      } else {
        mask = cidr_mask_tbl[n];
      }
    }
  };

  template<>
  struct action< mask >
  {
    template< typename Input >
    static void apply( const Input& in, string& local, string& mask, bool &ok)
    {
      mask = in.string();
    }
  };

  template<>
  struct action< grammer >
  {
    template< typename Input >
    static void apply( const Input& in, string& local, string& mask, bool &ok)
    {
      ok = true;
    }
  };
}

namespace {
  // netaddress : last local address
  std::map<string, string> assign_address_list;
}

//
// impl
//

void AddressValue::parse_and_set(const string& value) {
  string local;
  string mask;
  bool ok = false;
  try {
    string buff;
    pegtl::string_input<> in(value, buff);
    pegtl::parse< Address::grammer, Address::action>( in, local, mask, ok );
    if (!ok) throw std::runtime_error("not ok");

  } catch(const std::exception& e) {
    std::cerr << "Exception in parse address: \"" << value << "\"" << std::endl;
    std::cerr << "what: " << e.what() << std::endl;
    const char* msg
      = "Wrong Address format, it must be \"<Local>,<Mask>\" or \"<CIDR format>\"";
    std::cerr << __FILE__ << ":" << __LINE__ << ":" << msg << std::endl;
    throw std::runtime_error(msg);
  }

  /*
  std::cout << "local : " << local << std::endl;
  std::cout << "mask : " << mask << std::endl;
  std::cout << "ok : " << ok << std::endl;
  */

  m_local = toBinAddr(local);
  m_mask = toBinAddr(mask);
}

AddressValue::AddressValue(const string& value) {
  parse_and_set(value);
}
// do not return, network/bloadcast address
AddressValue AddressValue::GetNext() const {
  AddressValue next = *this;
  next.m_local += 1;
  // check reach to bload cast address
  if (next.m_local == ((next.m_local & next.m_mask) | ~next.m_mask)) {
    throw std::runtime_error("Exception at AddressValue::GetNext , "
        "could not increment address due to reach to broadcast address");
  }
  return next;
}
string AddressValue::GetLocal() const {
  return toStrAddr(m_local);
}
string AddressValue::GetNetworkAddress() const {
  return toStrAddr(m_local & m_mask);
}
string AddressValue::GetHost() const {
  return toStrAddr(m_local & ~m_mask);
}
string AddressValue::GetMask() const {
  return toStrAddr(m_mask);
}
bool AddressValue::operator==(const AddressValue& value) const {
  if (m_local == value.m_local && m_mask == value.m_mask) return true;
  return false;
};

//
//
//

std::vector<AddrGenCell> AddressGenerator::gen_cell_list_;
std::optional<std::reference_wrapper<AddrGenCell>> AddressGenerator::gen_cell_;
//AddrGenCell AddressGenerator::temp(AddressValue("192.168.22.0/24"), AddressType::NetworkUnique);

//uint32_t AddressGenerator::address_net;
//uint32_t AddressGenerator::address_mask;
//uint32_t AddressGenerator::address_last;

//void AddressGenerator::Init() {
//  address_net   = 0xC0A80000;
//  address_mask  = 0xFFFFFF00;
//  address_last  = address_net + ~address_mask + 1;
//}
string AddressGenerator::GetLocal() {
  return gen_cell_.value().get().GetLast().GetLocal();
  //return toStrAddr(address_last);
}
string AddressGenerator::GetNetworkAddress() {
  return gen_cell_.value().get().GetLast().GetNetworkAddress();
  //return toStrAddr(address_net & address_mask);
}
string AddressGenerator::GetHost() {
  return gen_cell_.value().get().GetLast().GetHost();
  //return toStrAddr(address_net & ~address_mask);
}
string AddressGenerator::GetMask() {
  return gen_cell_.value().get().GetLast().GetMask();
  //return toStrAddr(address_mask);
}
// find cell that has same network address
optional<std::reference_wrapper<AddrGenCell>> AddressGenerator::FindGenCell(AddressValue base_value) {
  for (auto&& i : gen_cell_list_) {
    if (i.GetBase().GetNetworkAddress() == base_value.GetNetworkAddress()) {
      return i;
    }
  }
  return {};
}
bool AddressGenerator::IsConsistent(AddrGenCell cell) {
  AddressValue base = cell.GetBase();
  AddressType type = cell.GetType();
  const auto& result = FindGenCell(base);
  if (result) {
    if (result.value().get().GetType() != type) return false;
  }
  return true;
}
void AddressGenerator::SetBase(AddressValue value, AddressType type) {
  AddrGenCell cell(value, type);
  if (not IsConsistent(cell)) {
    throw std::runtime_error("Exception at AddressGenerator::SetBase, could not set base address.\n"
        "for same network address, must be specified different type");
  }
  auto&& result = FindGenCell(value);
  if (not result) {
    gen_cell_list_.push_back(cell);
  }
  gen_cell_ = FindGenCell(value);
}
void AddressGenerator::Next() {
  gen_cell_.value().get().Next();
  //address_last  = address_last + ~address_mask + 1;
}
void AddressGenerator::SetDefault() {
  SetBase(AddressValue("10.0.0.0/8"), AddressType::NetworkUnique);
}

//

AddrGenCell::AddrGenCell(AddressValue base_addr, AddressType type)
: base_addr_(base_addr), addr_(base_addr), type_(type)
{
  // initialize
}
AddressValue AddrGenCell::GetBase() const { return base_addr_; }
AddressValue AddrGenCell::GetLast() const { return addr_; }
AddressType AddrGenCell::GetType() const { return type_; }
void AddrGenCell::Next() {
  addr_ = addr_.GetNext();
}
