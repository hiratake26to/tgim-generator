/*
The MIT License

Copyright 2019 hiratake26to

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/**
 * \file Network.hpp
 * \brief Network class.
 * \author hiratake26to@gmail
 * \date 2017
 */

#pragma once

#include "Types.hpp"
#include "network-prvt.hpp"

#include "Node.hpp"
#include "Channel.hpp"
#include "Application.hpp"

/** @brief Network segment model **/
class Network {
public:
  typedef enum {NET_T_BASIC, NET_T_WNET, NET_T_NONE } NetType_t;

private:
  NetType_t m_type;
  std::string m_name; // for the use of struct name

  std::map<std::string, Node> m_nodes;
  std::map<std::string, Channel> m_channels;
  std::map<std::string, Network> m_subnets;
  std::map<std::string, Application> m_apps;

public:
  /** Constructor **/
  explicit Network();
  explicit Network(NetType_t type, const std::string& name);

  /** Destructor **/
  virtual ~Network() = default;

public:
  /** Add node to network **/
  void AddNode(std::string name, std::string config);
  void AddNode(std::string name, std::string type, const std::vector<Netif>& netifs, Vector3D vec, std::string config);
  void AddSubnet(std::string name, const Network& subnet);
  void AddChannel(std::string name, std::string type, std::string config);
  void AddChannel(std::string name, std::string type, std::string tag, std::string config);
  void AddApp( std::string name, std::string type, const std::map<std::string, std::string>& args,
    std::string install, std::vector<std::string> nodes, std::vector<std::string> channels );

  /** Add a link from node to node **/
  void ConnectChannel(std::string ch_name, Node node);

  /** Up subnet iface */
  // return upped iface as Node
  Node UpIface(std::string subnet_name, std::string iface_name);

  void NodeConfig(std::string name, std::string conf);
  void ChannelConfig(std::string name, std::string conf);

  std::string GetName() const;
  int GetType() const;
  std::map<std::string, Node> GetNodes();
  Node GetNode(std::string node_name);
  std::map<std::string, Network> GetSubnets();
  std::map<std::string, Channel> GetChannels();
  std::map<std::string, Application> GetApps();

  /** dump format JSON */
  void DumpJson();
  operator std::string() const;
};

