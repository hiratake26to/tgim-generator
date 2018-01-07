#include "share.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;

#include <boost/filesystem.hpp>

class GenW {
  static std::string out_dir;
public:
  static void setOutDir(std::string dir) {
    GenW::out_dir = dir;
  }
  static void generate(const std::string& path) {
    namespace fs = boost::filesystem;

    NetworkLoader loader;
    Network net = loader.load(path);
    //net.DumpJson();
    NetworkGenerator gen(net);

    const fs::path p(path);
    std::string out_name = p.stem().string();

    std::ofstream ofs(GenW::out_dir + out_name +".hpp");
    for (auto line : gen.CppCode()) {
      //cout << line << endl;
      ofs << line << endl;
    }
    ofs.close();
  }
};
std::string GenW::out_dir = "./out/";

int main(int argc, char *argv[]) {
  //
  // initialization
  //

  using namespace std;
  namespace po = boost::program_options;
  po::options_description desc("option");
  desc.add_options()
    ("help,h", "show help")
    ("version,v", "show version")
    ("debug", "debug mode")
    ("output-dir", po::value<string>(), "output directory (default ./out/)")
  ;

  po::variables_map vm;
  auto const parsing_result = po::parse_command_line(argc, argv, desc);
  po::store(parsing_result, vm);
  po::notify(vm);

  if (vm.count("help")) {
    cout << "Usage: " << argv[0] << "[options] file..." << endl;
    cout << desc << endl;
    return 1;
  }
  if (vm.count("version")) {
    cout << "Version: " << TMGR_VERSION << endl;
    return 1;
  }
  if (vm.count("debug")) {
    eval_test();
    return 0;
  }
  if (vm.count("output-dir")) {
    // TODO: set output directory
    const auto odir = vm["output-dir"].as<string>();
    GenW::setOutDir(odir);
  }

  // run generator
  bool fexit = false;
  for (auto const& file : po::collect_unrecognized(parsing_result.options, po::include_positional)) {
    cout << "convert... " << file;
    GenW::generate(file);
    cout << " OK" << endl;
    fexit = true;
  }
  if ( fexit ) return 0;

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

