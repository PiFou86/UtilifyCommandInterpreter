#pragma once

#include <Arduino.h>

#include <Utilify/CommandInterpreter/HasOptionalLibs.h>

#include <Utilify/CommandInterpreter/SimpleCollection.h>

#ifdef ESP32
#include <vector>
#endif

struct WiFiNetwork {
  String ssid;
  long rssi;
  byte encryptionType;
  String bssid;
};

class Device {
 public:
  static String getId();
  #ifdef ARDUINO_AVR_UNO
  static SimpleCollection<uint16_t> getI2CAddresses();
  #elif ESP32
  static std::vector<uint16_t> getI2CAddresses();
  #endif

#ifdef ESP32
  static std::vector<WiFiNetwork> getWiFiNetworks();
#endif
};
