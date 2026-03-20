# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

- `pio run` - Build the project
- `pio run --target upload` - Upload to ESP32-S3 (COM4)
- `pio run --target monitor` - Monitor serial output (COM3, 115200 baud)

## Project Overview

This is a **reflow controller** application running on an ESP32-S3 (dual-core). It reads temperature from a MAX6675 thermocouple, drives a TFT display showing a real-time temperature graph, and controls a fan, SSR (Solid State Relay), and buzzer based on temperature thresholds.

## Dual-Core Architecture

- **Core 1 (Main App)**: Runs `src/main.cpp` - handles thermocouple reading, display updates, fan control, rotary encoder input
- **Core 0 (Secondary)**: Runs `src/main_core1.cpp` - runs the heartbeat LED (isolated so main app crashes don't affect it)

The secondary core is started via `StartSecondaryCore()` which creates a task pinned to Core 0 using `xTaskCreatePinnedToCore`.

## Key Components

| Component | File | Notes |
|-----------|------|-------|
| MAX6675 Thermocouple | `src/thermo.cpp`, `include/thermo.h` | Uses HSPI bus, custom pins (SCLK=40, MISO=39, CS=38). Hardware SPI uses SPI_MODE0 due to ESP32 timing quirks. |
| TFT LCD (ILI9341) | `src/tft_lcd.cpp` | SPI interface. Config in `config/TFT_eSPI_Setup_ILI9341.h`. Shows reflow oven profile graph. |
| Rotary Encoder | `src/rotary.cpp`, `include/rotary.h` | Uses ESP32RotaryEncoder library |
| RGB LED | `src/rgb_led.cpp` | Adafruit NeoPixel - two LEDs: temp indicator and ON/OFF status |
| Fan | `src/fan.cpp` | PWM speed control |
| SSR | `src/ssr.cpp` | On/off control via relay |
| Buzzer | `src/buzzer.cpp` | Alerts when fan speed direction changes |
| LCD 2004 | `src/lcd.cpp` | I2C backpack (LiquidCrystal_PCF8574) |
| WiFi | `src/my_wifi.cpp` | WiFiManager with 30s timeout, creates AP "AutoConnectAP" if no saved credentials |

## Hardware Configuration

- Board: `esp32-s3-devkitc-1-n16r8v` (ESP32-S3 with 16MB flash, 8MB PSRAM)
- Upload port: COM4, Monitor port: COM3
- Upload speed: 2M baud

## SPI Bus Usage

HSPI bus is shared between MAX6675 thermocouple and TFT display. SPI transactions (`SPIbus.beginTransaction`/`endTransaction`) are used to avoid conflicts.

## Temperature Color Coding

- < 30°C: Blue
- 30-50°C: Green
- 50-70°C: Yellow
- > 70°C: Red
