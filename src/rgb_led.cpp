#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "rgb_led.h"
#include "log.h"

#define RGB_LED_PIN 1    // GPIO pin connected to the RGB LED string

Adafruit_NeoPixel string(NUM_RGB_LEDS, RGB_LED_PIN, NEO_GRB + NEO_KHZ800);

void SetupRGBLED()
{
    // Initialize the NeoPixel library
    string.begin();
    string.show(); // Initialize all LEDs to 'off'
}

void SetRGBLEDColor(int ledIndex, int color)
{
    if (ledIndex < 0 || ledIndex >= NUM_RGB_LEDS) {
        Slog.printf(PSTR("Invalid LED index: %d\r\n"), ledIndex);
        return;
    }

    // Set the color of the specified LED
    string.setPixelColor(ledIndex, color);
    string.show(); // Update the LED to show the new color
}