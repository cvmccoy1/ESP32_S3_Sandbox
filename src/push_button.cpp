#include <Arduino.h>

#include "log.h"

#define BUTTON_SWITCH_PIN 2  // GPIO pin connected to the push button
#define BUTTON_PRESSED LOW   // Set to LOW when the button is pressed
#define BUTTON_RELEASED HIGH // Set to HIGH when the button is released

#define BUTTON_LED_PIN 1    // GPIO pin connected to the LED
#define BUTTON_LED_ON HIGH  // Set to HIGH to turn on the LED
#define BUTTON_LED_OFF LOW  // Set to LOW to turn off the LED

#define DEBOUNCE_DELAY  50  // 50ms debounce delay

volatile bool interruptFlag = false;
volatile unsigned long lastPressTime = 0;
volatile int lastRawState = BUTTON_RELEASED; // start unpressed

bool isOn = false;

int lastStableState = BUTTON_RELEASED; // debounced button state

portMUX_TYPE myMux = portMUX_INITIALIZER_UNLOCKED; // Spinlock for critical sections

void IRAM_ATTR handleButtonPress() {
    // Record the time and raw state on interrupt
    lastPressTime = millis();  // Note: safe for ISR in ESP32
    lastRawState = digitalRead(BUTTON_SWITCH_PIN);
    interruptFlag = true;
}

void SetupPushButton()
{
    // Set the button pin as an input with pull-up resistor
    pinMode(BUTTON_SWITCH_PIN, INPUT_PULLUP);
    // Set the LED pin as an output
    pinMode(BUTTON_LED_PIN, OUTPUT);
    
    // Initialize the LED to off
    digitalWrite(BUTTON_LED_PIN, BUTTON_LED_OFF);

    // Get the initial button state check
    int initialState = digitalRead(BUTTON_SWITCH_PIN);
    //Slog.printf(PSTR("SetupPushButton(): initialState = %s\r\n"), initialState == BUTTON_PRESSED ? "PRESSED" : "RELEASED");
    lastStableState = initialState;

    // Simulate an interrupt event to account for button already pressed at startup
    taskENTER_CRITICAL(&myMux);
    lastPressTime = millis();  // Mark the time we saw this state
    lastRawState = initialState;
    interruptFlag = true;      // Tell loop to check it after debounce
    taskEXIT_CRITICAL(&myMux);

    attachInterrupt(digitalPinToInterrupt(BUTTON_SWITCH_PIN), handleButtonPress, CHANGE);
}

bool GetPushButtonState()
{
    // Safely copy volatile variables
    taskENTER_CRITICAL(&myMux);
    bool localInterruptFlag = interruptFlag;
    unsigned long localLastPressTime = lastPressTime;
    int localLastRawState = lastRawState;
    taskEXIT_CRITICAL(&myMux);
  
    if (localInterruptFlag) {
      Slog.printf(PSTR("GetPushButtonState(): localLastRawState = %s\r\n"), localLastRawState == BUTTON_PRESSED ? "PRESSED" : "RELEASED");  
      if (millis() - localLastPressTime >= DEBOUNCE_DELAY) {
        int currentState = digitalRead(BUTTON_SWITCH_PIN);
        //Slog.printf(PSTR("GetPushButtonState(): currentState = %s\r\n"), currentState == BUTTON_PRESSED ? "PRESSED" : "RELEASED");
        //Slog.printf(PSTR("GetPushButtonState(): lastStableState = %s\r\n"), lastStableState == BUTTON_PRESSED ? "PRESSED" : "RELEASED");
        if (currentState == localLastRawState) {
            // Detect falling edge (button press)
            if (currentState == BUTTON_PRESSED && lastStableState == BUTTON_RELEASED) {
                isOn = !isOn;  // Toggle state
                //Slog.printf(PSTR("GetPushButtonState(): isOn = %s\r\n"), isOn ? "TRUE" : "FALSE");   
                digitalWrite(BUTTON_LED_PIN, isOn ? BUTTON_LED_ON : BUTTON_LED_OFF); // Update Button LED 
            }
            lastStableState = currentState;          
        }
        // Clear flag safely
        taskENTER_CRITICAL(&myMux);
        interruptFlag = false;
        taskEXIT_CRITICAL(&myMux);
      }
    }
    
    return isOn;
}
