/*
The MIT License

Copyright 2019 hiratake26to

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/**
 * \file Generator_genapp.cpp
 * \author hiratake26to@gmail
 * \date 2017
 */

#include "ns3gen/Generator.hpp"

#include "net/Application.hpp"

#include "loader/AppModelLoader.hpp"

#include <numeric>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

// parse json config
#include <json.hpp>
using json = nlohmann::json;

///
#include <tao/pegtl.hpp>
#define RANGE(a) a.begin(), a.end()

namespace pegtl = tao::pegtl;

namespace argument {
  using namespace tao::pegtl;

  struct var_id
    : pegtl::plus< pegtl::not_one<'}'> > {};
  struct var_lit
    : pegtl::seq< pegtl::one<'$'>, pegtl::one<'{'>, var_id, pegtl::one<'}'> > {};
  struct raw
    : pegtl::not_one<'$'> {};
  struct content
    : pegtl::star< pegtl::sor<raw, var_lit > > {};
  struct grammer
    : pegtl::seq< content, eof > {};

  template< typename Rule >
  struct action
    : pegtl::nothing< Rule >
  {};

  template<>
  struct action< raw >
  {
    template< typename Input >
    static void apply( const Input& in, std::string& result,
      const std::string& node_container,
      const std::map<std::string, Node>& nodes )
    {
      result += (in.string());
    }
  };

  template<>
  struct action< var_id >
  {
    template< typename Input >
    static void apply( const Input& in, std::string& result,
      const std::string& node_container,
      const std::map<std::string, Node>& nodes )
    {
      if ( nodes.count(in.string()) ) {
        result += (node_container + ".Get(" + nodes.at(in.string()).name + ")");
      } else {
        throw std::runtime_error("not found the node of name '" + in.string() + "'");
      }
    }
  };

  template<>
  struct action< grammer >
  {
    template< typename Input >
    static void apply( const Input& in, std::string& result,
      const std::string& node_container,
      const std::map<std::string, Node>& nodes )
    {
    }
  };
}


namespace {
  std::string resolveHolder(
      const std::string& str,
      const std::string& node_container,
      const std::map<std::string, Node>& nodes
  ){
    std::string ret;
    std::string buff;
    pegtl::string_input<> in(str, buff);
    pegtl::parse< argument::grammer, argument::action>( in, ret, node_container, nodes );
    if (ret != str) {
      boost::trim_if (ret, boost::is_any_of("\"") );
    }
    return ret;
  }
  void expandParams(
      CodeSecretary& lines,
      const AppModelLoader& loader,
      const std::string& instance_name,
      const Application& app,
      const std::string& node_container,
      const std::map<std::string, Node>& nodes
  ){
    // load application model
    auto loaded = loader.load();

    json jargs;
    for (auto& elem : loaded) {
      // search
      if (elem["type"] == app.type) {
        jargs = elem["args"];
        break;
      }
    }

    for (auto it = jargs.begin(); it != jargs.end(); ++it) {
      //std::cout << app.args[it.key()] << std::endl;
      lines.push_back( (boost::format(R"(%1%.Set_%2%(%3%);)")
            % instance_name
            % it.key()
            % resolveHolder( app.args.at(it.key()), node_container, nodes ) // TODO: impl, generator replace literals.
            ).str() );
    }
  }
  void expandParams2(
      CodeSecretary& lines,
      const std::string& instance_name,
      const Application& app,
      const std::string& node_container,
      const std::map<std::string, Node>& nodes
  ){

    for (const auto& key_and_val : app.args) {
      lines.push_back( (boost::format(R"(%1%.Set_%2%(%3%);)")
            % instance_name
            % key_and_val.first
            % resolveHolder( key_and_val.second, node_container, nodes ) // TODO: impl, generator replace literals.
            ).str() );
    }
  }
}

void NetworkGenerator::gen_app(CodeSecretary& lines) {
  //
  // function begin
  //
  lines.push_back("");
  lines.push_back("/*******************************************************");
  lines.push_back(" * application build function                          *");
  lines.push_back(" ******************************************************/");
  lines.push_back("void " + m_name_app_func + "() {");
  lines.indentRight();

/*
setMyTcpApp(Ptr<Node> n, int nport, Ptr<Node> m, int mport, int sim_start, int sim_stop);
*/
  for (const auto& item : m_apps) {
    const auto& app = item.second;
    // `app.name` is not used, it is only used for identify app
    lines.push_back("{");
    lines.indentRight();
      std::string instance_name = "__tgim_app_" + app.type;
      lines.push_back( "tgim::app::" + app.type + " " + instance_name
          + "(\"" + instance_name + "\");");

      expandParams2(lines, instance_name, app , m_name_node_container, m_nodes);
      lines.push_back( instance_name + ".Install("
          + m_name_node_container + ".Get(" + app.install + ")"
          +");" );
    lines.indentLeft();
    lines.push_back("}");
  }

#if 0
  for (const auto& item : nodes) {
    const auto& node = item.second;
    if (node.type == NODE_T_IFACE) continue; // already installed in subnet build
    lines.push_back("stack.Install("
                      + this->m_name_node_container + ".Get(" + node.name + ")"
                      + ");" );
  }
#endif


  /// function end
  lines.indentLeft();
  lines.push_back("}");
}
