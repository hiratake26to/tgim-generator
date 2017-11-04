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
  //using namespace boost::algorithm;

  Network net;

  NetUnit uni("type", "uni_1");
  // Network Node
    Node a{0, "node_0"};
    Node b{1, "node_1"};
  uni.AddNode("node_0");
  uni.AddNode("node_1");
  // Wire Link
  uni.ConnectChannel("link_0", "node_0");
  uni.ConnectChannel("link_0", "node_1");

  uni.ConnectChannel("csma_0", "node_1");
  uni.ConnectChannel("csma_0", "node_2");
  uni.ConnectChannel("csma_0", "node_3");

  uni.ConnectChannel("link_1", "node_4");
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

  // ---

  NetworkGenerator gen(uni);
  for (auto line : gen.CppCode()) {
    cout << line << endl;
  }

}
