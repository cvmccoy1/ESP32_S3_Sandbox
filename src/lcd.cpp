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

// Function to update the LCD display
void UpdateDisplay()
{ 
  // Clear the display
  lcd.clear();
  lcd.home();

  lcd.setCursor(0, 0);
  lcd.printf(PSTR("IP: %s"), WiFi.localIP().toString().c_str());

  for (int n = 1; n < lcdRows; n++)
  { 
    static int counter = 0; 
    // Print on LCD
    lcd.setCursor(0, n);
    lcd.printf(PSTR("Test Row %d: %8d"), n, ++counter);

    // Print to Serial Monitor   
    Slog.printf(PSTR("Test Row %d: %8d"), n, counter);
    Slog.print(PSTR("\t"));
  }
  Slog.print(PSTR("\n"));
}
