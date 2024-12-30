#pragma once

#include <Arduino.h>
#include <Utilify/BasicVector.h>

#ifdef ESP32

struct WiFiNetwork {
  String ssid;
  long rssi;
  byte encryptionType;
  String bssid;
};

class ESP32Utils {
 public:
  static void printWiFiInfo(Stream &stream);
  static void printPartitionInfo(Stream &stream);
  static void printWiFiScan(Stream &stream);
  static vector<WiFiNetwork> getWiFiNetworks();
};

#endif