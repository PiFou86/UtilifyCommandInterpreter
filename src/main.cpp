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

#include "CustomCommandInterpreter.h"

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