/*
The MIT License

Copyright (c) 2017 Hiratake Lab.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Author: hiratake26to@gmail.com
*/

/**
 * \file GenUtil.cpp
 * \author hiratake26to@gmail
 * \date 2017
 */

#include "ns3gen/GenUtil.hpp"

#include <iostream>
#include <boost/algorithm/string.hpp>
#include <tao/pegtl.hpp>

static std::string toStrAddr(uint32_t addr) {
  uint8_t oct[4];
  // [0].[1].[2].[3]
  oct[0] = 0xFF & (addr >> 24);
  oct[1] = 0xFF & (addr >> 16);
  oct[2] = 0xFF & (addr >>  8);
  oct[3] = 0xFF & (addr);

  std::string str_addr
    = std::to_string(oct[0]) + "."
    + std::to_string(oct[1]) + "."
    + std::to_string(oct[2]) + "."
    + std::to_string(oct[3]);

  return str_addr;
}

static uint32_t toBinAddr(std::string s) {
  uint32_t ret = 0;
  std::string buf = "";
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
    static void apply( const Input& in, std::string& local, std::string& mask, bool &ok)
    {
      local = in.string();
    }
  };

  template<>
  struct action< mask_cidr >
  {
    template< typename Input >
    static void apply( const Input& in, std::string& local, std::string& mask, bool &ok)
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
    static void apply( const Input& in, std::string& local, std::string& mask, bool &ok)
    {
      mask = in.string();
    }
  };

  template<>
  struct action< grammer >
  {
    template< typename Input >
    static void apply( const Input& in, std::string& local, std::string& mask, bool &ok)
    {
      ok = true;
    }
  };
}

namespace {
  // netaddress : last local address
  std::map<std::string, std::string> assign_address_list;
}

//
// impl
//

void AddressValue::parse_and_set(const std::string& value) {
  std::string local;
  std::string mask;
  bool ok = false;
  try {
    std::string buff;
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

AddressValue::AddressValue(const std::string& value) {
  parse_and_set(value);
}
std::string AddressValue::GetLocal() {
  return toStrAddr(m_local);
}
std::string AddressValue::GetNetworkAddress() {
  return toStrAddr(m_local & m_mask);
}
std::string AddressValue::GetHost() {
  return toStrAddr(m_local & ~m_mask);
}
std::string AddressValue::GetMask() {
  return toStrAddr(m_mask);
}

//
//
//

uint32_t AddressGenerator::address_net;
uint32_t AddressGenerator::address_mask;
uint32_t AddressGenerator::address_last;

void AddressGenerator::Init() {
  address_net   = 0xC0A80000;
  address_mask  = 0xFFFFFF00;
  address_last  = address_net + ~address_mask + 1;
}
std::string AddressGenerator::GetLocal() {
  return toStrAddr(address_last);
}
std::string AddressGenerator::GetNetworkAddress() {
  return toStrAddr(address_net & address_mask);
}
std::string AddressGenerator::GetHost() {
  return toStrAddr(address_net & ~address_mask);
}
std::string AddressGenerator::GetMask() {
  return toStrAddr(address_mask);
}
void AddressGenerator::Next() {
  address_last  = address_last + ~address_mask + 1;
}
