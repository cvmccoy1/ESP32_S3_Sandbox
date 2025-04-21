#include <Arduino.h>

#include "log.h"

#define BUTTON_SWITCH_PIN 2 // GPIO pin connected to the push button
#define BUTTON_PRESSED LOW  // Set to LOW when the button is pressed

#define BUTTON_LED_PIN 1    // GPIO pin connected to the LED
#define BUTTON_LED_ON HIGH  // Set to HIGH to turn on the LED

#define DEBOUNCE_DELAY  50  // 50ms debounce delay


volatile bool ledState = false;
unsigned long lastPressTime = 0;

void IRAM_ATTR handleButtonPress() {
    unsigned long currentTime = millis();
    
    //Slog.printf(PSTR("Button pressed at %lu ms\r\n"), currentTime);
    if (currentTime - lastPressTime > DEBOUNCE_DELAY) {  // Debounce check
        ledState = !ledState;   // Toggle LED state
        //Slog.printf(PSTR("Button pressed! LED state: %s\r\n"), ledState == BUTTON_LED_ON ? "ON" : "OFF");
        digitalWrite(BUTTON_LED_PIN, ledState ? BUTTON_LED_ON : !BUTTON_LED_ON); // Update LED state
        lastPressTime = currentTime;
    }
}

void SetupPushButton()
{
    // Set the button pin as an input with pull-up resistor
    pinMode(BUTTON_SWITCH_PIN, INPUT_PULLUP);
    // Set the LED pin as an output
    pinMode(BUTTON_LED_PIN, OUTPUT);
    // Initialize the LED to off (not lit)
    digitalWrite(BUTTON_LED_PIN, !BUTTON_LED_ON);

    attachInterrupt(digitalPinToInterrupt(BUTTON_SWITCH_PIN), handleButtonPress, FALLING);
}

bool GetPushButtonState()
{
    return ledState; // Return the current state of the LED (true if on, false if off)
}
