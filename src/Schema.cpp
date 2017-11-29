/*
The MIT License

Copyright (c) 2017 Hiratake Lab.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Author: hiratake26to@gmail.com
*/

/**
 * \file Schema.cpp
 * \brief Network attribute schema.
 * \author hiratake26to@gmail
 * \date 2017
 */

#include "schema/Schema.hpp"

#include <boost/algorithm/string.hpp>

using namespace std;

namespace Schema {

  //
  // SchemaValue
  //
  SchemaValue::SchemaValue(const std::vector<std::string>& vals) : type(LIST) {
    std::vector<SchemaValue> temp;
    for (const auto& str : vals) {
      temp.push_back(str);
    }
    list = temp;
  }
  SchemaValue::SchemaValue(const std::string& value) : type(STRING) {
    str = value;
  }
  SchemaValue::SchemaValue(int value) : type(NUMBER) {
    num = value;
  }
  SchemaValue::~SchemaValue() {
    switch (type) {
    case UNDEF:
      break;
    case STRING:
      //delete str;
      //str = nullptr;
      break;
    case NUMBER:
      break;
    case LIST:
      break;
    }
  }
  std::string
  SchemaValue::toString() const {
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
    case LIST:
      stringstream ss;
      ss << '[';
      bool delim = false;
      for (const auto item : list) {
        if (delim) {
          ss << ',';
          delim = false;
        }
        ss << item.toString();
        delim = true;
      }
      ss << ']';
      ret = ss.str();
      break;
    }

    return ret;
  }

  //
  // Schema
  //
  Schema&
  Schema::at(std::vector<std::string> key_hierarchy) {
    const string head = key_hierarchy[0];
    if (key_hierarchy.size() == 1) {
      return s_tbl[head];
    }
    key_hierarchy.erase(key_hierarchy.begin());
    return (s_tbl[head]).at(key_hierarchy);
  }

  Schema&
  Schema::at(std::string key_hierarchy) {
    using namespace boost::algorithm;
    std::vector<std::string> v;
    split(v, key_hierarchy, is_any_of("."));
    return at(v);
  }

  Schema
  Schema::get(std::vector<std::string> key_hierarchy) {
    return at(key_hierarchy);
  }

  /** add schema */
  void
  Schema::update(std::string key_hierarchy, std::string key, std::string value) {
    at(key_hierarchy).update(key, value);
  }
  void
  Schema::update(std::string key_hierarchy, std::string key, std::vector<std::string> value) {
    at(key_hierarchy).update(key, value);
  }
  void
  Schema::update(std::string key_hierarchy, std::string key, int value) {
    at(key_hierarchy).update(key, value);
  }

  /** add schema */
  void
  Schema::update(std::string key, std::string value) {
    v_tbl[key] = SchemaValue(value);
  }
  void
  Schema::update(std::string key, std::vector<std::string> value) {
    v_tbl[key] = SchemaValue(value);
  }
  void
  Schema::update(std::string key, int value) {
    v_tbl[key] = SchemaValue(value);
  }

  /** get all keys */
  std::vector<std::string>
  Schema::keys() {
    std::vector<std::string> ret;
    for (const auto& item : s_tbl) {
      ret.push_back(item.first);
    }
    for (const auto& item : v_tbl) {
      ret.push_back(item.first);
    }
    return ret;
  }

  std::string
  Schema::toString() const {
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

  std::pair<std::string,std::string>
  Schema::separate(const std::string &s, char delim) {
    pair<string, string> ret;

    //size_t s.find(delim);

    return ret;
  }

} // namespace Schema
