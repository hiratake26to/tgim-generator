/*
The MIT License

Copyright (c) 2017 Hiratake Lab.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Author: hiratake26to@gmail.com
*/

/**
 * \file Generator_genapp.cpp
 * \author hiratake26to@gmail
 * \date 2017
 */

#include "ns3gen/Generator.hpp"
#include "ns3gen/GenUtil.hpp"

#include "net/Application.hpp"

#include <boost/format.hpp>

// parse json config
#include <json.hpp>
using json = nlohmann::json;



namespace {
  std::string expandArgs(Application app, std::string all_nodes) {
    return (
        boost::format(R"(%1%, %2% , %3%, %4%, %5%, %6%)")
      % (all_nodes+".Get("+app.src.host+")")
      % app.src.port
      % (all_nodes+".Get("+app.dst.host+")")
      % app.dst.port
      % app.sim.start
      % app.sim.stop
      ).str();
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
  lines.push_back("void " + this->name_app_func + "() {");
  lines.indentRight();

/*
setMyTcpApp(Ptr<Node> n, int nport, Ptr<Node> m, int mport, int sim_start, int sim_stop);
*/
  for (const auto& item : apps) {
    const auto& app = item.second;
    // `app.name` is not used, it is only used for identify app
    lines.push_back( "tgim::app::" + app.type + "(" + expandArgs(app , name_all_nodes) + ");" );
  }

#if 0
  for (const auto& item : nodes) {
    const auto& node = item.second;
    if (node.type == NODE_T_IFACE) continue; // already installed in subnet build
    lines.push_back("stack.Install("
                      + this->name_all_nodes + ".Get(" + node.name + ")"
                      + ");" );
  }
#endif


  /// function end
  lines.indentLeft();
  lines.push_back("}");
}
