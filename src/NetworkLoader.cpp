/*
The MIT License

Copyright 2019 hiratake26to

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
#include <set>

using std::string;
using std::vector;
using std::map;
using std::cout;
using std::cerr;
using std::endl;
using std::accumulate;

#define RANGE(v) v.begin(), v.end()

namespace {
  using json = nlohmann::json;

  // TODO preprocess for channel tag
  void pre_channel_tag(json& j) {
    std::cout << "[generator] preprocess ..." << std::endl;
    // tag -> [channel]
    std::map<std::string, std::vector<std::string>> dict_tag_ch;
    // channel -> body // tag only
    std::map<std::string, json> dict_ch_body;
    for (auto& [key, value] : j["channel"].items()) {

      if (value["tag"].is_string()) {
        std::cout << "tag!!" << value["tag"] << std::endl;
        dict_ch_body[key] = value;
        dict_tag_ch[value["tag"]].push_back(key);
      } else if (value["tag"].is_null()){
        // nothing
      } else {
        throw std::runtime_error("channel tag is invalid, tag must be string type");
      }
    }

    // check tag channels
    for (const auto& item : dict_tag_ch) {
      auto tag       = item.first;
      auto&& ch_list = item.second;

      bool begin = true;
      json last;
      bool ok = true;
      for (auto&& ch : ch_list) {
        if (begin) {
          last = dict_ch_body[ch];
          begin = false;
        } else {
          ok = (last == dict_ch_body[ch]);
          last = dict_ch_body[ch];
          if (not ok) break;
        }
      }

      if (not ok) {
        std::string msg = std::string("invalid channel that tagged: ") + last.dump();
        throw std::runtime_error(msg);
      }
    }

    // append channel
    for (auto&& item : dict_tag_ch) {
      auto key = item.first;
      auto&& value = item.second;
      j["channel"][key] = dict_ch_body[value[0]];
      j["channel"][key].erase("tag");
    }
    
    // delete channel has tag
    for (auto&& item : dict_ch_body) {
      auto key = item.first;
      //auto&& value = item.second;
      j["channel"].erase(key);
    }
    
    // replace to tag
    for (auto& [node_key, node_value] : j["node"].items()) {
      if (node_value["netifs"].is_array())
      for (auto& [netif_idx, netif_value] : node_value["netifs"].items()) {
        //std::cout << netif_idx << "->" << netif_value.dump() << std::endl;
        if (netif_value["connect"].is_string()
            and not dict_ch_body[netif_value["connect"]]["tag"].empty()) {
          // replace to tag
          netif_value["connect"] = dict_ch_body[netif_value["connect"]]["tag"];
        }
      }
    }

    // merge and delete node.netifs .connect 
    for (auto& [node_key, node_value] : j["node"].items()) {
      cout << "[preprocess] node: " << node_key << endl;
      // map, connect -> [netif]
      map<string, vector<json>> m;

      if (node_value["netifs"].is_array())
      for (auto& [netif_idx, netif_value] : node_value["netifs"].items()) {
        cout << "[preprocess] netifs[" << netif_idx << "]: " << netif_value.dump() << endl;
        if (netif_value["connect"].is_string()) {
          m[netif_value["connect"]].push_back(netif_value);
        }
      }

      // netifs that connect same channel have the same body
      for (auto& it : m) {
        auto& ifs = it.second;

        auto to_vec = [](json a) -> vector<string> {
          if (a.is_string()) return {a};
          else if (a.is_array()) return a;
          std::runtime_error("netifs[].as must be string or [string]");
          return {};
        };

        auto role_fold = [](vector<string> a, vector<string> b) 
          -> vector<string> {
          std::set<string> ret;
          ret.insert(a.begin(), a.end());
          ret.insert(b.begin(), b.end());
          return {ret.begin(), ret.end()};
        };

        auto netif_fold = [to_vec, role_fold](json a, json b){
          if (a["connect"] != b["connect"]) 
            throw std::logic_error("invalid connect! connect must be same channel");

          auto ret = role_fold(to_vec(a["as"]), to_vec(b["as"]));
          a["as"] = ret;
          return a;
        };

        auto acc = std::accumulate(std::next(ifs.begin()), ifs.end(),
            ifs[0], netif_fold);
        ifs[0] = acc;
      }
      

      // delete netifs
      node_value["netifs"] = {};

      // set netifs
      // reduce vecor<json> -> json
      for (const auto& it : m) {
        cout << "- connect: " << it.second[0] << endl;
        node_value["netifs"] += it.second[0];
      }
        
    }

    cout << "[generator] preprocess result:" << endl;
    cout << j.dump(2) << endl;
    
  }

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
  pre_channel_tag(j);
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
    if (it.value()["tag"].is_string()) {
      net.AddChannel(it.key(), it.value()["type"], it.value()["tag"], it.value()["config"].dump());
    } else if (it.value()["tag"].is_null()){
      net.AddChannel(it.key(), it.value()["type"], it.value()["config"].dump());
    } else {
      throw std::runtime_error("channel tag is invalid, tag must be string type");
    }
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
      string install = it.value()["install"];
      vector<string> nodes = it.value()["nodes"];
      vector<string> channels = it.value()["channels"];

      net.AddApp(name, type, args, install, nodes, channels);

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
