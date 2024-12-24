#include <Arduino.h>

#ifdef ESP32
#include <WiFi.h>
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
    WiFi.begin("Erable", "LeoHortense86@@");
    while (WiFi.status() != WL_CONNECTED && tries++ < 20) {
        delay(500);
        Serial.println('.');
    }

    // Create AP with IP 192.168.23.1 
    IPAddress local_ip(192, 168, 23, 1);
    IPAddress gateway(192, 168, 23, 1);
    IPAddress subnet(255, 255, 255, 0);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    IPAddress IP = WiFi.softAPIP();
    WiFi.softAP("ESP32-AP", "1234567890");
    Serial.print("AP IP address: ");
    Serial.println(IP);
#endif

}

void loop() { commandInterpreter.tick(); }