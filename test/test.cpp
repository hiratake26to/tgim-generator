#include "share.h"

#include <boost/algorithm/string.hpp>

void eval_test() {
  //using namespace boost::algorithm;

  Network net("type", "net_1");
  // Network Node
    //Node a{0, "node_0"};
    //Node b{1, "node_1"};
  net.AddNode("node_0");
  net.AddNode("node_1");
  net.AddNode("node_2");
  net.AddNode("node_3");
  net.AddNode("node_4");
  // Wire Link
  net.ConnectChannel("link_0", "node_0");
  net.ConnectChannel("link_0", "node_1");

  net.ConnectChannel("csma_0", "node_1");
  net.ConnectChannel("csma_0", "node_2");
  net.ConnectChannel("csma_0", "node_3");

  net.ConnectChannel("link_1", "node_4");

  //net.at("link_0").config = "{\"delay\":\"100ms\", \"rate\":\"64kbps\"}";
  // Wireless Channel 
  //net.Channel("ch_0").create;
  //net.Channel("ch_0").append("node_0");
  //net.Channel("ch_0").append("node_1");
  //net.at("ch0").config = "{\"type\":\"wifi\", \"sta\":\"node_0\", \"ap\":[\"node_1\"]}";
  // NetUnit interface
  //net.Iface("iface_1").create;
  //net.Iface("iface_1").attatch("node_0");

  net.DumpJson();

  // ---

  NetworkGenerator gen(net);
  for (auto line : gen.CppCode()) {
    cout << line << endl;
  }

}
