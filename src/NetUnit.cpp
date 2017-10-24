#include "net/NetUnit.h"
#include "node/Node.h"
#include "link/Link.h"

#include <iostream>

NetUnit::NetUnit(const std::string& type, const std::string& name)
  : m_type(type), m_name(name)
{
}


// public:

/** Add a node which is the network segment **/
void NetUnit::AddNode(const Node& node)
{
  m_nodes.push_back(node.GetId());
}

/** Add a link from node to node **/
void NetUnit::AddLink(const Node& first, const Node& second)
{
  m_links.push_back( Link(first.GetId(), second.GetId()) );
}

void NetUnit::dump(std::string msg, bool nl) {
  static int i = 0;

  if (msg == "}" || msg == "]") {
    std::cout << std::endl;
    i--;
    for (auto j = 0; j < i; j++) std::cout << "  ";
  }
  if (msg == "{" || msg == "[") {
    i++;
  }
  std::cout << msg << std::flush;
  if (msg == "}" || msg == "]") {
    std::cout << std::endl;
    for (auto j = 0; j < i; j++) std::cout << "  ";
  }
  if (msg == "{" || msg == "[") {
    std::cout << std::endl;
    for (auto j = 0; j < i; j++) std::cout << "  ";
  }
  if (nl) {
    std::cout << std::endl;
    for (auto j = 0; j < i; j++) std::cout << "  ";
  }
}

void NetUnit::DumpJson()
{
  dump("\"NetUnit\":");
  dump("{");
  dump("\"Name\":");
  dump("\"");
  dump(m_name);
  dump("\"");
  dump(",", true);
  // nodes
  dump("\"node\":");
  dump("[");
  for (auto itr = m_nodes.begin(), end = m_nodes.end() ; itr != end; itr++) {
    if (itr != m_nodes.begin()) dump(",");
    dump(std::to_string(*itr));
  }
  dump("]");
  dump(",", true);
  dump("\"link\":");
  dump("[");
  for (auto itr = m_links.begin(), end = m_links.end() ; itr != end; itr++) {
    if (itr != m_links.begin()) dump(",");
    dump("\"");
    dump(itr->toString());
    dump("\"");
  }
  dump("]");

  dump("}");
}
