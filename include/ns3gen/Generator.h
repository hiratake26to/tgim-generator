/*
The MIT License

Copyright (c) 2017 Hiratake Lab.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Author: hiratake26to@gmail.com
*/

/**
 * \file Generator.h
 * \brief NS3 Code generate unit
 * \author hiratake26to@gmail
 * \date 2017
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "net/Network.h"
#include "node/Node.h"
#include "channel/Channel.h"
#include "channel/Csma.h"
#include "channel/Link.h"
#include "CodeSecretary.h"

class NetworkGenerator {
  // names
  std::string name;
  std::string name_build_func = "build";
  std::string name_all_nodes = "nodes";
  // node
  std::map<std::string, Node> nodes;
  // channel
  std::map<std::string, std::shared_ptr<Channel>> channels;
  // auto_channel
  std::map<std::string, std::shared_ptr<Channel>> auto_channels;

  // netdevice (channel_name , netdev_name)
  std::map<std::string, std::string> netdevs;

public:
  /**
   * @brief constructor
   */
  NetworkGenerator(Network net);
  /**
   * @brief generate NS3 C++ code
   */
  std::vector<std::string> CppCode();

private:
  /** variable declare */
  void gen_decl(CodeSecretary& lines);

  /**
   * @brief build function
   * @details
   * - initialize variables
   * - create node
   * - config channel
   * - create link
   */
  void gen_build(CodeSecretary& lines);

};
