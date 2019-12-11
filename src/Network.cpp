/*
The MIT License

Copyright (c) 2017 Hiratake Lab.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Author: hiratake26to@gmail.com
*/

/**
 * \file Network.cpp
 * \author hiratake26to@gmail
 * \date 2017
 */

#include "net/Network.hpp"
#include "net/Channel.hpp"
#include "net/Node.hpp"

#include <iostream>
#include "schema/Schema.hpp"

Network::Network()
  : m_type(NET_T_NONE), m_name("")
{ }

Network::Network(NetType_t type, const std::string& name)
  : m_type(type), m_name(name)
{ }


// public:

/** Add a node which is the network segment **/
void Network::AddNode(std::string name, std::string config)
{
  Node added { (int)m_nodes.size(), name, config };
  m_nodes[name] = added;
}

void Network::AddNode(std::string name, std::string type, const std::vector<Netif>& netifs, Vector3D vec, std::string config)
{
  Node added { (int)m_nodes.size(), name, config };
  if (type == "Adhoc") {
    added.type = NODE_T_ADHOC;
  }
  else if (type == "Ap") {
    added.type = NODE_T_AP;
  }
  else if (type == "Sta") {
    added.type = NODE_T_STA;
  }
  else if (type == "Basic" ){
    added.type = NODE_T_BASIC;
  }
  else {
    std::cout << "network '" << m_name << "' node '" << name << "' is no specified type, it type decides to 'Basic'." << std::endl;
    added.type = NODE_T_BASIC;
  }
  added.netifs = netifs;
  added.point.x = vec.x;
  added.point.y = vec.y;
  added.point.z = vec.z;
  m_nodes[name] = added;
}

void Network::AddSubnet(std::string name, const Network& subnet)
{
  if ( m_type != NET_T_WNET ) m_type = NET_T_WNET; // change type to NET_T_WNET
  m_subnets[name] = subnet;
}

void Network::AddChannel(std::string name, std::string type, std::string config)
{
  Channel added { name, type, config };
  m_channels[name] = added;
}

void Network::AddApp(
    std::string name, std::string type, const std::map<std::string, std::string>& args,
    std::string install, std::vector<std::string> nodes, std::vector<std::string> channels )
{
  Application added { name, type, args, install, nodes, channels };
  m_apps[name] = added;
}

void Network::ConnectChannel(std::string ch_name, Node node)
{
  if (m_nodes[node.name].name != node.name) throw std::logic_error("not exist node has name of " + node.name);
  if (m_channels[ch_name].name != ch_name) {
    AddChannel(ch_name, "UNDEFINED", "");
  }
  m_channels[ch_name].nodes.push_back(node);
}

Node Network::UpIface(std::string subnet_name, std::string iface_name)
{
  if (m_subnets[subnet_name].GetType() == NET_T_NONE) {
    throw std::runtime_error(m_name + " is has not subnet '" + subnet_name + "'");
  }
  std::map<std::string, Node> sub_n = m_subnets[subnet_name].GetNodes();
  if (sub_n[iface_name].name != iface_name) {
    throw std::runtime_error("node is not defined in " + subnet_name);
  }
  std::string name_with_prefix = subnet_name+"_"+sub_n[iface_name].name;
  // [TODO] Integrate node role ("as")
  AddNode(name_with_prefix, sub_n[iface_name].config);
  m_nodes[name_with_prefix].type = NODE_T_IFACE;
  m_nodes[name_with_prefix].subnet_name = subnet_name;
  m_nodes[name_with_prefix].subnet_class = m_subnets[subnet_name].GetName();
  m_nodes[name_with_prefix].subnet_node_id = sub_n[iface_name].name;
  return m_nodes[name_with_prefix];
}

void Network::NodeConfig(std::string name, std::string conf)
{
  if (m_nodes[name].name != name) throw std::logic_error("node has not name");
  m_nodes[name].config = conf;
}

std::string Network::GetName() const
{
  return m_name;
}

int Network::GetType() const
{
  return m_type;
}

std::map<std::string, Node> Network::GetNodes()
{
  return m_nodes;
}
Node Network::GetNode(std::string node_name)
{
  return m_nodes[node_name];
}

std::map<std::string, Network> Network::GetSubnets()
{
  return m_subnets;
}

std::map<std::string, Channel> Network::GetChannels()
{
  return m_channels;
}

std::map<std::string, Application> Network::GetApps()
{
  return m_apps;
}

void Network::DumpJson()
{
  std::cout << "Schema test ***" << std::endl;

  Schema::Schema s; // network schema

  s.update("name", m_name);

  for (const auto& item : m_nodes ) {
    s.update((std::string)"node."+item.first, "id", item.second.id ) ;
    s.update((std::string)"node."+item.first, "conf", item.second.config ) ;
  }
  for (const auto& item : m_channels) {
    const auto& channel = item.second;
    std::vector<std::string> ch_nodes;
    for (int i = 0; i < channel.nodes.size(); ++i) ch_nodes.push_back(channel.nodes[i].name);
    s.update((std::string)"channel."+channel.name, "nodes", ch_nodes ) ;
    s.update((std::string)"channel."+channel.name, "conf", channel.config ) ;
  }

  std::cout << s.toString() << std::endl;

  std::cout << "***" << std::endl;
}

Network::operator std::string() const
{
  return m_name;
}
