#pragma once

#include <string>

class IpAddress;
class Transport;
class App;

class InternetStack {
  IpAddress ip();
  Transport transport();
  //Routing routing;
  App app();
};

class IpAddress {
  unsigned char bi[4];
public:
  IpAddress(unsigned char b1 = 0
          , unsigned char b2 = 0
          , unsigned char b3 = 0
          , unsigned char b4 = 0);
};

class Transport {
public:
  Transport(std::string protocol = "TCP");
};

class App {
public:
  App(std::string name = "Default");
};
