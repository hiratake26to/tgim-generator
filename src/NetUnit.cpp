#include "net/NetUnit.h"
#include "channel/Link.h"
#include "node/Node.h"

#include <iostream>
#include "schema/Schema.hpp"

NetUnit::NetUnit(const std::string& type, const std::string& name)
  : m_type(type), m_name(name)
{
}


// public:

/** Add a node which is the network segment **/
void NetUnit::AddNode(std::string name)
{
  Node added { (int)m_nodes.size(), name };
  m_nodes[name] = added;
}

/** Add a link from node to node **/
void NetUnit::AddLink(std::string name, std::string first, std::string second)
{
  m_channels[name] = std::shared_ptr<Channel>(new Link ( (int)m_channels.size(), name, first, second ));
}

void NetUnit::NodeConf(std::string name, std::string conf)
{
  m_nodes[name].config = conf;
}

void NetUnit::LinkConf(std::string name, std::string conf)
{
  m_channels[name]->config = conf;
}

std::string NetUnit::GetName()
{
  return m_name;
}

std::map<std::string, Node> NetUnit::GetNodes()
{
  return m_nodes;
}

std::map<std::string, std::shared_ptr<Channel>> NetUnit::GetChannels()
{
  return m_channels;
}

void NetUnit::DumpJson()
{
  cout << "Schema test ***" << endl;

  Schema::Schema s; // net unit schema

  s.update("name", m_name);

  for (const auto& item : m_nodes ) {
    s.update((std::string)"node."+item.first, "id", item.second.id ) ;
    s.update((std::string)"node."+item.first, "conf", item.second.config ) ;
  }
  for (const auto& item : m_channels) {
    const auto& channel = item.second;
    if (channel->GetType() == "PointToPoint") {
      Link *link = dynamic_cast<Link*>(channel.get());
      s.update((std::string)"link."+link->name, "id", link->id ) ;
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
