/*
The MIT License

Copyright (c) 2017 Hiratake Lab.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Author: hiratake26to@gmail.com
*/

/**
 * \file NetLink.h
 * \brief Network Link class.
 * \author hiratake26to@gmail
 * \date 2017
 */

#pragma once

#include "network-prvt.h"

#include "Network.h"
#include "node/Node.h"
#include "link/Link.h"

/** @brief Network segment model **/
class NetUnit : public Network {
  /** Network segment type **/
  std::string m_type;
  /** Network segment name **/
  std::string m_name;
  /** This net unit id */
  int net_unit_id;
  /** Node(:NetworkSegment) list **/
  int node_tbl_id;
  /** Link table **/
  int link_tbl_id;

  std::vector<int> m_nodes;
  std::vector<Link> m_links;

public:
  /** Constructor **/
  explicit NetUnit(const std::string& type, const std::string& name);

  /** Destructor **/
  virtual ~NetUnit() = default;

public:
  /** Add node to network **/
  void AddNode(const Node& node);

  /** Add a link from node to node **/
  void AddLink(const Node& first, const Node& second);

  void DumpJson();

private:
  void dump(std::string msg, bool nl = false);
};

