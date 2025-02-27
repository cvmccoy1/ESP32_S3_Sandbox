// Include the Serial Peripheral Interface (SPI) Library
#include <SPI.h>

// Include Arduino Framework Library
#include <Arduino.h>

// Include PCF8574 Library for I2C LCD
#include <LiquidCrystal_PCF8574.h>

// Include Wire Library for I2C Communications
#include <Wire.h>

#include <WiFi.h>

// Include Assert Library
#include <assert.h>

#include "log.h"
#include "lcd.h"

// Define LCD object
LiquidCrystal_PCF8574 lcd(lcdI2Caddr); 

const char* ipAddress = PSTR("Not Defined");

void SetupDisplay()
{
  // Setup the LCD Display
  lcd.begin(lcdColumns, lcdRows); 
  lcd.setBacklight(255);
}

void UpdateIpAddress(const char* ipAddr)
{
  ipAddress = ipAddr;
}

#define WIFI_STATUS_STATES 7
const std::string wifiStatusString[WIFI_STATUS_STATES] =
{
  PSTR("IDLE"),
  PSTR("NO SSID"),
  PSTR("SCAN COMPLETED"),
  PSTR("CONNECTED"),
  PSTR("CONNECT FAILED"),
  PSTR("CONNECTION LOST"),
  PSTR("DISCONNECTED")
};

// Function to update the LCD display
void UpdateDisplay(int rpm)
{ 
  // Clear the display
  lcd.clear();
  lcd.home();

  lcd.setCursor(0, 0);
  lcd.printf(PSTR("IP: %s"), WiFi.localIP().toString().c_str());

  lcd.setCursor(0, 1);
  wl_status_t status = WiFi.status();
  Slog.printf(PSTR("WiFi %s (%d)\n"), status < WIFI_STATUS_STATES ? wifiStatusString[status].c_str() : PSTR("UNKNOWN"), status);
  lcd.printf(PSTR("WiFi %s"), status < WIFI_STATUS_STATES ? wifiStatusString[status].c_str() : PSTR("UNKNOWN"));

  lcd.setCursor(0, 2);
  Slog.printf(PSTR("Fan RPM: %d\n"), rpm);
  lcd.printf(PSTR("Fan RPM: %d"), rpm);

  static int counter = 0; 
  lcd.setCursor(0, 3);
  Slog.printf(PSTR("Counter: %d\n"), counter);
  lcd.printf(PSTR("Counter: %d"), ++counter);
}
