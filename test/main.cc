#include "share.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;

#include <boost/filesystem.hpp>
#include "loader/TemplateLoader.hpp"

class GenW {
  static std::string output;
  static std::string codetemp_path;
  static std::string appmodel_path;
public:
  static void setOutput(std::string name) {
    GenW::output = name;
  }
  static void setTemplatePath(std::string path) {
    GenW::codetemp_path = path;
  }
  static void setAppModelPath(std::string path) {
    GenW::appmodel_path = path;
  }
  static std::string getOutput() {
    return GenW::output;
  }
  static std::string getTemplatePath() {
    return GenW::codetemp_path;
  }
  static std::string getAppModelPath() {
    return GenW::appmodel_path;
  }
  static void generate(const std::string& path) {
    NetworkLoader loader;
    Network net = loader.load(path);
    //net.DumpJson();
    TemplateLoader tloader;
    AppModelLoader aloader;
    tloader.setPath(GenW::codetemp_path);
    aloader.setPath(GenW::appmodel_path);
    NetworkGenerator gen(net, tloader, aloader);

    std::ofstream ofs(GenW::output);
    for (auto line : gen.CppCode()) {
      //cout << line << endl;
      ofs << line << endl;
    }
    ofs.close();
  }
};

std::string GenW::output;
std::string GenW::codetemp_path;
std::string GenW::appmodel_path;


int main(int argc, char *argv[]) {
  //
  // initialization
  //

  using namespace std;
  namespace po = boost::program_options;
  po::options_description desc("option");
  desc.add_options()
    ("help,h",    "show help")
    ("version,v", "show version")
    ("output,o",      po::value<string>()->value_name("<file>"), "place the output into <file>")
    ("appmodel-path", po::value<string>()->value_name("<path>"), "ns3 application model path\n(default ./src/model/application.json)")
    ("template-path", po::value<string>()->value_name("<path>"), "ns3 code template path\n(default ./resource/ns3template-cxx.json)")
    ("debug", "debug mode")
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
  if (vm.count("output")) {
    const auto odir = vm["output"].as<string>();
    GenW::setOutput(odir);
  } else {
    GenW::setOutput("");
  }
  if (vm.count("template-path")) {
    const auto tpath = vm["template-path"].as<string>();
    GenW::setTemplatePath(tpath);
  } else {
    GenW::setTemplatePath("./resource/ns3template-cxx.json");
  }
  if (vm.count("appmodel-path")) {
    const auto apath = vm["appmodel-path"].as<string>();
    GenW::setAppModelPath(apath);
  } else {
    GenW::setAppModelPath("./src/model/application.json");
  }

  // run generator
  if ( po::collect_unrecognized(parsing_result.options, po::include_positional).size() ) {
    if (po::collect_unrecognized(parsing_result.options, po::include_positional).size() != 1) {
      std::cerr << "file argument error!" << endl;
      return 1;
    }

    std::string file = po::collect_unrecognized(parsing_result.options, po::include_positional).at(0);
    namespace fs = boost::filesystem;
    const fs::path p(file);
    if (GenW::getOutput() == "") {
      auto o = p.stem().string() + ".hpp";
      GenW::setOutput(o);
    }

    // show config
    cout << "==> Configuration" << endl;
    cout << "output=\"" << GenW::getOutput() << "\"" << endl;
    cout << "template-path=\"" << GenW::getTemplatePath() << "\"" << endl;
    cout << "appmodel-path=\"" << GenW::getAppModelPath() << "\"" << endl;

    cout << "==> Convert: " << file << endl;
    try {
      GenW::generate(file);
    } catch(std::exception e) {
      std::cerr << e.what() << std::endl;
      cout << "Convert failed" << std::endl;
      return -1;
    }
    cout << "OK" << endl;

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

