#pragma once
#include <json.hpp>
using json = nlohmann::json;

class AppModelLoader {
  std::string load_path;
public:
  AppModelLoader() {
    load_path = "";
  }
  void setPath(std::string path) {
    load_path = path;
  }
  json load() const {
    json ret;
    std::ifstream ifs(load_path);
    ifs >> ret;
    ifs.close();
    return ret;
  }
};
