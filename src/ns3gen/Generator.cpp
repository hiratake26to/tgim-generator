/*
The MIT License

Copyright (c) 2017 Hiratake Lab.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Author: hiratake26to@gmail.com
*/

/**
 * \file Generator.cpp
 * \author hiratake26to@gmail
 * \date 2017
 */

#include "ns3gen/Generator.hpp"
#include "ns3gen/GenUtil.hpp"

#include <boost/range/adaptor/indexed.hpp>
#include <boost/algorithm/string.hpp>

// parse json config
#include <json.hpp>
using json = nlohmann::json;

NetworkGenerator::NetworkGenerator(Network net, const TemplateLoader& template_loader, const AppModelLoader& appmodel_loader)
{
  // nodes
  m_nodes = net.GetNodes();
  // subnets
  m_subnets = net.GetSubnets();
  // channels
  m_channels = net.GetChannels();
  // apps
  m_apps = net.GetApps();
  // name
  m_name = net.GetName();
  // type
  m_net_type = net.GetType();

  // loader
  m_ns3template_loader = template_loader;
  m_ns3appmodel_loader = appmodel_loader;

  // other init
  AddressGenerator::Init();
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
