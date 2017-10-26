#pragma once

#include <string>
#include <vector>
#include "CodeSecretary.h"

class BaseGenerator {
public:
};

// main file
class MainGenerator : public BaseGenerator {
};


// network unit file
class NetworkGenerator : public BaseGenerator {
  std::string name;
  int id;
  struct Node {
    std::string name;
    int id;
  };
  struct Link {
    std::string name;
    int id;
    int first;
    int second;
  };
  std::vector<Node> nodes;
  std::vector<Link> links;

public:
  NetworkGenerator() {
    // this
    this->id = 0;
    // nodes
    nodes.push_back({"",0});
    nodes.push_back({"",1});
    // links
    links.push_back({"",0,0,1});

    // name
    this->name = "NetworkUnit_" + std::to_string(this->id);
    for (auto& node : nodes) {
      node.name = "node_" + std::to_string(node.id);
    }
    for (auto& link : links) {
      link.name = "link_" + std::to_string(link.id);
    }
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
    for (const auto& node : nodes) {
      lines.push_back("Node " + node.name + ";");
    }

    /// create only link
    lines.push_back("// create link");
    for (const auto& link : links) {
      lines.push_back("PointToPointHelper " + link.name+ ";");
    }

    /// init begin
    lines.push_back("");
    lines.push_back("// network unit build");
    lines.push_back(this->name + "() {");
    lines.indentRight();

    // connect nodes via link
    lines.push_back("// connect link");
    for (const auto& link : links) {
      lines.push_back( link.name + ".install("
                        + std::to_string(link.first)
                        + ","
                        + std::to_string(link.second)
                      + ");" );
    }

    // install Internet stack
    lines.push_back("// install internet stack");
    lines.push_back("InternetStackHelper stack;");
    for (const auto& node : nodes) {
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
