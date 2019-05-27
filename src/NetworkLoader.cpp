/*
The MIT License

Copyright (c) 2017 Hiratake Lab.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Author: hiratake26to@gmail.com
*/

/**
 * \file NetworkLoader.cpp
 * \author hiratake26to@gmail
 * \date 2017
 */

#include "loader/NetworkLoader.hpp"

#include <fstream>
#include <iostream>
#include <json.hpp>

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
  //cout << "Load from \"" << filename << "\"" << endl;

  std::string net_name;
  auto j = loadJson(filename);
  // get name
  {
    json jname = j["name"];
    if (!jname.is_string()) throw std::runtime_error("name is none");
    net_name = jname.get<std::string>();
  }
  Network net(Network::NET_T_BASIC, net_name);

  //cout << "***load channel***" << endl;
  { /* load ch */
  json jch = j["channel"];
  for (auto it = jch.begin(); it != jch.end(); ++it) {
    //cout << it.key() << " : " << it.value() << endl;
    net.AddChannel(it.key(), it.value()["type"], it.value()["config"].dump());
  }
  } /* load ch */

  //cout << "***load node***" << endl;
  { /* load node */
  json jnode = j["node"];
  for (auto it = jnode.begin(); it != jnode.end(); ++it) {
    //cout << it.key() << " : " << it.value() << endl;
    // get config
    std::string conf;
    if (!it.value()["config"].empty()) {
      conf = it.value()["config"].dump();
    }
    Vector3D vec;
    try {
      vec = Vector3D { 
        (*it)["point"]["x"],
        (*it)["point"]["y"],
        0
      };
    } catch (std::exception e) {
      std::cerr << "node." << it.key() << ".point is invalid!, must specify .x, .y value." << std::endl;
      throw e;
    }
    // set z value.
    if ( (*it)["point"]["z"].is_number() ) {
      vec.z = (*it)["point"]["z"];
    } else {
      std::cerr << "node." << it.key() << ".point.z is set automatically to 0." << std::endl;
    }

    // [FIXME] type is option, role
    {
      // [TODO] add parse "as"
      std::vector<Netif> ifs;
      std::cout << it.key() << "'s netifs:" << std::endl;
      for (auto json_netif : (*it)["netifs"]) {
        if ( json_netif.contains("as") ) {
          std::cout << "- connect: " << json_netif["connect"] << ", as: " << json_netif["as"] << std::endl;
          ifs.push_back(
              Netif { json_netif["connect"], json_netif["as"] }
              );
        } else {
          std::cout << "- connect: " << json_netif["connect"] << "(, as: \"\")" << std::endl;
          ifs.push_back(
              Netif { json_netif["connect"], "" }
              );
        }
      }
      if ( (*it).contains("type") ) {
        net.AddNode(it.key(), it.value()["type"], ifs, vec, conf);
      } else {
        std::cout << "node." << it.key() << ".type is set automatically to \"Basic\".";
        net.AddNode(it.key(), "Basic", ifs, vec, conf);
      }
    }

    // connect
    for (auto netif : it.value()["netifs"]) {
      net.ConnectChannel(netif["connect"], net.GetNode(it.key()));
    }
  }
  } /* load node */

  //cout << "***load subnet***" << endl;
  { /* load subnet */
  json jsubnet = j["subnet"];
  for (auto it = jsubnet.begin(); it != jsubnet.end(); ++it) {
    // "subnet name" : { "load" : "file", "netifs" : [ { "up" : "name" }, ... ] }
    //cout << it.key() << " : " << it.value() << endl;
    // load
    std::string fname = it.value()["load"].get<std::string>();
    NetworkLoader loader;
    Network snet = loader.load(fname);
    net.AddSubnet(it.key(), snet);
    // netifs up
    for (auto netif : it.value()["netifs"]) {
      Node upped = net.UpIface(it.key(), netif["up"]);
      if (netif["connect"].is_string()) {
        net.ConnectChannel(netif["connect"], upped);
      }
    }
  }
  } /* load subnet */

  //cout << "***load app***" << endl;
  { /* load app */
  json jnode = j["apps"];
  for (auto it = jnode.begin(); it != jnode.end(); ++it) {
    try {
      //std::cout << it.key() << std::endl;
      std::string name = it.key();
      std::string type = it.value()["type"];
      auto jargs = it.value()["args"];
      std::map<std::string, std::string> args;
      for (auto it = jargs.begin(); it != jargs.end(); ++it) {
        //std::cout << "arg load: " << it.key() << ":" << it.value().dump() << std::endl;
        args[it.key()] = it.value().dump();
      }

      net.AddApp(name, type, args);

    } catch (std::exception& e) {
      std::cerr << e.what() << std::endl;
      throw std::runtime_error("Could not load of application, due to missing json format");
    }
  }
#if 0 // debug
  auto apps = net.GetApps();
  for (auto itr : apps) {
    std::cout << "name: " << itr.first;
    std::cout << ", args: " << itr.second.args << std::endl;
  }
#endif
  } /* load app */

  
  //cout << "***LOAD FINISH!***" << endl;
  return net;
}
