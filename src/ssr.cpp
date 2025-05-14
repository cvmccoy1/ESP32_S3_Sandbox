#include <Arduino.h>

#include "ssr.h"
#include "log.h"

#define SSR_PIN 42    // GPIO pin connected to the SSR control pin
#define SSR_ON HIGH   // Set to HIGH to turn on the SSR (and the connected load)
#define SSR_OFF LOW   // Set to LOW to turn off the SSR (and the connected load)

void SetupSSR()
{
    // Set the SSR pin as an output
    pinMode(SSR_PIN, OUTPUT);
    // Initialize the SSR to off (0% duty cycle)
    digitalWrite(SSR_PIN, SSR_OFF);
}

void SetSSRState(bool state)
{
    static bool lastState = false; // Variable to store the last state of the SSR
    if (state != lastState) {
        // Set the SSR state (on or off)
        if (state) {
            digitalWrite(SSR_PIN, SSR_ON);  // Turn on the SSR
            //Slog.printf(PSTR("SSR is ON\r\n"));
        } else {
            digitalWrite(SSR_PIN, SSR_OFF); // Turn off the SSR
            //Slog.printf(PSTR("SSR is OFF\r\n"));
        }

        lastState = state; // Update the last state variable
        Slog.printf(PSTR("SSR state changed to %s\r\n"), state ? "ON" : "OFF");
    }
}
