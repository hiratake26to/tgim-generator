#pragma once

#include <string>
#include <vector>
#include "CodeSecretary.h"
#include "net/NetUnit.h"
#include "node/Node.h"
#include "link/Link.h"

class BaseGenerator {
public:
};

// main file
class MainGenerator : public BaseGenerator {
};


// network unit file
class NetworkGenerator : public BaseGenerator {
  std::string name;
  std::map<std::string, Node> nodes;
  std::map<std::string, Link> links;

public:
  NetworkGenerator(NetUnit net) {
    // nodes
    nodes = net.GetNodes();
    // links
    links = net.GetLinks();

    // name
    this->name = net.GetName();
  }
  std::vector<std::string> CppCode() {
    CodeSecretary lines;

    // namespace begin
    lines.push_back("namespace tgim {");
    lines.push_back("struct " + name + " {");
    lines.indentRight();

    /// variables
    // create nodes
    lines.push_back("// create nodes");
    for (const auto& item : nodes) {
      const auto& node = item.second;
      lines.push_back("Node " + node.name + ";");
    }

    /// create only link
    lines.push_back("// create link");
    for (const auto& item : links) {
      const auto& link = item.second;
      lines.push_back("PointToPointHelper " + link.name + ";");
    }

    /// init begin
    lines.push_back("");
    lines.push_back("// network unit build");
    lines.push_back(this->name + "() {");
    lines.indentRight();

    // connect nodes via link
    lines.push_back("// connect link");
    for (const auto& item : links) {
      const auto& link = item.second;
      lines.push_back( link.name + ".install("
                        + link.first
                        + ","
                        + link.second
                      + ");" );
    }

    // install Internet stack
    lines.push_back("// install internet stack");
    lines.push_back("InternetStackHelper stack;");
    for (const auto& item : nodes) {
      const auto& node = item.second;
      lines.push_back("stack.Install(" + node.name + ");");
    }

    // routing
    // ...

    /// init end
    lines.indentLeft();
    lines.push_back("}");

    // namespace end
    lines.indentLeft();
    lines.push_back("}");
    lines.push_back("}");

    return lines.get(); 
  }

};
