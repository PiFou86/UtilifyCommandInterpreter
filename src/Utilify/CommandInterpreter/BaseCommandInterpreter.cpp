#include <Arduino.h>
#include <Stream.h>

#include <Utilify/CommandInterpreter/BaseCommandInterpreter.h>

#include <Utilify/CommandInterpreter/Device.h>

#include <Utilify/StringUtils.h>

#ifdef ESP32
#include <ESP.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <vector>
#endif

#ifdef UTILIFY_LOGGER
#include <Utilify/Logger/Logger.h>
#endif

BaseCommandInterpreter::BaseCommandInterpreter(Stream &stream)
    : m_stream(stream), m_lastSerialInput("") {
  ;
}

void BaseCommandInterpreter::tick() {
  while (this->m_stream.available()) {
    char c = this->m_stream.read();
    this->m_stream.print(c);
    if (c == '\n') {
      this->executeCommand(m_lastSerialInput);
      this->m_lastSerialInput = "";
    } else {
      this->m_lastSerialInput += c;
    }
  }
}

bool BaseCommandInterpreter::executeCommand(const String &command) {
  String cmd = command;

  String parameters = "";
  int index = cmd.indexOf(' ');
  if (index != -1) {
    parameters = cmd.substring(index + 1);
    cmd = cmd.substring(0, index);
  }
  cmd.trim();
  parameters.trim();

#ifdef UTILIFY_LOGGER
  Logger.verboseln(String(F("> ")) + cmd + " " + parameters);
#endif

  return this->interpret(cmd, parameters);
}

bool BaseCommandInterpreter::interpret(const String &command,
                                        const String &parameters) {
  bool error = false;

  if (command == "hello") {
    m_stream.println(F("Hello, world!"));
  } else if (command == "set") {
    String key = parameters.substring(0, parameters.indexOf(' '));
    String value = parameters.substring(parameters.indexOf(' ') + 1);
    error = !this->setParameter(key, value);
  } else if (command == "get") {
    String key = parameters;
    error = !this->getParameter(key);
  } else if (command == "id") {
    m_stream.println(String(F("ID: ")) + Device::getId());
  } 
#ifdef ESP32
  else if (command == "reboot") {
    Serial.println(F("Rebooting..."));
    ESP.restart();
  } 
#endif  
  else if (command == "scan") {
    String deviceType = parameters;

    if (deviceType == "i2c") {
      #ifdef ARDUINO_AVR_UNO
      SimpleCollection<uint16_t> i2cAddresses = Device::getI2CAddresses();
      #elif ESP32
      std::vector<uint16_t> i2cAddresses = Device::getI2CAddresses();
      #endif
      m_stream.println(F("I2C addresses:"));
      for (size_t i = 0; i < i2cAddresses.size(); i++) {
        m_stream.println(String(F("  - 0x")) +
                         StringUtils::padLeft(String(i2cAddresses[i], HEX), 2, '0'));
      }
      m_stream.println(String(F("")));
    } 
    #ifdef ESP32
    else if (deviceType == "wifi") {
      std::vector<WiFiNetwork> networks = Device::getWiFiNetworks();
      m_stream.println(F("Wifi networks:"));
      for (size_t i = 0; i < networks.size(); i++) {
        String encryption = "unknown";
        switch (networks[i].encryptionType) {
          case (WIFI_AUTH_OPEN):
            encryption = "Open";
            break;
          case (WIFI_AUTH_WEP):
            encryption = "WEP";
            break;
          case (WIFI_AUTH_WPA_PSK):
            encryption = "WPA_PSK";
            break;
          case (WIFI_AUTH_WPA2_PSK):
            encryption = "WPA2_PSK";
            break;
          case (WIFI_AUTH_WPA_WPA2_PSK):
            encryption = "WPA_WPA2_PSK";
            break;
          case (WIFI_AUTH_WPA2_ENTERPRISE):
            encryption = "WPA2_ENTERPRISE";
            break;
        }
        m_stream.println(
            String(F("  - ")) + networks[i].ssid + String(F(" (")) +
            encryption + String(F(") : ")) + String(networks[i].rssi) +
            String(F(" dBm - ")) +
            String(constrain((100.0 + networks[i].rssi) * 2, 0, 100)) +
            String(F("% - BSSID: ")) + networks[i].bssid);
      }
      m_stream.println("");
    }
    #endif
     else {
      m_stream.println(String(F("Unknown device type ")) + deviceType);
    }
  } 
  #ifdef ESP32
  else if (command == "flash") {
    m_stream.println(String(F("Flash size: ")) + ESP.getFlashChipSize() +
                     String(F(" bytes")));
    m_stream.println(String(F("Flash speed: ")) + ESP.getFlashChipSpeed() +
                     String(F(" Hz")));
    m_stream.println(String(F("Flash mode: ")) + ESP.getFlashChipMode());
  } else if (command == "network") {
    m_stream.println(String(F("Network: ")));
    m_stream.println(String(F("  - SSID: ")) + WiFi.SSID());
    m_stream.println(String(F("  - IP: ")) + WiFi.localIP().toString());
    m_stream.println(String(F("  - MAC: ")) + WiFi.macAddress());
    m_stream.println(String(F("  - RSSI: ")) + String(WiFi.RSSI()) +
                     String(F(" dBm")));
    m_stream.println(String(F("  - Channel: ")) + String(WiFi.channel()));
    m_stream.println(String(F("  - DNS: ")) + WiFi.dnsIP().toString());
    m_stream.println(String(F("  - DNS: ")) + WiFi.dnsIP(1).toString());
    wifi_config_t conf;
    esp_wifi_get_config(WIFI_IF_STA, &conf);
    m_stream.println(String(F("  - SSID: ")) + String((char *)conf.sta.ssid));
    m_stream.println(String(F("  - Password: ")) +
                       String((char *)conf.sta.password));

    m_stream.println(String(F("")));
  }
#endif
  else if (command == "help") {
    m_stream.println(F("Available commands:"));
    m_stream.println(F("  hello"));
    m_stream.println(F("  id"));
    m_stream.println(F("  set <key> <value>"));
    m_stream.println(F("  get <key>"));
#ifdef ESP32
    m_stream.println(F("  scan i2c|wifi"));
    m_stream.println(F("  reboot"));
    m_stream.println(F("  flash"));
    m_stream.println(F("  network"));
#elif ARDUINO_AVR_UNO
    m_stream.println(F("  scan i2c"));
#endif
    m_stream.println(F(""));
    m_stream.println(F("  help"));
  } else {
    m_stream.println(String(F("Unknown command: ")) + command);
    error = true;
  }

  return !error;
}

bool BaseCommandInterpreter::getParameter(const String &key) {
  bool error = false;
  bool keyFound = false;

#ifdef UTILIFY_LOGGER
  if (key == "debug") {
    keyFound = true;
    m_stream.println(String(F("debug: ")) + Logger.getLoggerLevel());
  }
#endif
  
  if (!keyFound) {
    m_stream.println(F("get : Invalid key"));
    error = true;
  }

  return !error;
}

bool BaseCommandInterpreter::setParameter(const String &key,
                                          const String &value) {
  bool error = false;
  bool keyFound = false;

#ifdef UTILIFY_LOGGER
  if (key == "debug") {
    keyFound = true;
    int debugLevel = value.toInt();
    if (debugLevel > 0 && debugLevel <= 4) {
      Logger.setLoggerLevel((LoggerLevel)debugLevel);
    } else {
      m_stream.println(F("Invalid debug level"));
    }
  } 
#endif

  if (!keyFound) {
    m_stream.println(F("set : Invalid key"));
    error = true;
  }

  return !error;
}