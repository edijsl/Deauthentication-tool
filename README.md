This project implements a Wi-Fi Deauthentication Tool running on an ESP32 microcontroller. The ESP32 creates a Wi-Fi Access Point (AP) and serves a web page where nearby Wi-Fi networks are scanned and listed in real-time. Users can trigger deauthentication (deauth) packets to selected networks directly from the browser interface.

⚠️ Disclaimer: This tool is for educational and research purposes only. Unauthorized use of deauthentication attacks is illegal and unethical. Always ensure you have permission to test on a given network.

🚀 Features

✅ ESP32 Access Point (AP) mode for easy connection.

✅ Real-time Wi-Fi network scanner via auto-refreshing web interface.

✅ One-click deauthentication attack on detected Wi-Fi networks.

✅ AJAX-based dynamic update of available networks without reloading the page.

✅ Custom-built deauth packet crafting and transmission at the firmware level.

🛠️ Requirements

ESP32 board (e.g., ESP32 Dev Module).

Arduino IDE or PlatformIO with ESP32 board support.

Libraries:

WiFi.h

WebServer.h

esp_wifi.h (part of ESP32 core libraries).

🔑 Access Point Credentials

Parameter	Value

SSID	65 64 69 6A 73 6C

Password	12345678

🌐 Web Interface

Upon connecting to the ESP32 AP, navigate to the device's IP address (shown in Serial Monitor, typically 192.168.4.1).


Functionalities:

Network Scan: Lists all nearby Wi-Fi networks with SSID, RSSI, and BSSID.

Auto-Refresh: Updates the list every 5 seconds.

Deauth Button: Triggers a deauthentication packet targeting the chosen BSSID.

⚔️ How Deauthentication Works

The ESP32:


Scans for nearby Wi-Fi networks and identifies channels/BSSIDs.

Allows user selection of target networks from the web page.

Switches to the appropriate Wi-Fi channel of the target.

Crafts a raw IEEE 802.11 deauthentication frame.

Sends the packet directly using low-level esp_wifi_80211_tx.

📄 File Breakdown

File	Description

main.cpp	Main firmware containing Wi-Fi AP setup, web server, Wi-Fi scanner, and deauth packet sender.

🚨 Legal Notice

IMPORTANT: This tool sends deauthentication frames, which can disrupt Wi-Fi connections.

Usage is restricted to authorized security testing and educational purposes.

Do NOT use on networks you do not own or without explicit permission.

The authors are not responsible for any misuse or illegal activities performed with this code.

✅ Usage

Flash the code onto your ESP32 board.

Connect to the ESP32 Wi-Fi network using the credentials.

Access the web interface through the provided IP.
View networks and optionally send deauth packets.
