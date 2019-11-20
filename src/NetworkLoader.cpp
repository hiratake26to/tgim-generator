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
#include <algorithm>

using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;
using std::accumulate;

#define RANGE(v) v.begin(), v.end()

namespace {
  using json = nlohmann::json;

  json loadJson(string filename) {
    std::ifstream ifs(filename);
    if (ifs.is_open()) {
      json j;
      ifs >> j;
      ifs.close();
      return j;
    }

    string ss;
    ss += "could not open file: ";
    ss += filename;
    throw std::runtime_error(ss);
  }
}

// impl
Network NetworkLoader::load(string filename) {
  //cout << "Load from \"" << filename << "\"" << endl;

  string net_name;
  auto j = loadJson(filename);
  // get name
  {
    json jname = j["name"];
    if (not jname.is_string()) throw std::runtime_error("no network has a name");
    net_name = jname.get<string>();
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
    const auto& key = it.key();
    auto&& val = *it;
    //cout << it.key() << " : " << it.value() << endl;
    // get config
    string conf;
    if (val["config"].empty()) {
      conf = val["config"].dump();
    }
    Vector3D vec;
    try {
      vec = Vector3D { 
        val["point"]["x"],
        val["point"]["y"],
        0
      };
    } catch (const std::exception& e) {
      cerr << "node." << key << ".point is invalid!, must specify .x, .y value." << endl;
      throw e;
    }
    // set z value.
    if ( val["point"]["z"].is_number() ) {
      vec.z = val["point"]["z"];
    } else {
      cerr << "node." << key << ".point.z set default value of 0." << endl;
    }

    // parse `node.as` as a default role for channel.
    vector<string> default_roles = {};
    if (val.contains("as")) {
      if (val["as"].is_string()) {
        default_roles.push_back(val["as"]);
      } else if (val["as"].is_array()) {
        for (const auto& rl : val["as"]) if (rl.is_string()) {
          default_roles.push_back(rl);
        }
      } else {
        throw std::runtime_error(string()
            + "Exception: ignore `node." + key + ".as` "
            + "due to specified type, it must be string.");
      }

      cerr << "Info: node." << key << " "
        << ", the default role is "
        << accumulate(RANGE(default_roles), string{},
            [](string a,string b){if(a=="")return b;return a+", "+b;})
        << "." << endl;
    }

    {
      // parse `as` that type is [ String | Array ]
      vector<Netif> ifs;
      cout << key << "'s netifs:" << endl;
      for (auto json_netif : val["netifs"]) {
        // make roles
        vector<string> roles = {default_roles};
        if ( json_netif.contains("as") ) { // parse `as`
          if (json_netif["as"].is_string()) { // for type of String
            roles.push_back(json_netif["as"]);
          } else if (json_netif["as"].is_array()) { // for type of Array
            for (const auto& role : json_netif["as"]) {
              if (!role.is_string()) {
                throw std::runtime_error("role is invalid, '" + role.dump() + "' is not string.");
              }
              roles.push_back(role);
            }
            ifs.push_back( Netif{ json_netif["connect"], roles });
          } else { // error
            throw std::runtime_error(
                "Could not load of node['" + key +
                "'].as, due to missing json format. type must be type [ String | Array ]");
          }
        }

        // append the netif to the node.
        cout << "- connect: " << json_netif["connect"]
          << ", as: "
          << accumulate(RANGE(roles), string{},
              [](string a,string b){if(a=="")return b;return a+", "+b;})
          << endl;
        ifs.push_back( Netif{ json_netif["connect"], roles });

      }
      if ( val.contains("type") ) {
        net.AddNode(key, val["type"], ifs, vec, conf);
      } else {
        cout << "node." << key << ".type set default value of \"Basic\"." << endl;
        net.AddNode(key, "Basic", ifs, vec, conf);
      }
    }

    // connect
    for (auto netif : val["netifs"]) {
      net.ConnectChannel(netif["connect"], net.GetNode(key));
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
    string fname = it.value()["load"].get<string>();
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
      //cout << it.key() << endl;
      string name = it.key();
      string type = it.value()["type"];
      auto jargs = it.value()["args"];
      std::map<string, string> args;
      for (auto it = jargs.begin(); it != jargs.end(); ++it) {
        //cout << "arg load: " << it.key() << ":" << it.value().dump() << endl;
        args[it.key()] = it.value().dump();
      }

      net.AddApp(name, type, args);

    } catch (const std::exception& e) {
      cerr << e.what() << endl;
      throw std::runtime_error("Could not load of application, due to missing json format");
    }
  }
#if 0 // debug
  auto apps = net.GetApps();
  for (auto itr : apps) {
    cout << "name: " << itr.first;
    cout << ", args: " << itr.second.args << endl;
  }
#endif
  } /* load app */

  
  //cout << "***LOAD FINISH!***" << endl;
  return net;
}
