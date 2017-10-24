/*
The MIT License

Copyright (c) 2017 Hiratake Lab.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Author: hiratake26to@gmail.com
*/

/**
 * \file pool.hpp
 * \brief Network object pool.
 * \author hiratake26to@gmail
 * \date 2017
 */

#pragma once

#include "model/network.hpp"

#include <string>
#include <vector>
#include <utility>

namespace Tgim {

using namespace NetModel;

class NetObject;

class NetObjPool;

//

/** @brief Network object pool **/
class NetObjPool {
public:
  NetObjPool();

  /** Create object form network model
   * @return object id
   * @retval -1 error                 **/
  int CreateFromNetModel(const NetModel& model);

  /** Get count of object in pool **/
  int GetCount();

  /** Return copy of object from id **/
  NetObject CopyObject(int id);

  /** Return reference of object from id **/
  NetObject& RefObject(int id);

  /** Delete object in pool **/
  bool DeleteObject(int id);
};

}
