#pragma once

#include <Arduino.h>
#include <Utilify/CommandInterpreter/HasOptionalLibs.h>
#include <Utilify/BasicVector.h>

class Device {
 public:
  static String getId();
  static vector<uint16_t> scanI2CAddresses();
};
