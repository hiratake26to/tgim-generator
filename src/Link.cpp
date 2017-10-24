#include "link/Link.h"

/*
class Link {
  LinkAttr attr;
public:
  Link(const Node& first, const Node& second);
  Link(int first_node_id, int second_node_id);
  virtual ~Link();

  void setNodePare(Node *first, Node *second);
  void setFirst(Node *first);
  void setSecond(Node *second);
  Node* getNodeFirst();
  Node* getNodeSecond();

  void setRate(int rate);
  void getRate(int rate);

  void setDelay(int delay);
  void getDelay(int delay);
};
*/
Link::Link(const Node& first, const Node& second)
{
  attr.setNodePare(first.GetId(), second.GetId());
}

Link::Link(int first_node_id, int second_node_id)
{
  attr.setNodePare(first_node_id, second_node_id);
}

std::string Link::toString()
{
  std::string ret = "<Link first='";
  ret += std::to_string(attr.getFirst());
  ret += "' second='";
  ret += std::to_string(attr.getSecond());
  ret += "' />";
  return ret;
}

// 
#include "link/LinkAttr.h"
//LinkAttr::

void LinkAttr::setNodePare(int first, int second)
{
  setFirst(first);
  setSecond(second);
}

void LinkAttr::setFirst(int first)
{
  this->first = first;
}

void LinkAttr::setSecond(int second)
{
  this->second = second;
}

int LinkAttr::getFirst()
{
  return this->first;
}

int LinkAttr::getSecond()
{
  return this->second;
}
