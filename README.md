# ESP32-S3 Reflow Controller

A dual-core reflow oven controller built on the ESP32-S3 that reads temperature from a MAX6675 thermocouple, drives a TFT display showing a real-time temperature graph, and controls a fan, SSR, and buzzer based on temperature thresholds.

## Features

- **Real-time Temperature Monitoring** via MAX6675 thermocouple
- **TFT Display** (ILI9341) showing a live temperature graph
- **20x4 LCD** (I2C) for secondary status display
- **Fan Speed Control** with automatic duty cycle cycling (20-100%)
- **SSR Control** for heating element on/off
- **RGB LED Indicators** for temperature zones and system status
- **Rotary Encoder** with push button for manual control
- **Web Interface** with WebSocket for remote monitoring and control
- **WiFi Manager** for easy AP configuration
- **Dual-Core Architecture** for fault isolation

## Hardware

### Board
- ESP32-S3-DevKitC-1 (N16R8V - 16MB flash, 8MB PSRAM)

### Connected Devices
- MAX6675 Thermocouple (HSPI)
- ILI9341 TFT LCD 2.4" (HSPI)
- LCD 2004 with I2C backpack
- Rotary Encoder
- 2x RGB NeoPixel LEDs
- PWM Fan with tachometer
- Solid State Relay
- Passive Buzzer

### Pin Configuration
See [docs/esp32_S3_pins.txt](docs/esp32_S3_pins.txt) for full pinout.

| Pin | Function |
|-----|----------|
| 40 | MAX6675 SCK |
| 39 | MAX6675 MISO |
| 38 | MAX6675 CS |
| 10 | TFT CS |
| 11 | TFT MOSI |
| 12 | TFT SCLK |
| 6 | TFT RST |
| 7 | TFT DC |
| 42 | SSR Control |
| 5 | Fan PWM |
| 4 | Fan Tach |
| 14 | Buzzer |
| 16 | Rotary CLK |
| 17 | Rotary DT |
| 18 | Rotary SW |
| 8 | LCD SDA |
| 9 | LCD SCL |
| 48 | Onboard RGB LED |
| 47 | WiFi LED |
| 1 | External RGB LED String |

## Build & Upload

```bash
# Build
pio run

# Upload to device (COM4)
pio run --target upload

# Monitor serial output (COM3, 115200 baud)
pio run --target monitor
```

## Dual-Core Architecture

- **Core 1 (Main App)**: Thermocouple reading, display updates, fan control, rotary encoder, web server
- **Core 0 (Secondary)**: Heartbeat LED (isolated so main app crashes don't affect it)

## Web Interface

The controller hosts a web dashboard at `http://<device-ip>` when connected to WiFi.

### API Endpoints

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/` | GET | Web dashboard |
| `/api/status` | GET | Current system status (JSON) |
| `/api/control` | POST | Control actions (toggle_ssr, toggle_mode, buzz) |
| `/api/fan` | POST | Set fan duty cycle (duty=0-100) |
| `/ws` | WebSocket | Real-time status updates |

### WebSocket Messages

Send commands as text:
- `toggle_ssr` - Toggle heating element
- `toggle_mode` - Switch auto/manual mode
- `buzz` - Trigger buzzer
- `set_fan:50` - Set fan to 50%
- `get_status` - Request current status

## Temperature Zones

The RGB LED indicates current temperature zone:

| Temperature | Color |
|-------------|-------|
| < 30°C | Blue |
| 30-50°C | Green |
| 50-70°C | Yellow |
| > 70°C | Red |

## WiFi Setup

On first boot, the device creates an AP named `AutoConnectAP`. Connect to it and use the captive portal to configure your WiFi credentials. The device has a 30-second timeout before starting the AP if no credentials are found.

## Project Structure

```
src/
├── main.cpp           # Main app entry point (Core 1)
├── main_core1.cpp     # Secondary core task (Core 0 - heartbeat)
├── thermo.cpp         # MAX6675 thermocouple interface
├── tft_lcd.cpp        # ILI9341 TFT display
├── lcd.cpp            # 20x4 LCD I2C interface
├── rotary.cpp         # Rotary encoder input
├── fan.cpp            # Fan PWM control
├── ssr.cpp            # Solid state relay control
├── buzzer.cpp         # Buzzer alerts
├── rgb_led.cpp        # NeoPixel RGB LED control
├── my_wifi.cpp        # WiFiManager setup
├── web_server.cpp     # Async web server + WebSocket
└── web_pages.cpp      # Embedded HTML/CSS/JS

config/
└── TFT_eSPI_Setup_ILI9341.h  # TFT display configuration

docs/
└── esp32_S3_pins.txt  # Full pinout documentation
```

## Dependencies

- [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) - TFT display library
- [MAX6675 library](https://github.com/adafruit/MAX6675-library) - Thermocouple interface
- [WiFiManager](https://github.com/tzapu/WiFiManager) - WiFi configuration portal
- [ESP Async WebServer](https://github.com/mathieucarbou/ESPAsyncWebServer) - Async web server
- [Adafruit NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel) - RGB LED control
- [LiquidCrystal_PCF8574](https://github.com/mathieucarbou/LiquidCrystal_PCF8574) - LCD I2C
