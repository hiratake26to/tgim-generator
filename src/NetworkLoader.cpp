#include "NetworkLoader.h"

#include <json.hpp>
#include <fstream>

namespace {
  using json = nlohmann::json;
  using std::cout;
  using std::endl;

  json loadJson(std::string filename) {
    std::ifstream ifs(filename);
    json j;
    ifs >> j;
    ifs.close();
    return j;
  }
}

// impl
Network NetworkLoader::load(std::string filename) {
  cout << "Load from \"" << filename << "\"" << endl;

  std::string net_name;
  auto j = loadJson(filename);
  // get name
  {
    json jname = j["name"];
    if (!jname.is_string()) throw std::runtime_error("name is none");
    net_name = jname.get<std::string>();
  }
  Network net(Network::NET_T_BASIC, net_name);

  cout << "***load channel***" << endl;
  { /* load ch */
  json jch = j["channel"];
  for (auto it = jch.begin(); it != jch.end(); ++it) {
    cout << it.key() << " : " << it.value() << endl;
    net.AddChannel(it.key(), it.value()["type"], it.value()["config"].dump());
  }
  } /* load ch */

  cout << "***load node***" << endl;
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

  cout << "***load subnet***" << endl;
  { /* load subnet */
  json jsubnet = j["subnet"];
  for (auto it = jsubnet.begin(); it != jsubnet.end(); ++it) {
    // "subnet name" : { "load" : "file", "netifs" : [ { "up" : "name" }, ... ] }
    cout << it.key() << " : " << it.value() << endl;
    // load
    std::string fname = it.value()["load"].get<std::string>();
    NetworkLoader loader;
    Network snet = loader.load(fname);
    net.AddSubnet(it.key(), snet);
    // netifs up
    for (auto netif : it.value()["netifs"]) {
      std::string nname = net.UpIface(it.key(), netif["up"]);
      if (netif["connect"].is_string()) {
        net.ConnectChannel(netif["connect"], nname);
      }
    }
  }
  } /* load subnet */
  
  cout << "Finish!" << endl;
  return net;
}
