#include "net/Network.h"
#include "channel/Link.h"
#include "node/Node.h"

#include <iostream>
#include "schema/Schema.hpp"

Network::Network(const std::string& type, const std::string& name)
  : m_type(type), m_name(name)
{
}


// public:

/** Add a node which is the network segment **/
void Network::AddNode(std::string name)
{
  Node added { (int)m_nodes.size(), name };
  m_nodes[name] = added;
}

void Network::ConnectChannel(std::string name, std::string node_name)
{
  if ( !m_channels[name] ) {
    m_channels[name] = std::shared_ptr<Channel>(new Channel( name ));
  }
  m_channels[name]->nodes.push_back(node_name);
}

void Network::NodeConf(std::string name, std::string conf)
{
  m_nodes[name].config = conf;
}

/*
void Network::LinkConf(std::string name, std::string conf)
{
  m_channels[name]->config = conf;
}
*/

std::string Network::GetName()
{
  return m_name;
}

std::map<std::string, Node> Network::GetNodes()
{
  return m_nodes;
}

std::map<std::string, std::shared_ptr<Channel>> Network::GetChannels()
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
    if (channel->GetType() == "PointToPoint") {
      Link *link = dynamic_cast<Link*>(channel.get());
      s.update((std::string)"link."+link->name, "first", link->first ) ;
      s.update((std::string)"link."+link->name, "second", link->second ) ;
      s.update((std::string)"link."+link->name, "conf", link->config ) ;
    } else {
      std::cout << "faild get channel!!" << std::endl;
    }

  }

  cout << s.toString() << endl;

  cout << "***" << endl;
}
