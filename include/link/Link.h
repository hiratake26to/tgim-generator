/*
The MIT License

Copyright (c) 2017 Hiratake Lab.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Author: hiratake26to@gmail.com
*/

/**
 * \file Link.h
 * \brief Network Link Model.
 * \author hiratake26to@gmail
 * \date 2017
 */

#pragma once

#include "network-prvt.h"
#include "LinkAttr.h"

#include "node/Node.h"

struct Link {
  int id;
  std::string name;
  std::string first;
  std::string second;

  std::string config;

  /*
  Link(const Node& nf, const Node& ns) {
    this->first = nf.id;
    this->second = nf.id;
  }
  */
  //int attribute;
};

#if 0
/** @brief Link model **/
class Link {
  LinkAttr attr;
public:
  Link(const Node& first, const Node& second);
  Link(int first_node_id, int second_node_id);
  virtual ~Link() = default;

  std::string toString();

  /*
  void setNodePare(Node *first, Node *second);
  void setFirst(Node *first);
  void setSecond(Node *second);
  Node* getNodeFirst();
  Node* getNodeSecond();

  void setRate(int rate);
  void getRate(int rate);

  void setDelay(int delay);
  void getDelay(int delay);
  */
};
#endif
