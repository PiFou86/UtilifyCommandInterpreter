#include <Arduino.h>

#ifdef ESP32
#include <WiFi.h>
#include <ping/ping.h>
#endif

// optional include
#if __has_include("WiFiConfig.h")
#include "WiFiConfig.h"
#else
#define WIFI_STA_SSID "Erable"
#define WIFI_STA_PASSWORD "TastyMapleSyrup_ChocolatineIsTheBest"
#define WIFI_AP_SSID "Erable"
#define WIFI_AP_PASSWORD "TastyMapleSyrup_ChocolatineIsTheBest"
#endif

#include <Utilify/CommandInterpreter/BaseCommandInterpreter.h>

class CustomCommandInterpreter : public BaseCommandInterpreter {
 protected:
  virtual bool interpret(const String &command,
                         const String &parameters) override {
    bool error = false;

    if (command == "test") {
      Serial.print(F("test command executed with parameters: "));
      Serial.println(parameters);
    } else if (command == "switch") {
      if (parameters == "on") {
        Serial.println(F("Switch turned ON"));
        digitalWrite(LED_BUILTIN, HIGH);  // Turn on the built-in LED
      } else if (parameters == "off") {
        Serial.println(F("Switch turned OFF"));
        digitalWrite(LED_BUILTIN, LOW);  // Turn off the built-in LED
      } else {
        Serial.println(F("Invalid parameters for switch command"));
        error = true;
      }
    } else {
      error = !BaseCommandInterpreter::interpret(command, parameters);
    }

    return !error;
  }
  virtual bool getParameter(const String &key) override {
    // Custom code here to handle custom keys and values
    return BaseCommandInterpreter::getParameter(key);
  }
  virtual bool setParameter(const String &key, const String &value) override {
    // Custom code here to handle custom keys and values
    return BaseCommandInterpreter::setParameter(key, value);
  }

 public:
  CustomCommandInterpreter(Stream &stream) : BaseCommandInterpreter(stream) {
    addCommand(F("test"), F("<parameters>"),
               F("Test command that prints the <parameters> to the console"));
    addCommand(F("switch"), F("on"), F("Turn the built-in LED ON"));
    addCommand(F("switch"), F("off"), F("Turn the built-in LED OFF"));

    pinMode(LED_BUILTIN, OUTPUT);
  }
};

CustomCommandInterpreter commandInterpreter(Serial);
void setup() {
  Serial.begin(115200);
  Serial.println("Starting ESP32");

#ifdef ESP32
  // Connect to WiFi
  Serial.println("Connecting to WiFi : ");
  int tries = 0;
  WiFi.begin(WIFI_STA_SSID, WIFI_STA_PASSWORD);
  while (WiFi.status() != WL_CONNECTED && tries++ < 20) {
    delay(500);
    Serial.print('.');
  }

  // Create AP with IP 192.168.23.1
  IPAddress local_ip(192, 168, 23, 1);
  IPAddress gateway(192, 168, 23, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  IPAddress IP = WiFi.softAPIP();
  WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASSWORD);
  Serial.print("AP IP address: ");
  Serial.println(IP);
#endif

  commandInterpreter.begin();
}

void loop() { commandInterpreter.tick(); }