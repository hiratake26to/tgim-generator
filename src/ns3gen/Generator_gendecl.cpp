/*
The MIT License

Copyright (c) 2017 Hiratake Lab.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Author: hiratake26to@gmail.com
*/

/**
 * \file Generator_gendecl.cpp
 * \author hiratake26to@gmail
 * \date 2017
 */

#include "ns3gen/Generator.hpp"
#include "ns3gen/GenUtil.hpp"

#include <boost/range/adaptor/indexed.hpp>
#include <boost/algorithm/string.hpp>

// parse json config
#include <json.hpp>
using json = nlohmann::json;

void NetworkGenerator::gen_decl(CodeSecretary& lines) {
  // generate enum for node
  lines.push_back("// enum node name");
  lines.push_back("enum NodeName {");
  lines.indentRight();
  for (const auto& item : nodes) {
    const auto& node = item.second;
    lines.push_back("" + node.name + ",");
  }
  lines.indentLeft();
  lines.push_back("};");

  // %%
  lines.push_back("");
  
  // section before build
  lines.push_back("/*******************************************************");
  lines.push_back(" * config this if you need before build                *");
  lines.push_back(" ******************************************************/");

  // channel helper
  lines.push_back("// helper");
  std::unordered_map<std::string, std::string> helper_name_map;
  helper_name_map["PointToPoint"] = "PointToPointHelper";
  helper_name_map["Csma"] = "CsmaHelper";
  for (const auto& item : channels) {
    const auto& ch_buf = item.second;
    if ( helper_name_map[ch_buf.type] != "" ) {
      lines.push_back("// Channel : " + ch_buf.name );
      lines.push_back(helper_name_map[ch_buf.type]
                        + " " + ch_buf.name + ";");
    } else {
      if ( ch_buf.nodes.size() < 2 )
      {
        lines.push_back("// [TGIM ERR] Channel '"
                          + ch_buf.name
                          + "' due to node count less then 2.");
      }
      else if ( ch_buf.nodes.size() == 2 )
      {
        lines.push_back("// [TGIM AUTO]");
        lines.push_back(helper_name_map["PointToPoint"]
                          + " " + ch_buf.name + ";");
      }
      else if ( ch_buf.nodes.size() > 2 )
      {
        lines.push_back("// [TGIM AUTO]");
        lines.push_back(helper_name_map["Csma"]
                          + " " + ch_buf.name + ";");
      }
    }
  }

  // subnet
  lines.push_back("// subnet");
  for (const auto& item : subnets) {
    const auto& name = item.first;
    const auto& subnet = item.second;
    lines.push_back("struct " + subnet.GetName() + " " + name + ";" );
  }

  // %%
  lines.push_back("");
  
  // section after build
  lines.push_back("/*******************************************************");
  lines.push_back(" * we can after build use this                         *");
  lines.push_back(" ******************************************************/");
  // all nodes
  lines.push_back("// nodes");
  lines.push_back("NodeContainer " + name_all_nodes + ";");
  // netdevices
  for (const auto& item : channels) {
    const auto& channel = item.second;
    netdevs[channel.name] = "ndc_" + channel.name;
    lines.push_back("NetDeviceContainer " + netdevs[channel.name] + ";");
  }
}
