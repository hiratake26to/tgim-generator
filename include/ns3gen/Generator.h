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
#include "CodeSecretary.h"
#include "net/NetUnit.h"
#include "node/Node.h"
#include "link/Link.h"
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
  std::map<std::string, Link> links;

public:
  /**
   * @brief constructor
   */
  NetworkGenerator(NetUnit net) {
    // nodes
    this->nodes = net.GetNodes();
    // links
    this->links = net.GetLinks();
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
    lines.push_back("struct " + name + " {");
    lines.indentRight();

    // declare
    gen_decl(lines);

    // build function
    gen_build(lines);

    // namespace end
    lines.indentLeft();
    lines.push_back("}");
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
    lines.push_back("// P2P link");
    for (const auto& item : links) {
      const auto& link = item.second;
      lines.push_back("PointToPointHelper " + link.name + ";");
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
    lines.push_back(this->name_build_func + "() {");
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
    std::string ndc_name = ;
    for (const auto& item : links) {
      const auto& link = item.second;
      lines.push_back( link.name + ".Install("
                        + link.first
                        + ","
                        + link.second
                      + ");" );
    }

    // assign IP address

    // install Internet stack
    lines.push_back("// install internet stack");
    lines.push_back("InternetStackHelper stack;");
    for (const auto& item : nodes) {
      const auto& node = item.second;
      lines.push_back("stack.Install(" + node.name + ");");
    }

    // routing
    // ...

    /// function end
    lines.indentLeft();
    lines.push_back("};");
  }

};
