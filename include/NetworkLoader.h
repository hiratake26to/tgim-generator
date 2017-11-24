#pragma once

#include "net/Network.h"
#include <string>

class NetworkLoader {
public:
  Network load(std::string filename);
};
