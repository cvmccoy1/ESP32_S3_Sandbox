#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "utilities.h"

// Define the pin connected to the RGB LED
#define LED_PIN 48
// Define the number of LEDs (1 for the onboard RGB LED)
#define NUM_LEDS 1

// Create an instance of the NeoPixel library
Adafruit_NeoPixel rgbLED(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
  // Start Serial Monitor                                                 
  Serial.begin(115200); 

  Serial.println("This is a test");
  delay(2000);    // Not sure why, but the ESP32-S3 seems to require a two second delay before the serial port works

  ReportChipInfo();

  // Initialize the NeoPixel library
  rgbLED.begin();
  rgbLED.show(); // Initialize all LEDs to 'off'
}

void loop()
{
  //Serial.println("Set LED to Red");
  rgbLED.setPixelColor(0, rgbLED.Color(255, 0, 0)); // Red
  rgbLED.show();
  delay(1000);

  //Serial.println("Set LED to Green");
  rgbLED.setPixelColor(0, rgbLED.Color(0, 255, 0)); // Green
  rgbLED.show();
  delay(1000);

  //Serial.println("Set LED to Blue");
  rgbLED.setPixelColor(0, rgbLED.Color(0, 0, 255)); // Blue
  rgbLED.show();
  delay(1000);
}