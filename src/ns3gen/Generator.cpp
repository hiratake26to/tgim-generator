/*
The MIT License

Copyright 2019 hiratake26to

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/**
 * \file Generator.cpp
 * \author hiratake26to@gmail
 * \date 2017
 */

#include "ns3gen/Generator.hpp"

#include <boost/range/adaptor/indexed.hpp>
#include <boost/algorithm/string.hpp>
#include <algorithm>

// parse json config
#include <json.hpp>
using json = nlohmann::json;

#include <iostream>

namespace {
  using std::string;
  using std::vector;
  using std::map;
  using std::sort;
  using std::multiset;

  Channel FoldChannel(const vector<Channel>& channels) {
    // TODO type decision, config one definision, nodes
    Channel ret;
    ret = channels[0];

    int i = 0;
    for (const auto& c: channels) {
      if (i++ == 0) continue;
      if (ret.type != c.type) throw std::runtime_error("tag channel type is defference");
      if (ret.tag != c.tag) throw std::logic_error("tag channel tag is defference");
      if (ret.config != c.config) throw std::runtime_error("tag channel config is defference");
      ret.nodes.insert(ret.nodes.end(), c.nodes.begin(), c.nodes.end());
    }

    // delete duplicate nodes
    std::sort(ret.nodes.begin(), ret.nodes.end(),
        [](auto a, auto b){ return a < b; });
    auto last = std::unique(ret.nodes.begin(), ret.nodes.end(),
        [](auto a, auto b){ return a == b; });
    ret.nodes.erase(last, ret.nodes.end());
    
    return ret;
  }

  map<string, Channel> ReduceTagChannel(const map<string, Channel>& channels) {
    map<string, Channel> ret;
    map<string, vector<Channel>> bucket; // bucket each tag

    for (const auto& ch : channels) {
      string name;
      if (ch.second.tag != "") name = ch.second.tag;
      else name = ch.second.name;
      bucket[name].push_back(ch.second);
    }

    for (const auto& pair : bucket) {
      ret[pair.first] = FoldChannel(pair.second);
      ret[pair.first].name = pair.first;
    }

    return ret;
  }

  map<string, string> ChToUniqTag(const map<string, Channel>& channels) {
    map<string, string> ret;

    for (const auto& ch : channels) {
      if (ch.second.tag != "") {
        ret[ch.second.name] = ch.second.tag;
        std::cout << "[generator] " << ch.second.name << "->" << ch.second.tag << std::endl;
      }
    }

    return ret;
  }

  // FIXME
  map<string, Node> ReplaceTag(const map<string, Node>& nodes, const map<string, string>& ch_to_tag) {
    map<string, Node> ret;

    for (const auto& pr: nodes) {
      ret[pr.first] = pr.second;

      vector<Netif> temp;
      for (auto nif : ret[pr.first].netifs) {
        auto it = ch_to_tag.find(nif.connect);
        if (it != ch_to_tag.end()) {
          std::cout << "[generator] " << nif.connect << "," << it->second << std::endl;
          nif.connect = it->second;
        }
        temp.push_back(nif);
      }
      ret[pr.first].netifs = temp;
    }

    return ret;
  }
};

NetworkGenerator::NetworkGenerator(Network net, const TemplateLoader& template_loader, const AppModelLoader& appmodel_loader)
{
  // nodes
  //m_nodes = ReplaceTag( net.GetNodes(), ChToUniqTag(net.GetChannels()) );
  //for (auto n : m_nodes) {
  //  std::cout << "[generator] " << n.second.name << std::endl;
  //  for (auto nif : n.second.netifs) {
  //    std::cout << "           " << nif.connect << std::endl;
  //  }
  //}
  m_nodes = net.GetNodes();
  // subnets
  m_subnets = net.GetSubnets();
  // channels
  //m_channels = ReduceTagChannel( net.GetChannels() );
  m_channels = net.GetChannels();
  // app
  m_apps = net.GetApps();
  // name
  m_name = net.GetName();
  // type
  m_net_type = net.GetType();

  // loader
  m_ns3template_loader = template_loader;
  m_ns3appmodel_loader = appmodel_loader;
}

std::vector<std::string> NetworkGenerator::CppCode() {
  // load ns3 code template
  ns3template = m_ns3template_loader.load();

  CodeSecretary lines;

  // namespace begin
  lines.push_back("namespace tgim {");
  // struct struct
  lines.push_back("struct " + m_name + " {");
  lines.indentRight();

  // declare
  gen_decl(lines);

  // build function
  gen_build(lines);

  // application build function
  gen_app(lines);

  lines.indentLeft();
  // struct end
  lines.push_back("};");
  // namespace end
  lines.push_back("}");

  return lines.get(); 
}
