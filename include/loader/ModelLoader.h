#pragma once

#include "../network-prvt.h"

/** @brief Loader of each model **/
template <class T>
class ModelLoader {
  std::string file_name;
public:
  ModelLoader(std::string file_name);
  /** load model from file **/
  T Load();
};
