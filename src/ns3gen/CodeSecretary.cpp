/*
The MIT License

Copyright 2019 hiratake26to

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/**
 * \file CodeSecretary.cpp
 * \author hiratake26to@gmail
 * \date 2017
 */

#include "ns3gen/CodeSecretary.hpp"

void
CodeSecretary::push_back(std::string line) {
  m_code.push_back(m_indent+line);
}
void
CodeSecretary::push_back(std::vector<std::string> lines) {
  for (const auto& l : lines) push_back(l);
}

void
CodeSecretary::indentRight() {
  m_indent += "  ";
}

void
CodeSecretary::indentLeft() {
  if (m_indent.empty()) return;
  m_indent.pop_back();
  m_indent.pop_back();
}

std::vector<std::string>
CodeSecretary::get() {
  return m_code;
}
