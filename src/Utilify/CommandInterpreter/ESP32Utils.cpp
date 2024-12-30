
#ifdef ESP32
#include <ESP.h>

#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_netif.h>

#include <esp_ping.h>
#include <esp_log.h>
#include <lwip/inet.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <ping/ping.h>

#include <esp_partition.h>
#include <LittleFS.h>

#include <vector>

#include <Utilify/CommandInterpreter/HasOptionalLibs.h>

#include <Utilify/CommandInterpreter/ESP32Utils.h>

#include <Utilify/CommandInterpreter/Device.h>

void ESP32Utils::printWiFiInfo(Stream &stream) {
    wifi_config_t conf;
    esp_netif_t *netif_sta = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    esp_netif_t *netif_ap = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
    esp_netif_ip_info_t ip_info;
    esp_netif_dns_info_t dns_info;

    // WiFi STA Information
    stream.println(F("WiFi STA:"));
    if (esp_wifi_get_config(WIFI_IF_STA, &conf) == ESP_OK) {
        stream.println(String(F("  - SSID: ")) + String((char *)conf.sta.ssid));
        stream.println(String(F("  - Password: ")) + String((char *)conf.sta.password));
    } else {
        stream.println(F("  - STA configuration not available"));
    }

    if (netif_sta && esp_netif_get_ip_info(netif_sta, &ip_info) == ESP_OK) {
        char ip[16];
        esp_ip4addr_ntoa(&ip_info.ip, ip, sizeof(ip));
        stream.println(String(F("  - IP: ")) + String(ip));

        esp_ip4addr_ntoa(&ip_info.netmask, ip, sizeof(ip));
        stream.println(String(F("  - Netmask: ")) + String(ip));

        esp_ip4addr_ntoa(&ip_info.gw, ip, sizeof(ip));
        stream.println(String(F("  - Gateway: ")) + String(ip));
    } else {
        stream.println(F("  - STA IP information not available"));
    }

    stream.println(String(F("  - MAC: ")) + WiFi.macAddress());
    stream.println(String(F("  - BSSID: ")) + WiFi.BSSIDstr());

    for (int i = 0; i < 2; ++i) { // Primary and secondary DNS
        if (netif_sta && esp_netif_get_dns_info(netif_sta, (esp_netif_dns_type_t)i, &dns_info) == ESP_OK) {
            char ip[16];
            esp_ip4addr_ntoa(&dns_info.ip.u_addr.ip4, ip, sizeof(ip));
            stream.println(String(F("  - DNS ")) + String(i) + String(F(": ")) + String(ip));
        }
    }

    // WiFi AP Information
    stream.println(F("\nWiFi AP:"));
    if (esp_wifi_get_config(WIFI_IF_AP, &conf) == ESP_OK) {
        stream.println(String(F("  - SSID: ")) + String((char *)conf.ap.ssid));
        stream.println(String(F("  - Password: ")) + String((char *)conf.ap.password));
    } else {
        stream.println(F("  - AP configuration not available"));
    }

    if (netif_ap && esp_netif_get_ip_info(netif_ap, &ip_info) == ESP_OK) {
        char ip[16];
        esp_ip4addr_ntoa(&ip_info.ip, ip, sizeof(ip));
        stream.println(String(F("  - IP: ")) + String(ip));

        esp_ip4addr_ntoa(&ip_info.netmask, ip, sizeof(ip));
        stream.println(String(F("  - Netmask: ")) + String(ip));

        esp_ip4addr_ntoa(&ip_info.gw, ip, sizeof(ip));
        stream.println(String(F("  - Gateway: ")) + String(ip));
    } else {
        stream.println(F("  - AP IP information not available"));
    }

    stream.println(String(F("  - MAC: ")) + WiFi.softAPmacAddress());
    stream.println(String(F("  - BSSID: ")) + WiFi.softAPmacAddress());

    stream.println("");
}

void ESP32Utils::printPartitionInfo(Stream &stream) {
    const esp_partition_t *partition = NULL;
    esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, NULL);

    stream.println(F("Partitions:"));
    while (it != NULL) {
        partition = esp_partition_get(it);
        if (partition != NULL) {
            stream.println(String(F("  - Label: ")) + partition->label);
            stream.println(String(F("    Type: ")) + partition->type);
            stream.println(String(F("    Subtype: ")) + partition->subtype);
            stream.println(String(F("    Address: 0x")) + String(partition->address, HEX));
            stream.println(String(F("    Size: ")) + String(partition->size) + F(" bytes"));

            // Vérifie si la partition est LittleFS
            if ((partition->type == ESP_PARTITION_TYPE_DATA &&
                 partition->subtype == ESP_PARTITION_SUBTYPE_DATA_FAT) || // Alternative pour LittleFS
                strcmp(partition->label, "littlefs") == 0) { // Vérifie le label
                stream.println(F("    Filesystem: LittleFS"));

                // Monte la partition LittleFS
                if (!LittleFS.begin(true)) {
                    stream.println(F("      Failed to mount LittleFS"));
                } else {
                    size_t totalBytes = LittleFS.totalBytes();
                    size_t usedBytes = LittleFS.usedBytes();
                    stream.println(String(F("      Total Size: ")) + String(totalBytes) + F(" bytes"));
                    stream.println(String(F("      Used: ")) + String(usedBytes) + F(" bytes"));
                    stream.println(String(F("      Free: ")) + String(totalBytes - usedBytes) + F(" bytes"));
                }

                // Démonte LittleFS après utilisation
                LittleFS.end();
            } else {
                stream.println(F("    Filesystem: None/Unsupported"));
            }
        }
        it = esp_partition_next(it);
    }
    esp_partition_iterator_release(it);
}

void ESP32Utils::printWiFiScan(Stream &stream) {
    vector<WiFiNetwork> networks = ESP32Utils::getWiFiNetworks();
      stream.println(F("Wifi networks:"));
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
        stream.println(
            String(F("  - ")) + networks[i].ssid + String(F(" (")) +
            encryption + String(F(") : ")) + String(networks[i].rssi) +
            String(F(" dBm - ")) +
            String(constrain((100.0 + networks[i].rssi) * 2, 0, 100)) +
            String(F("% - BSSID: ")) + networks[i].bssid);
      }
      stream.println("");
}

vector<WiFiNetwork> ESP32Utils::getWiFiNetworks() {
  vector<WiFiNetwork> networks;
  WiFi.scanNetworks();
  for (int i = 0; i < WiFi.scanComplete(); i++) {
    WiFiNetwork network;
    network.ssid = WiFi.SSID(i);
    network.rssi = WiFi.RSSI(i);
    network.encryptionType = WiFi.encryptionType(i);
    network.bssid = WiFi.BSSIDstr(i);
    networks.push_back(network);
  }
  return networks;
}
#endif