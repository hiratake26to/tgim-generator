#include "share.h"

#include <boost/algorithm/string.hpp>

#include <json.hpp>
#include <fstream>
using json = nlohmann::json;

class NetworkLoader {
  json loadJson(std::string filename) {
    std::ifstream ifs(filename);
    json j;
    ifs >> j;
    ifs.close();
    return j;
  }
public:
  Network load(std::string filename) {
    cout << "Load from \"" << filename << "\"" << endl;
    Network net("basic", "net_xxxx");

    auto j = loadJson(filename);

    cout << "load channel" << endl;
    { /* load ch */
    json jch = j["channel"];
    for (auto it = jch.begin(); it != jch.end(); ++it) {
      cout << it.key() << " : " << it.value() << endl;
      net.AddChannel(it.key(), it.value()["type"], it.value()["config"].dump());
    }
    } /* load ch */

    cout << "load node" << endl;
    { /* load node */
    json jnode = j["node"];
    for (auto it = jnode.begin(); it != jnode.end(); ++it) {
      cout << it.key() << " : " << it.value() << endl;
      // get config
      std::string conf;
      if (!it.value()["config"].empty()) {
        conf = it.value()["config"].dump();
      }
      net.AddNode(it.key(), conf);
      // connect
      for (auto netif : it.value()["netifs"]) {
        net.ConnectChannel(netif["connect"], it.key());
      }
    }
    } /* load node */
    
    cout << "Finish!" << endl;
    return net;
  }
};

#include "ns3gen/GenUtil.h"

void eval_test() {
#if 1
  NetworkLoader loader;
  Network net = loader.load("./test/config.json");
  net.DumpJson();
  NetworkGenerator gen(net);
  for (auto line : gen.CppCode()) {
    cout << line << endl;
  }
#endif

//#define NLOHMANN_JSON_TEST
#ifdef NLOHMANN_JSON_TEST
  std::ifstream ifs("./test/config.json");
  std::string line;
  while (getline(ifs, line)) {
    std::cout << line << std::endl;
  }
  ifs.clear();
  ifs.seekg(0, ios_base::beg);

  json j;
  
  ifs >> j;

  j.dump(4);

  //cout << j["channel"] << endl;
  json j2 = j["channel"];
  for (auto it = j2.begin(); it != j2.end(); ++it) {
    cout << "--" << endl;
    cout << it.key() << " : " << it.value() << endl;
  }
#endif

//#define TMGR_NETTSET
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

  net.ChannelConfig("link_0", R"(
    {
      "DataRate" : "5Mbps",
      "Delay" : "2ms"
    }
  )");
  net.ChannelConfig("link_1", R"(
    {
      "DataRate" : "5Mbps",
      "Delay" : "2ms"
    }
  )");
  net.ChannelConfig("csma_0", R"(
    {
      "DataRate" : "5Mbps",
      "Delay" : "2ms"
    }
  )");
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

  std::ofstream ofs((std::string)net + ".dump.hpp");
  NetworkGenerator gen(net);
  for (auto line : gen.CppCode()) {
    cout << line << endl;
    ofs << line << endl;
  }
  ofs.close();
#endif

}
