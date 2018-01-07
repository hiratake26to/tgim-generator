/*
The MIT License

Copyright (c) 2017 Hiratake Lab.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Author: hiratake26to@gmail.com
*/

/**
 * \file GenUtil.hpp
 * \brief Generator utility.
 * \author hiratake26to@gmail
 * \date 2017
 */

#pragma once

#include <string>

class AddressValue {
  std::string m_local;
  std::string m_mask;
  void parse_and_set(const std::string& value);
public:
  AddressValue(const std::string& value);
  std::string GetLocal();
  std::string GetMask();
};

class AddressGenerator {
  // 10.0.0.0 ~ 10.255.255.255
  static uint32_t address_net;
  static uint32_t address_mask;
  static uint32_t address_last;
  static std::string toStrAddr(uint32_t addr);
public:
  static void Init();
  static std::string GetLocal();
  static std::string GetMask();
  static void Next();
};
