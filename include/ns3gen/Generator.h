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
#include "CodeSecretary.h"
#include "net/NetUnit.h"
#include "node/Node.h"
#include "channel/Channel.h"
#include "channel/Link.h"
#include <boost/range/adaptor/indexed.hpp>

class BaseGenerator {
public:
};

// main file
class MainGenerator : public BaseGenerator {
};


// network unit file
class NetworkGenerator : public BaseGenerator {
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
  NetworkGenerator(NetUnit net) {
    // nodes
    this->nodes = net.GetNodes();
    // channels
    this->channels = net.GetChannels();
    // name
    this->name = net.GetName();
  }
  /**
   * @brief generate NS3 C++ code
   */
  std::vector<std::string> CppCode() {
    CodeSecretary lines;

    // namespace begin
    lines.push_back("namespace tgim {");
    // struct struct
    lines.push_back("struct " + name + " {");
    lines.indentRight();

    // declare
    gen_decl(lines);

    // build function
    gen_build(lines);

    lines.indentLeft();
    // struct end
    lines.push_back("};");
    // namespace end
    lines.push_back("}");

    return lines.get(); 
  }

private:
  /** variable declare */
  void gen_decl(CodeSecretary& lines) {
    lines.push_back("/*******************************************************");
    lines.push_back(" * section [we can always use]                         *");
    lines.push_back(" ******************************************************/");

    // each node
    lines.push_back("// enum node name");
    lines.push_back("enum NodeName {");
    lines.indentRight();
    for (const auto& item : nodes) {
      const auto& node = item.second;
      lines.push_back("" + node.name + ",");
    }
    lines.indentLeft();
    lines.push_back("};");

    // %%
    lines.push_back("");
    
    // section before build
    lines.push_back("/*******************************************************");
    lines.push_back(" * section [config this when before build if you need] *");
    lines.push_back(" ******************************************************/");

    // link conf
    lines.push_back("// helper");
    std::unordered_map<std::string, std::string> helper_name_map;
    helper_name_map["PointToPoint"] = "PointToPointHelper";
    for (const auto& item : channels) {
      const auto& channel = item.second;
      lines.push_back(helper_name_map[channel->GetType()]
                        + " " + channel->name + ";");
    }

    // %%
    lines.push_back("");
    
    // section after build
    lines.push_back("/*******************************************************");
    lines.push_back(" * section [when after build, we can use this]         *");
    lines.push_back(" ******************************************************/");
    // all nodes
    lines.push_back("// nodes");
    lines.push_back("NodeContainer " + name_all_nodes + ";");
    // netdevices
    for (const auto& item : channels) {
      const auto& channel = item.second;
      netdevs[channel->name] = "ndc_" + channel->name;
      lines.push_back("NetDeviceContainer " + netdevs[channel->name] + ";"
                        + " // P2P link net devices");
    }
  }

  /**
   * @brief build function
   * @details
   * - initialize variables
   * - create node
   * - config channel
   * - create link
   */
  void gen_build(CodeSecretary& lines) {
    // function begin
    lines.push_back("");
    lines.push_back("/*******************************************************");
    lines.push_back(" * build function                                      *");
    lines.push_back(" ******************************************************/");
    lines.push_back("void " + this->name_build_func + "() {");
    lines.indentRight();

    // create all nodes
    lines.push_back("// create all nodes");
    lines.push_back(this->name_all_nodes
                  + ".Create("
                  + std::to_string(this->nodes.size())
                  + ");");

    // name each node
    /*
    lines.push_back("// name each node");
    for (const auto& elem : nodes | boost::adaptors::indexed()) {
      const auto& node = elem.value().second;
      const auto& i = std::to_string(elem.index());
      lines.push_back(node.name + " = "
                    + this->name_all_nodes + ".Get(" + i + ");");
    }
    */

    // connect nodes via link
    lines.push_back("// connect link");
    // -- connector generate
    std::unordered_map<std::string, std::function<void(Channel*)>> connector;
    connector["PointToPoint"]
      = [=, &lines](Channel *channel) { 
        Link link = *dynamic_cast<Link*>(channel);
        lines.push_back( netdevs[link.name] + " = "
                          + link.name + ".Install("
                          + this->name_all_nodes + ".Get(" + link.first + ")"
                          + ","
                          + this->name_all_nodes + ".Get(" + link.second + ")"
                        + ");" );
      };
    // -- connect node to channel
    for (const auto& item : channels) {
      std::shared_ptr<Channel> ch_buf = item.second;
      if ( connector[ch_buf->GetType()] )
      {
        connector[ch_buf->GetType()](ch_buf.get());
      }
      else // auto set channel type, this will been generated latest for loop
      {
        // generate added channnel name
        std::string added_ch_name = "link_auto_"+std::to_string(channels.size());

        ch_buf.reset(new Channel(*ch_buf));  // replace to copy from original
        if ( ch_buf->nodes.size() < 2 )
        {
          lines.push_back("// [TGIM ERR] Channel's node count is less then 2.");
        }
        else if ( ch_buf->nodes.size() == 2 )
        {
          Link *linkptr = new Link();
          linkptr->name = ch_buf->name;
          linkptr->first = ch_buf->nodes[0];
          linkptr->second = ch_buf->nodes[1];
          ch_buf.reset(static_cast<Channel*>(linkptr));
          auto_channels[ch_buf->name] = ch_buf;
        }
        //else if ( ch_buf->nodes.size() > 2 )
        //{
        //}
      }
    }
    for (const auto& item : auto_channels) {
      std::shared_ptr<Channel> ch_buf = item.second;
      if ( connector[ch_buf->GetType()] )
      {
        lines.push_back("// [TGIM AUTO]");
        connector[ch_buf->GetType()](ch_buf.get());
      }
    }

    // assign IP address

    // install Internet stack
    lines.push_back("// install internet stack");
    lines.push_back("InternetStackHelper stack;");
    for (const auto& item : nodes) {
      const auto& node = item.second;
      lines.push_back("stack.Install("
                        + this->name_all_nodes + ".Get(" + node.name + ")"
                        + ");" );
    }

    // routing
    // ...

    /// function end
    lines.indentLeft();
    lines.push_back("}");
  }

};
