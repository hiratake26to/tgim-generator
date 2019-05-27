/*
The MIT License

Copyright (c) 2017 Hiratake Lab.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Author: hiratake26to@gmail.com
*/

/**
 * \file Generator.hpp
 * \brief NS3 Code generate unit
 * \author hiratake26to@gmail
 * \date 2017
 */

#pragma once

#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "CodeSecretary.hpp"
#include "net/Application.hpp"
#include "net/Channel.hpp"
#include "net/Network.hpp"
#include "net/Node.hpp"
#include "loader/TemplateLoader.hpp"
#include "loader/AppModelLoader.hpp"

#include <json.hpp>
using json = nlohmann::json;

class NetworkGenerator {
  // names
  std::string m_name;
  std::string m_name_build_func = "build";
  std::string m_name_app_func = "app";
  std::string m_name_all_nodes = "nodes";
  // type
  int m_net_type;
  // node
  std::map<std::string, Node> m_nodes;
  // subnet
  std::map<std::string, Network> m_subnets;
  // channel
  std::map<std::string, Channel> m_channels;
  // channel
  std::map<std::string, Application> m_apps;
  // auto_channel
  std::map<std::string, Channel> m_auto_channels;

  // netdevice (channel_name , netdev_name)
  std::map<std::string, std::string> m_netdevs;
  // netdevice, has ip (channel_name , netdev_name)
  std::map<std::string, std::string> m_netdevs_has_ip;

  // ns3 code template
  TemplateLoader m_ns3template_loader;
  json ns3template;
  AppModelLoader m_ns3appmodel_loader;

public:
  /**
   * @brief constructor
   */
  NetworkGenerator(Network net, const TemplateLoader& template_loader, const AppModelLoader& loader);
  /**
   * @brief generate NS3 C++ code
   */
  std::vector<std::string> CppCode();

private:
  /** variable declare */
  void gen_decl(CodeSecretary& lines);

  /**
   * @brief Generate build function
   * @details
   * - initialize variables
   * - create node
   * - config channel
   * - create link
   */
  void gen_build(CodeSecretary& lines);

  /**
   * @brief Generate application
   */
  void gen_app(CodeSecretary& lines);

};
