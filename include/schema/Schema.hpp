/*
The MIT License

Copyright (c) 2017 Hiratake Lab.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Author: hiratake26to@gmail.com
*/

/**
 * \file Schema.hpp
 * \brief Network attribute schema.
 * \author hiratake26to@gmail
 * \date 2017
 */

#pragma once

#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

namespace Schema {

class SchemaValue {
  enum { UNDEF, STRING, NUMBER, LIST } type = UNDEF;
  std::vector<SchemaValue> list;
  std::string str;
  int num;

public:
  SchemaValue() = default;
  SchemaValue(const std::vector<std::string>& vals);
  SchemaValue(const std::string& value);
  SchemaValue(int value);
  ~SchemaValue();
  std::string toString() const;
};

class Schema {
private:
  std::map<std::string, Schema> s_tbl;
  std::map<std::string, SchemaValue> v_tbl;

protected:
  Schema& at(std::vector<std::string> key_hierarchy);

public:
  Schema() = default;

  Schema& at(std::string key_hierarchy);

  Schema get(std::vector<std::string> key_hierarchy);

  /** add schema */
  virtual void update(std::string key_hierarchy, std::string key, std::string value);
  virtual void update(std::string key_hierarchy, std::string key, std::vector<std::string> value);
  virtual void update(std::string key_hierarchy, std::string key, int value);

  /** add schema */
  virtual void update(std::string key, std::string value);
  virtual void update(std::string key, std::vector<std::string> value);
  virtual void update(std::string key, int value);

  /** get all keys */
  virtual std::vector<std::string> keys();

  std::string toString() const;

private:
  std::pair<std::string,std::string> separate(const std::string &s, char delim);
};

} // namespace Schema
