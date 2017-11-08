#include "share.h"

#include <boost/algorithm/string.hpp>

void eval_test() {
  //using namespace boost::algorithm;

  Network net;

  NetUnit uni("type", "uni_1");
  // Network Node
    //Node a{0, "node_0"};
    //Node b{1, "node_1"};
  uni.AddNode("node_0");
  uni.AddNode("node_1");
  uni.AddNode("node_2");
  uni.AddNode("node_3");
  uni.AddNode("node_4");
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
