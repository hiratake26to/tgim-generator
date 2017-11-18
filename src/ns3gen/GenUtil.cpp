#include "ns3gen/GenUtil.h"

#include <iostream>
#include <boost/algorithm/string.hpp>
#include <tao/pegtl.hpp>

namespace pegtl = tao::pegtl;

namespace Address {
  using namespace tao::pegtl;

  struct octet
    : pegtl::plus<pegtl::digit> {};
  struct dot
    : pegtl::one<'.'> {};
  struct local
    : pegtl::seq<octet, dot, octet, dot, octet, dot, octet> {};
  struct mask_cidr
    : pegtl::plus<pegtl::digit> {};
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
        mask = "255.255.255.255";
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

//
// impl
//

void AddressValue::parse_and_set(const std::string& value) {
  std::string local;
  std::string mask;
  bool ok = false;
  {
    std::string buff;
    pegtl::string_input<> in(value, buff);
    pegtl::parse< Address::grammer, Address::action>( in, local, mask, ok );
  }

  /*
  std::cout << "local : " << local << std::endl;
  std::cout << "mask : " << mask << std::endl;
  std::cout << "ok : " << ok << std::endl;
  */

  m_local = local;
  m_mask = mask;

  if (!ok) {
    std::cerr << "Value: " << value << std::endl;
    const char* msg
      = "Wrong Address format, it must be \"<Local>,<Mask>\" or \"<CIDR format>\"";
    std::cerr << __FILE__ << ":" << __LINE__ << msg << std::endl;
    throw std::logic_error(msg);
  }
}

AddressValue::AddressValue(const std::string& value) {
  parse_and_set(value);
}
std::string AddressValue::GetLocal() {
  return m_local;
}
std::string AddressValue::GetMask() {
  return m_mask;
}
