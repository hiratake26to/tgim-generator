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

void Network::ConnectChannel(std::string ch_name, std::string node_name)
{
  if (m_nodes[node_name].name != node_name) throw std::logic_error("not exist node has name of " + node_name);
  if (m_channels[ch_name].name != ch_name) {
    AddChannel(ch_name, "UNDEFINED", "");
  }
  m_channels[ch_name].nodes.push_back(node_name);
}

std::string Network::UpIface(std::string subnet_name, std::string iface_name)
{
  if (m_subnets[subnet_name].GetType() == NET_T_NONE) {
    throw std::runtime_error(m_name + " is has not subnet '" + subnet_name + "'");
  }
  std::map<std::string, Node> sub_n = m_subnets[subnet_name].GetNodes();
  if (sub_n[iface_name].name != iface_name) {
    throw std::runtime_error("node is not defined in " + subnet_name);
  }
  std::string name_with_prefix = subnet_name+"_"+sub_n[iface_name].name;
  AddNode(name_with_prefix, sub_n[iface_name].config);
  m_nodes[name_with_prefix].type = NODE_T_IFACE;
  m_nodes[name_with_prefix].subnet_name = subnet_name;
  m_nodes[name_with_prefix].subnet_class = m_subnets[subnet_name].GetName();
  m_nodes[name_with_prefix].subnet_node_id = sub_n[iface_name].name;
  return name_with_prefix;
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

std::map<std::string, Network> Network::GetSubnets()
{
  return m_subnets;
}

std::map<std::string, Channel> Network::GetChannels()
{
  return m_channels;
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
    s.update((std::string)"channel."+channel.name, "nodes", channel.nodes ) ;
    s.update((std::string)"channel."+channel.name, "conf", channel.config ) ;
  }

  std::cout << s.toString() << std::endl;

  std::cout << "***" << std::endl;
}

Network::operator std::string() const
{
  return m_name;
}
