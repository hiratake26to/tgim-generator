#include <iostream>
#include <string>

#include "network.h"
#include "TmgrRepl.h"
#include "ns3gen/Generator.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;


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

  NetworkGenerator gen;
  for (auto line : gen.CppCode()) {
    cout << line << endl;
  }

  Network net;
  NetUnit uni("type", "uni_1");
    Node a; a.SetId(0);
    Node b; b.SetId(1);
  uni.AddNode(a);
  uni.AddNode(b);
  uni.AddLink(a, b);

  //uni.DumpJson();
}
