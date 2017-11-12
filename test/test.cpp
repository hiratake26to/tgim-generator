#include "share.h"

#include <boost/algorithm/string.hpp>

#include <json.hpp>
using json = nlohmann::json;

void eval_test() {

  // create an empty structure (null)
  json j;

  // add a number that is stored as double (note the implicit conversion of j to an object)
  j["pi"] = 3.141;

  // add a Boolean that is stored as bool
  j["happy"] = true;

  // add a string that is stored as std::string
  j["name"] = "Niels";

  // add another null object by passing nullptr
  j["nothing"] = nullptr;

  // add an object inside the object
  j["answer"]["everything"] = 42;

  // add an array that is stored as std::vector (using an initializer list)
  j["list"] = { 1, 0, 2 };

  // add another object (using an initializer list of pairs)
  j["object"] = { {"currency", "USD"}, {"value", 42.99} };

  // instead, you could also write (which looks very similar to the JSON above)
  json j2 = {
    {"pi", 3.141},
    {"happy", true},
    {"name", "Niels"},
    {"nothing", nullptr},
    {"answer", {
      {"everything", 42}
    }},
    {"list", {1, 0, 2}},
    {"object", {
      {"currency", "USD"},
      {"value", 42.99}
    }}
  };
  cout << j2.dump() << endl;

#ifdef TMGR_NETTSET
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
#endif

}
