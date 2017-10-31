#include <iostream>
#include <string>

#include "network.h"
#include "TmgrRepl.h"
#include "ns3gen/Generator.h"
#include "schema/Schema.hpp"
#include <boost/algorithm/string/join.hpp>

using std::cout;
using std::cin;
using std::endl;
using std::string;


void eval_test();

int main(int argc, char *argv[]) {

  //
  // initialization
  //

  if (argc > 1) {
    if (string(argv[1]) == "--debug") {
      eval_test();
      return 0;
    }
  }

  Repl repl;
  //
  // main
  //
  
  std::string buf;
  while (true) {
    cout << ">";
    cin >> buf;
    if (cin.eof()) return 0;
    switch (repl.eval(buf))
    {
    case Repl::Res::QUIT:
      return 0;
      break;
    case Repl::Res::TEST:
      eval_test();
      break;
    case Repl::Res::SUCCESS:
      break;
    default:
      cout << "what!?" << endl;
      break;
    }
    buf.clear();
  }

  //
  // test
  //
  
  return 0;
}

#include <boost/algorithm/string.hpp>

void eval_test() {
  using namespace boost::algorithm;

#if 0
  cout << "Schema test ***" << endl;

  Schema::Schema s; // net unit schema

  // this schema
  // attr_name, attr_value
  s.update("name", "unit_0");
  // child_schema, attr_name, attr_value
  s.update("node.node_0", "delay", "100ms");
  s.update("node.node_0", "rate", "64kbps");
  s.update("link.link_0", "first", "node.node_0");
  s.update("link.link_0", "second", "node.node_1");

  cout << s.toString() << endl;

  cout << "***" << endl;

  // ---

  NetworkGenerator gen;
  for (auto line : gen.CppCode()) {
    cout << line << endl;
  }
#endif

  Network net;
  NetUnit uni("type", "uni_1");
  // Network Node
    Node a{0, "node_0"};
    Node b{1, "node_1"};
  uni.AddNode("node_0");
  uni.AddNode("node_1");
  // Wire Link
  uni.AddLink("link_0", "node_0", "node_1");
  //uni.at("link_0").config = "{\"delay\":\"100ms\", \"rate\":\"64kbps\"}";
  // Wireless Channel 
  //uni.Channel("ch_0").create;
  //uni.Channel("ch_0").append("node_0");
  //uni.Channel("ch_0").append("node_1");
  //uni.at("ch0").config = "{\"type\":\"wifi\", \"sta\":\"node_0\", \"ap\":[\"node_1\"]}";
  // NetUnit interface
  //uni.Iface("iface_1").create;
  //uni.Iface("iface_1").attatch("node_0");

  uni.DumpJson();
}
