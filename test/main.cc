#include "share.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;

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

