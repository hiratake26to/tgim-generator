#pragma once

#include <string>

namespace Schema {

// Luaなどの組み込み言語を使用して動的にトポロジデータの管理ができるようにしたい
class Schema {
public:
  Schema() = default;
  void addKey(std::string key);
  void addValue(std::string key, std::string value);
};

}
