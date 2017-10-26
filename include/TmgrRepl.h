/*
The MIT License

Copyright (c) 2017 Hiratake Lab.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Author: hiratake26to@gmail.com
*/

/**
 * \file TmgrRepl.h
 * \brief Topology manager REPL class
 * \author hiratake26to@gmail
 * \date 2017
 */

#pragma once

#include <iostream>
#include <string>

#include "network.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;

class Repl {
  NetUnit *net;

  vector<NetUnit> units;
  vector<Node> nodes;
public:
  enum Res {
    QUIT,
    TEST,
    SUCCESS,
  };

public:
  Repl() {
    net = new NetUnit("type", "unit_0");
  }
  ~Repl() {
    delete net;
  }
  Res eval(const std::string& cmd) {
    if (cmd == "quit") return QUIT;
    if (cmd == "test") return TEST;
    if (cmd == "dump") return dump();
    if (cmd == "addUnit") return addUnit();
    if (cmd == "addNode") return addNode();
  }

private:
  Res addUnit() {
    cout << "not available!" << endl;
    return SUCCESS;
  }

  Res addNode() {
    Node a;
    a.SetId(nodes.size());
    nodes.push_back(a);
    net->AddNode(a);
    return SUCCESS;
  }

  Res dump() {
    net->DumpJson();
    return SUCCESS;
  }
};
