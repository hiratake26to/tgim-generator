/*
The MIT License

Copyright (c) 2017 Hiratake Lab.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Author: hiratake26to@gmail.com
*/

/**
 * \file Generator.cpp
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

NetworkGenerator::NetworkGenerator(Network net)
{
  // nodes
  this->nodes = net.GetNodes();
  // subnets
  this->subnets = net.GetSubnets();
  // channels
  this->channels = net.GetChannels();
  // name
  this->name = net.GetName();
  // type
  this->net_type = net.GetType();
}

std::vector<std::string> NetworkGenerator::CppCode(std::string out_filename) {
  std::ifstream ifs("./resource/ns3template-cxx.json");
  ns3template.clear();
  ifs >> ns3template;
  ifs.close();

  CodeSecretary lines;

  // namespace begin
  lines.push_back("namespace tgim {");
  // struct struct
  lines.push_back("struct " + name + " {");
  lines.indentRight();

  // declare
  gen_decl(lines);

  // build function
  gen_build(lines);

  lines.indentLeft();
  // struct end
  lines.push_back("};");
  // namespace end
  lines.push_back("}");

  std::ofstream ofs("./out/"+out_filename+".hpp");
  for (auto line : lines.get()) {
    ofs << line << std::endl;
  }
  ofs.close();

  return lines.get(); 
}

void NetworkGenerator::gen_decl(CodeSecretary& lines) {
  lines.push_back("/*******************************************************");
  lines.push_back(" * we can always use                                   *");
  lines.push_back(" ******************************************************/");

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

void NetworkGenerator::gen_build(CodeSecretary& lines) {
  // make config table
  // config_at["channel"] = {"Delay", "DataRate"}
  std::map<std::string, std::vector<std::string>> config_at;
  for (auto it = ns3template.begin(); it != ns3template.end(); ++it) {
    for (auto at = it.value()["at"].begin(); at != it.value()["at"].end(); ++at) {
      config_at[at.key()].push_back(it.key());
    }
  }

  //
  // function begin
  //
  lines.push_back("");
  lines.push_back("/*******************************************************");
  lines.push_back(" * build function                                      *");
  lines.push_back(" ******************************************************/");
  lines.push_back("void " + this->name_build_func + "() {");
  lines.indentRight();

  //
  // config channel
  //
  std::cout << "***CONFIG CHANNEL***" << std::endl;

  lines.push_back("// config channel");
  for (const auto& item : channels) {
    Channel ch = item.second;

    try {
    /* begin */
    auto jconf = json::parse(ch.config);

    // config at channel
    for (auto attr : config_at["channel"])
    { // ** for begin **
      // prototype 2
      // find helper method
      json jhelper_method = ns3template["helper"] [ch.type];
      std::cout << jhelper_method.dump(2) << std::endl;
      for (auto jmethod_attr = jhelper_method.begin()
          ; jmethod_attr != jhelper_method.end()
          ; ++jmethod_attr)
      {
        // declare variable for function params
        std::string method;
        std::string value;
        // set method
        method = jmethod_attr.key();
        // find attr from attributes list
        json jattr_list = ns3template["attributes"]
                                [jmethod_attr.value().get<std::string>()];
        // set each attr
        for (auto jattr = jattr_list.begin()
            ; jattr != jattr_list.end()
            ; ++jattr)
        {
          if (attr != jattr.key()) continue;
          // set value
          value = jconf[attr];
          // generate line
          lines.push_back( ch.name
                        + "." + method
                        + "(\""
                          + attr
                        + "\","
                          + "StringValue(\"" + value + "\")"
                        + ");");
        }
      }
    } // ** for end **

    /* end */
    } catch(const std::exception& e) {
      std::cerr << __FILE__ << ":" << __LINE__ << ": " << e.what() << std::endl;
    }
  }

  //
  // generate all subnet
  //
  std::cout << "***BUILD ALL SUBNETS***" << std::endl;

  lines.push_back("// build all subnets");
  for (const auto& item : subnets) {
    const auto& name = item.first;
    lines.push_back(name + ".build();" );
  }

  //
  // create all nodes
  //
  std::cout << "***CREATE ALL NODES***" << std::endl;

  lines.push_back("// create all nodes");
  for (const auto& item : nodes) {
    const auto& node = item.second;
    std::string added = "CreateObject<Node>()";
    if ( node.type == NODE_T_IFACE ) {
      added = node.subnet_name + ".nodes.Get("+node.subnet_class+"::"+node.subnet_node_id+")";
    }
    lines.push_back("// - " + node.name);
    lines.push_back(this->name_all_nodes
                  + ".Add("
                  + added
                  + ");");
  }

  // name each node
  /*
  lines.push_back("// name each node");
  for (const auto& elem : nodes | boost::adaptors::indexed()) {
    const auto& node = elem.value().second;
    const auto& i = std::to_string(elem.index());
    lines.push_back(node.name + " = "
                  + this->name_all_nodes + ".Get(" + i + ");");
  }
  */

  //
  // connect nodes via link
  // 
  std::cout << "***CONNECT LINK***" << std::endl;

  lines.push_back("// connect link");
  // -- connector generate
  std::unordered_map<std::string, std::function<void(Channel&)>> connector;
  connector["PointToPoint"]
    = [=, &lines](Channel& channel) { 
			if (channel.nodes.size() < 2) {
				lines.push_back( "// " + netdevs[channel.name]
																+ ": not connected due to node count less than 2" );
				return;
			}
      lines.push_back( netdevs[channel.name] + " = "
                        + channel.name + ".Install("
                        + this->name_all_nodes + ".Get(" + channel.nodes[0] + ")"
                        + ","
                        + this->name_all_nodes + ".Get(" + channel.nodes[1] + ")"
                      + ");" );
    };
  connector["Csma"]
    = [=, &lines](Channel& channel) { 
      // make node container
      lines.push_back( "{" );
      lines.indentRight();
      lines.push_back( "NodeContainer nc_local;" );
      // add node
      for (const auto& node : channel.nodes) {
        lines.push_back( "nc_local.Add("
                        + this->name_all_nodes + ".Get(" + node + ")"
                        + ");" );
      }
      // connect csma
      lines.push_back( netdevs[channel.name] + " = "
                        + channel.name + ".Install(nc_local);" );
      lines.indentLeft();
      lines.push_back( "}" );
    };
  // -- connect node to channel
  for (const auto& item : channels) {
    Channel ch_buf = item.second;
    if ( connector[ch_buf.type] )
    {
      connector[ch_buf.type](ch_buf);
    }
    else // auto set channel type, this will been generated latest for loop
    {
      // generate added channnel name
      std::string added_ch_name = "link_auto_"+std::to_string(channels.size());

      if ( ch_buf.nodes.size() < 2 )
      {
        lines.push_back("// [TGIM ERR] Channel '"
                          + ch_buf.name
                          + "' is node count is less then 2.");
      }
      else if ( ch_buf.nodes.size() == 2 )
      {
        auto_channels[ch_buf.name] = ch_buf;
      }
      else if ( ch_buf.nodes.size() > 2 )
      {
        auto_channels[ch_buf.name] = ch_buf;
      }
    }
  }
  for (const auto& item : auto_channels) {
    Channel ch_buf = item.second;
    if ( connector[ch_buf.type] )
    {
      lines.push_back("// [TGIM AUTO]");
      connector[ch_buf.type](ch_buf);
    }
  }

  //
  // install Internet stack
  //
  lines.push_back("// install internet stack");
  lines.push_back("InternetStackHelper stack;");
  for (const auto& item : nodes) {
    const auto& node = item.second;
    if (node.type == NODE_T_IFACE) continue; // already installed in subnet build
    lines.push_back("stack.Install("
                      + this->name_all_nodes + ".Get(" + node.name + ")"
                      + ");" );
  }

  //
  // assign IP address
  //
  std::cout << "***ASSIGN IP***" << std::endl;

  lines.push_back("// ");
  lines.push_back("NS_LOG_INFO (\"Assign ip addresses.\");");
  /* // assign ip addresses
   * NS_LOG_INFO ("Assign ip addresses.");
   * Ipv4AddressHelper ip;
   * ip.SetBase ("192.168.1.0", "255.255.255.0");
   * Ipv4InterfaceContainer addresses = ip.Assign (devs); */
  lines.push_back("{ Ipv4AddressHelper ip;");
  lines.indentRight();
  for (const auto& item : channels) {
    Channel ch = item.second;

    std::string nd = netdevs[ch.name];
    lines.push_back("// " + nd);
    json jconf;
    if (ch.config.empty()) {
			lines.push_back("// config is empty");
      continue;
		}
    jconf = json::parse(ch.config);
    std::vector<std::string> splited;
    if (!jconf["Address"].is_string()) {
			lines.push_back("// not set address");
      continue;
    }
    std::string str_addr = jconf["Address"].get<std::string>();
    AddressValue addr(str_addr);
    std::string base_ip = addr.GetLocal();
    std::string base_mask = addr.GetMask();
    // set base
    lines.push_back( (std::string)"ip.SetBase ("
                      + "\"" + base_ip + "\""
                      + ","
                      + "\"" + base_mask + "\""
                      + ");" );
    // ip assign
    //lines.push_back ( "Ipv4InterfaceContainer addresses = ip.Assign (" + nd + ");" );
    lines.push_back ( "ip.Assign (" + nd + ");" );
  }
  lines.indentLeft();
  lines.push_back("}");

  //
  // routing
  // 
  std::cout << "***ROUTING***" << std::endl;
  lines.push_back( "NS_LOG_INFO (\"Initialize Global Routing.\");" );
  lines.push_back( "Ipv4GlobalRoutingHelper::PopulateRoutingTables ();" );
  lines.push_back( "Ipv4GlobalRoutingHelper::RecomputeRoutingTables ();" );
  
  //
  // applications
  //

  // ...

  /// function end
  lines.indentLeft();
  lines.push_back("}");
}
