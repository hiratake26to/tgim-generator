#pragma once

#include "node/Node.h"

struct LinkAttr {
  int first;
  int second;
  int rate;
  int delay;
public:
  void setNodePare(int first, int second);
  void setFirst(int first);
  void setSecond(int second);
  int getFirst();
  int getSecond();
/*
  void setRate(int rate);
  void getRate(int rate);

  void setDelay(int delay);
  void getDelay(int delay);
  */
};
