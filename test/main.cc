#include "share.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;

#include <boost/filesystem.hpp>

void generate(std::string path) {
  namespace fs = boost::filesystem;

  NetworkLoader loader;
  Network net = loader.load(path);
  net.DumpJson();
  NetworkGenerator gen(net);

  const fs::path p(path);
  std::string out_name = p.stem().string();

  for (auto line : gen.CppCode(out_name)) {
    cout << line << endl;
  }
}

int main(int argc, char *argv[]) {
  cout << "version : " << TMGR_VERSION << endl;

  //
  // initialization
  //

  using namespace std;
  namespace po = boost::program_options;
  po::options_description desc("option");
  desc.add_options()
    ("help,h", "show help")
    ("debug", "debug mode")
    ("input-file", po::value<vector<string>>()->multitoken(), "input file (.json)")
  ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    cout << desc << endl;
    return 1;
  }
  if (vm.count("debug")) {
    eval_test();
    return 0;
  }
  if (vm.count("input-file")) {
    for (const auto& file : vm["input-file"].as<vector<string>>())
    {
      cout << file << endl;
      generate(file);
    }
    return 0;
  }

  //
  // main
  //
  
  Repl repl;
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

