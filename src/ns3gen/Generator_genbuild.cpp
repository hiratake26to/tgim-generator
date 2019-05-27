/*
The MIT License

Copyright (c) 2017 Hiratake Lab.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Author: hiratake26to@gmail.com
*/

/**
 * \file Generator_genbuild.cpp
 * \author hiratake26to@gmail
 * \date 2017
 */

#include "ns3gen/Generator.hpp"
#include "ns3gen/GenUtil.hpp"

#include <iostream>
#include <algorithm>
#include <utility>
#include <boost/range/adaptor/indexed.hpp>
#include <boost/algorithm/string.hpp>

// parse json config
#include <json.hpp>
using json = nlohmann::json;

// utility
static bool findRole(const Node& node, const std::vector<std::string>& roles) {
  std::cout << "node " << node.name << " has roles { ";
  for (const auto& ni : node.netifs) {
    std::cout << ni.as << ", ";
  }
  std::cout << "}" << std::endl;

  auto p = std::find_if(node.netifs.begin(), node.netifs.end()
      , [&roles](Netif nif){
        for (const auto& role : roles) {
          if (nif.as == role) return true;
        }
      });
  return (p != node.netifs.end()); // found!
}

// return list of pair(nic_id, connect_to)
std::vector<std::pair<int,std::string>>
getBridgeNics(const Node& node) {
  std::vector<std::pair<int, std::string>> nics;
  int i = 0;
  for (const auto& ni : node.netifs) {
    if (ni.as == "Switch") {
      nics.push_back(std::make_pair(i, ni.connect));
    }
    i++;
  }
  return nics;
}

int getNicIdFromChannel(const Node& node, std::string connect_to, std::map<std::string,Channel>m_channels) {
  Channel ch = m_channels[connect_to]; // [TODO] CHECK VALID!
  int i = 0;
  for (auto&& it = ch.nodes.begin(); it != ch.nodes.end(); ++it) {
    if (it->name == node.name) return i;
    ++i;
  }

  return -1;
}

std::vector<int> collectNodeHasIp(Channel channel)
{
  std::vector<int> ret;
  int idx = 0;
  std::cout << "[DEBUG] called collectNodeHasIp" << std::endl;
  for (const auto& node : channel.nodes) {
    std::cout << (std::string)node << std::endl;
    if (not findRole(node, {"Switch"})) {
      ret.push_back(idx);
    }
    ++idx;
  }
  return ret;
}

std::vector<std::string>
buildBridgeCode(const Node& node, std::vector<std::pair<int,std::string>> nics
    , const std::map<std::string,Channel>& m_channels) {
#if 0
    { // [TODO] now working
      // bridge install
      Ptr<Node> bridge_node = nodes.Get(sw);
      NetDeviceContainer bridge_ndc;
      bridge_ndc.Add (ndc_c0.Get(2));
      bridge_ndc.Add (ndc_c1.Get(2));
      BridgeHelper bridge;
      bridge.Install (bridge_node, bridge_ndc);
    }
#endif
  // debug
  std::cout << "debug print for channel" << std::endl;
  for (const auto& i : m_channels) {
    const auto& name = i.first;
    const auto& channel = i.second;

    std::cout << "name: " << name << std::endl;
    std::cout << "channel["+name+"]: " << (std::string)channel << std::endl;
    for (const auto& j : channel.nodes) {
      std::cout << "channel.node["+j.name+"]: " << (std::string)j << std::endl;
    }
    std::cout << "---" << std::endl;
  }

  // 
  CodeSecretary code;
  code.push_back("{");
  code.indentRight();

  code.push_back("// [NOW IMPLEMENTATING] buildBridgeCode (this is dummy)");
  code.push_back("Ptr<Node> bridge_node = nodes.Get("+node.name+");");
  code.push_back("NetDeviceContainer bridge_ndc;");
  for (const auto& i : nics) {
    const auto& connect_to = i.second;
    int id = getNicIdFromChannel(node, connect_to, m_channels);
    code.push_back("bridge_ndc.Add(ndc_"+connect_to+".Get("+std::to_string(id)+"));");
  }
  code.push_back("BridgeHelper bridge;");
  code.push_back("bridge.Install(bridge_node, bridge_ndc);");

  code.indentLeft();
  code.push_back("}");
  return code.get();
}

// main
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
  lines.push_back("void " + m_name_build_func + "() {");
  lines.indentRight();

  //
  // config channel
  //
  //std::cout << "***CONFIG CHANNEL***" << std::endl;

  lines.push_back("// config channel");
  for (const auto& item : m_channels) {
    Channel ch = item.second;

    if ( ch.config == "") {
      std::cerr << "WARNING: chennel config is empty in \"" << ch.name  << "\"" << std::endl;
    }
    try {
    /* begin */
    auto jconf = json::parse(ch.config);

    // config at channel
    for (auto attr : config_at["channel"])
    { // ** for begin **
      // prototype 2
      // find helper method
      json jhelper_method = ns3template["helper"] [ch.type];
      //std::cout << jhelper_method.dump(2) << std::endl;
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
  //std::cout << "***BUILD ALL SUBNETS***" << std::endl;

  lines.push_back("// build all subnets");
  for (const auto& item : m_subnets) {
    const auto& name = item.first;
    lines.push_back(name + ".build();" );
    lines.push_back(name + ".app();" );
  }

  //
  // create all nodes
  //
  //std::cout << "***CREATE ALL NODES***" << std::endl;

  lines.push_back("// create all nodes");
  for (const auto& item : m_nodes) {
    const auto& node = item.second;
    std::string added = "CreateObject<Node>()";
    if ( node.type == NODE_T_IFACE ) {
      added = node.subnet_name + ".nodes.Get("+node.subnet_class+"::"+node.subnet_node_id+")";
    }
    lines.push_back("{ // - " + node.name);
    lines.indentRight();

    lines.push_back(m_name_all_nodes
                  + ".Add("
                  + added
                  + ");");

    // Add node name that use logging
    lines.push_back(std::string()
                  + "Names::Add("
                  + "\""+node.name+"\","
                  + "nodes.Get("+node.name+")"
                  + ");");

    // if list of node.netifs.as is {Adhoc, Ap, Sta} then install mobility and allocate position.
    // [NOTE] Adding all nodes
    if ( findRole(node, {"Adhoc", "Ap", "Sta"})
      || true ) {
      // install mobility
      lines.push_back("// Install mobility");
      lines.push_back("installMobility(nodes.Get("+node.name+"));");
      lines.push_back("allocateFixedPos(nodes.Get("+node.name+"), "
          + std::to_string(node.x) + ", "
          + std::to_string(node.y) + ", "
          + std::to_string(node.z) + ");");
    }

    lines.indentLeft();
    lines.push_back("}");
  }

  // name each node
  /*
  lines.push_back("// name each node");
  for (const auto& elem : nodes | boost::adaptors::indexed()) {
    const auto& node = elem.value().second;
    const auto& i = std::to_string(elem.index());
    lines.push_back(node.name + " = "
                  + m_name_all_nodes + ".Get(" + i + ");");
  }
  */

  //
  // connect nodes via link
  // 
  //std::cout << "***CONNECT LINK***" << std::endl;

  lines.push_back("// connect link");
  // -- connector generate
  std::unordered_map<std::string, std::function<void(Channel&)>> connector;
  connector["PointToPoint"]
    = [=, &lines](Channel& channel) { 
      lines.push_back( "{ // - "+channel.name );
      lines.indentRight();
			if (channel.nodes.size() < 2) {
				lines.push_back( "// " + m_netdevs[channel.name]
																+ ": not connected because number of node is less than 2" );
        std::cerr << "Warning! " + m_netdevs[channel.name]
																+ ": not connected because number of node is less than 2" << std::endl;
				return;
			}
			else if (channel.nodes.size() > 2) {
        std::string emsg = "Error! " + m_netdevs[channel.name]
																+ ": not connected bacause number of node is more than 2\n"
                                + "PointToPoint channel can be connected to two node";
        throw std::runtime_error(emsg);
				return;
			}
      lines.push_back( m_netdevs[channel.name] + " = "
                        + channel.name + ".Install("
                        + m_name_all_nodes + ".Get(" + channel.nodes[0].name + ")"
                        + ","
                        + m_name_all_nodes + ".Get(" + channel.nodes[1].name + ")"
                      + ");" );
      lines.indentLeft();
      lines.push_back( "}" );
    };
  connector["Csma"]
    = [=, &lines](Channel& channel) { 
      // make node container
      lines.push_back( "{ // - "+channel.name );
      lines.indentRight();
      lines.push_back( "NodeContainer nc_local;" );
      // add node
      for (const auto& node : channel.nodes) {
        lines.push_back( "nc_local.Add("
                        + m_name_all_nodes + ".Get(" + node.name + ")"
                        + ");" );
      }
      // connect csma
      lines.push_back( m_netdevs[channel.name] + " = "
                        + channel.name + ".Install(nc_local);" );
      lines.indentLeft();
      lines.push_back( "}" );
    };
  connector["Wifi"]
    = [=, &lines](Channel& channel) { 
      // make node container
      lines.push_back( "{ // - "+channel.name );
      lines.indentRight();
      lines.push_back( "NodeContainer nc_local;" );
      // add node
      for (const auto& node : channel.nodes) {
        lines.push_back( "nc_local.Add("
                        + m_name_all_nodes + ".Get(" + node.name + ")"
                        + ");" );
      }
      // connect Wifi
      lines.push_back( m_netdevs[channel.name] + " = "
                        + "WifiAdhocInstall("+channel.name+", nc_local);" );
      lines.indentLeft();
      lines.push_back( "}" );
    };
  connector["WifiApSta"]
    = [=, &lines](Channel& channel) { 
      // make node container
      lines.push_back( "{ // - "+channel.name );
      lines.indentRight();
      lines.push_back( "NodeContainer nc_local_ap;" );
      lines.push_back( "NodeContainer nc_local_stas;" );
      // add AP node
      for (const auto& node : channel.nodes) if (node.type == NODE_T_AP) {
        // [TODO] check that add just one AP node.
        lines.push_back( "nc_local_ap.Add("
                        + m_name_all_nodes + ".Get(" + node.name + ")"
                        + ");" );
      }
      // add STA node
      for (const auto& node : channel.nodes) if (node.type == NODE_T_STA){
        lines.push_back( "nc_local_stas.Add("
                        + m_name_all_nodes + ".Get(" + node.name + ")"
                        + ");" );
      }
      // connect Wifi
      lines.push_back( m_netdevs[channel.name] + " = "
                        + "WifiApStaInstall("+channel.name+", nc_local_ap, nc_local_stas);" );
      lines.indentLeft();
      lines.push_back( "}" );
    };
  // -- connect node to channel
  //(preprocess to deside auto channel)
  for (const auto& item : m_channels) {
    Channel ch_buf = item.second;
    if (not connector[ch_buf.type] )
    {
      // generate added channnel name
      std::string added_ch_name = "link_auto_"+std::to_string(m_channels.size());

      if ( ch_buf.nodes.size() < 2 )
      {
        //lines.push_back("// [TGIM ERR] Channel '"+ ch_buf.name +"' is node count is less then 2.");
        std::cout << "[warning] Channel '"+ ch_buf.name +"' is node count is less then 2." << std::endl;
      }
      else if ( ch_buf.nodes.size() == 2 )
      {
        m_auto_channels[ch_buf.name] = ch_buf;
      }
      else if ( ch_buf.nodes.size() > 2 )
      {
        m_auto_channels[ch_buf.name] = ch_buf;
      }
    }
  }
  for (const auto& item : m_channels) {
    Channel ch_buf = item.second;
    if ( connector[ch_buf.type] )
    {
      connector[ch_buf.type](ch_buf);
      // collect netdevs which has ip (e.g. "ndc_c0_has_ip.Add(ndc_c0.Get(0))", 0 of index of that node has ip)
      for (const auto& idx : collectNodeHasIp(ch_buf)) {
        lines.push_back( m_netdevs_has_ip[ch_buf.name]+".Add("+m_netdevs[ch_buf.name]+".Get("+std::to_string(idx)+"));" );
      }
    }
  }
  for (const auto& item : m_auto_channels) {
    Channel ch_buf = item.second;
    if ( connector[ch_buf.type] )
    {
      lines.push_back("// [TGIM] auto decision channel type");
      connector[ch_buf.type](ch_buf);
      // collect netdevs which has ip (e.g. "ndc_c0_has_ip.Add(0)", 0 of index of that node has ip)
      for (const auto& idx : collectNodeHasIp(ch_buf)) {
        lines.push_back( m_netdevs_has_ip[ch_buf.name]+".Add("+std::to_string(idx)+");" );
      }
    }
  }

  //
  // install bridge
  //
  
  // search node as bridge
  // get bridge's nic
  // prepare bridgeHelper and set parameter
  // call bridge install

  for (const auto& item : m_nodes) {
    const auto& node = item.second;
    if (not findRole(node, {"Switch"})) continue;
    std::cout << "Switch found from '" << node.name  << "', install Bridge!" << std::endl;
    auto nics = getBridgeNics(node);
    lines.push_back( buildBridgeCode(node, nics, m_channels) );
  }

  //
  // install Internet stack
  //
  lines.push_back("// install internet stack");
  lines.push_back("InternetStackHelper stack;");
  for (const auto& item : m_nodes) {
    const auto& node = item.second;
    if (node.type == NODE_T_IFACE) continue; // already installed in subnet build
    lines.push_back("stack.Install("
                      + m_name_all_nodes + ".Get(" + node.name + ")"
                      + ");" );
  }

  //
  // assign IP address
  //
  //std::cout << "***ASSIGN IP***" << std::endl;

  lines.push_back("// ");
  lines.push_back("NS_LOG_INFO (\"Assign ip addresses.\");");
  /* // assign ip addresses
   * NS_LOG_INFO ("Assign ip addresses.");
   * Ipv4AddressHelper ip;
   * ip.SetBase ("192.168.1.0", "255.255.255.0");
   * Ipv4InterfaceContainer addresses = ip.Assign (devs); */
  lines.push_back("{ Ipv4AddressHelper ip;");
  lines.indentRight();
  for (const auto& item : m_channels) {
    Channel ch = item.second;

    std::string nd = m_netdevs_has_ip[ch.name];
    lines.push_back("// " + nd);
    json jconf;
    if (ch.config.empty()) {
			lines.push_back("// config is empty");
      continue;
		}
    jconf = json::parse(ch.config);
    std::string str_addr;
    if (!jconf["Address"].is_string()) {
			lines.push_back("// auto set address");
      str_addr = AddressGenerator::GetLocal()
        + "," + AddressGenerator::GetMask();
      //for (size_t i = 0; i < ch.nodes.size(); i++) {
        AddressGenerator::Next();
      //}
    } else {
      str_addr = jconf["Address"].get<std::string>();
    }
    AddressValue addr(str_addr);
    std::string netaddr = addr.GetNetworkAddress();
    std::string mask = addr.GetMask();
    std::string base_host = addr.GetHost();
    // set base
    lines.push_back( (std::string)"ip.SetBase ("
                      + "\"" + netaddr + "\""
                      + ","
                      + "\"" + mask + "\""
                      + ","
                      + "\"" + base_host + "\""
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
  //std::cout << "***ROUTING***" << std::endl;
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
