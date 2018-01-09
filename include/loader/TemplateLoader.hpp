#pragma once
#include <json.hpp>
using json = nlohmann::json;

class TemplateLoader {
  std::string load_path;
public:
  TemplateLoader() {
    load_path = "";
  }
  void setPath(std::string path) {
    load_path = path;
  }
  json load() {
    json ret;
    std::ifstream ifs(load_path);
    ifs >> ret;
    ifs.close();
    return ret;
  }
};
