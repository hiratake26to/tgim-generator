#include "share.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;

#include <boost/filesystem.hpp>
#include "loader/TemplateLoader.hpp"

class GenW {
  static std::string out_dir;
  static std::string codetemp_path;
public:
  static void setOutDir(std::string dir) {
    GenW::out_dir = dir;
  }
  static void setTemplatePath(std::string path) {
    GenW::codetemp_path = path;
  }
  static std::string getOutDir() {
    return GenW::out_dir;
  }
  static std::string getTemplatePath() {
    return GenW::codetemp_path;
  }
  static void generate(const std::string& path) {
    namespace fs = boost::filesystem;

    NetworkLoader loader;
    Network net = loader.load(path);
    //net.DumpJson();
    TemplateLoader tloader;
    tloader.setPath(GenW::codetemp_path);
    NetworkGenerator gen(net, tloader);

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

std::string GenW::out_dir;
std::string GenW::codetemp_path;


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
    ("output-dir", po::value<string>(), "output directory\n(default ./out/)")
    ("template-path", po::value<string>(), "ns3 code template path\n(default ./resource/ns3template-cxx.json)")
  ;

  po::variables_map vm;
  auto const parsing_result = po::parse_command_line(argc, argv, desc);
  po::store(parsing_result, vm);
  po::notify(vm);

  if (vm.count("help")) {
    cout << "Usage: " << argv[0] << " [options] file..." << endl;
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
    const auto odir = vm["output-dir"].as<string>();
    GenW::setOutDir(odir);
  } else {
    GenW::setOutDir("./out/");
  }
  if (vm.count("template-path")) {
    const auto tpath = vm["template-path"].as<string>();
    GenW::setTemplatePath(tpath);
  } else {
    GenW::setTemplatePath("./resource/ns3template-cxx.json");
  }

  // run generator
  if ( po::collect_unrecognized(parsing_result.options, po::include_positional).size() ) {
    // show config
    cout << "==> Configuration" << endl;
    cout << "output-dir=\"" << GenW::getOutDir() << "\"" << endl;
    cout << "template-path=\"" << GenW::getTemplatePath() << "\"" << endl;

    for (auto const& file : po::collect_unrecognized(parsing_result.options, po::include_positional)) {
      cout << "==> Convert: " << file << endl;
      GenW::generate(file);
      cout << "OK" << endl;
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

