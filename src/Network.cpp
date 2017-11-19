#include "net/Network.h"
#include "channel/Channel.h"
#include "node/Node.h"

#include <iostream>
#include "schema/Schema.hpp"

Network::Network(const std::string& type, const std::string& name)
  : m_type(type), m_name(name)
{
}


// public:

/** Add a node which is the network segment **/
void Network::AddNode(std::string name, std::string config)
{
  Node added { (int)m_nodes.size(), name, config };
  m_nodes[name] = added;
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

void Network::NodeConfig(std::string name, std::string conf)
{
  if (m_nodes[name].name != name) throw std::logic_error("node has not name");
  m_nodes[name].config = conf;
}

std::string Network::GetName()
{
  return m_name;
}

std::map<std::string, Node> Network::GetNodes()
{
  return m_nodes;
}

std::map<std::string, Channel> Network::GetChannels()
{
  return m_channels;
}

void Network::DumpJson()
{
  cout << "Schema test ***" << endl;

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

  cout << s.toString() << endl;

  cout << "***" << endl;
}

Network::operator std::string() const
{
  return m_name;
}
