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

void eval_test();

int main(int argc, char *argv[]) {

  //
  // initialization
  //

  if (argc > 1) {
    if (string(argv[1]) == "--debug") {
      eval_test();
      return 0;
    }
  }

  Repl repl;
  //
  // main
  //
  
  std::string buf;
  while (true) {
    cout << ">";
    cin >> buf;
    if (cin.eof()) return 0;
    switch (repl.eval(buf))
    {
    case Repl::Res::QUIT:
      return 0;
      break;
    case Repl::Res::TEST:
      eval_test();
      break;
    case Repl::Res::SUCCESS:
      break;
    default:
      cout << "what!?" << endl;
      break;
    }
    buf.clear();
  }

  //
  // test
  //
  
  return 0;
}

void eval_test() {
  Network net;
  NetUnit uni("type", "uni_1");
    Node a; a.SetId(0);
    Node b; b.SetId(1);
  uni.AddNode(a);
  uni.AddNode(b);
  uni.AddLink(a, b);

  uni.DumpJson();
}
