#pragma once

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <boost/algorithm/string.hpp>

using namespace std;

namespace Schema {

class SchemaValue {
  enum { UNDEF, STRING, NUMBER } type = UNDEF;

  std::string str;
  int num;

public:
  SchemaValue() = default;
  SchemaValue(const std::string& value) : type(STRING) {
    str = value;
  }
  SchemaValue(int value) : type(NUMBER) {
    num = value;
  }
  ~SchemaValue() {
    switch (type) {
    case UNDEF:
      break;
    case STRING:
      //delete str;
      //str = nullptr;
      break;
    case NUMBER:
      break;
    }
  }
  std::string toString() {
    std::string ret;
    switch (type) {
    case UNDEF:
      ret = "undefined";
      break;
    case STRING:
      ret = (std::string)"\"" + str + "\"";
      break;
    case NUMBER:
      ret = std::to_string(num);
      break;
    }

    return ret;
  }
};

class Schema {
private:
  std::map<std::string, Schema> s_tbl;
  std::map<std::string, SchemaValue> v_tbl;
protected:
  //void defName();
  //void defKey();

  Schema& at(std::vector<std::string> key_hierarchy) {
    const string head = key_hierarchy[0];
    if (key_hierarchy.size() == 1) {
      return s_tbl[head];
    }
    key_hierarchy.erase(key_hierarchy.begin());
    return (s_tbl[head]).at(key_hierarchy);
  }
public:
  Schema() = default;

  Schema& at(std::string key_hierarchy) {
    using namespace boost::algorithm;
    std::vector<std::string> v;
    split(v, key_hierarchy, is_any_of("."));
    return at(v);
  }

  Schema get(std::vector<std::string> key_hierarchy) {
    return at(key_hierarchy);
  }

  /** add schema */
  virtual void update(std::string key_hierarchy, std::string key, std::string value) {
    at(key_hierarchy).update(key, value);
  }

  virtual void update(std::string key_hierarchy, std::string key, int value) {
    at(key_hierarchy).update(key, value);
  }

  /** add schema */
  virtual void update(std::string key, std::string value) {
    v_tbl[key] = SchemaValue(value);
  }

  /** add schema */
  virtual void update(std::string key, int value) {
    v_tbl[key] = SchemaValue(value);
  }

  /** get all keys */
  virtual std::vector<std::string> keys() {
    std::vector<std::string> ret;
    for (const auto& item : s_tbl) {
      ret.push_back(item.first);
    }
    for (const auto& item : v_tbl) {
      ret.push_back(item.first);
    }
    return ret;
  }

  std::string toString() const {
    stringstream ss;
    ss << "{";
    //
    bool delim = false;
    for (const auto& item : s_tbl) {
      if (delim) {
        ss << ",";
        delim = false;
      }
      ss << "\"" << item.first << "\":";
      ss << item.second.toString();
      delim = true;
    }
    //
    for (auto item : v_tbl) {
      if (delim) {
        ss << ",";
        delim = false;
      }
      ss << "\"" << item.first << "\":";
      ss << item.second.toString();
      delim = true;
    }
    //
    ss << "}";
    return ss.str();
  }

private:
  /** */
  std::pair<std::string,std::string> separate(const std::string &s, char delim) {
    pair<string, string> ret;

    //size_t s.find(delim);

    return ret;
  }

};

}
