# ESP32 Wi-Fi AP Config Portal

This project lets you configure Wi-Fi credentials on an ESP32 via a captive portal (Access Point mode) using the Arduino framework.

## Features

- ESP32 starts as an Access Point if no Wi-Fi credentials are set
- Captive portal web page for entering SSID and password
- Stores credentials and connects to Wi-Fi automatically
- Button to trigger AP mode for reconfiguration

## How it works

1. On first boot (or if no SSID is set), ESP32 starts as an AP (`test_config`).
2. Connect to the AP with your phone/laptop.
3. Open a browser and go to `192.168.4.1` to access the config page.
4. Enter your Wi-Fi SSID and password.
5. ESP32 saves the credentials, reboots, and connects to your Wi-Fi.

## File Structure

```
esp32-wifi-ap-config-portal/
├── platformio.ini
├── src/
│   └── main.cpp
├── include/
├── lib/
├── test/
└── .pio/ (auto-generated, can be ignored)
```

## Usage

- Build and upload with PlatformIO or Arduino IDE.
- Press the boot button to enter AP mode at any time.

---

**Project by [zamilaqeel](https://github.com/zamilaqeel)**
