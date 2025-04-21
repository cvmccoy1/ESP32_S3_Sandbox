#include <Arduino.h>

#include "log.h"

#define BUZZER_PIN 14         // GPIO pin for the buzzer
#define BUZZER_CHANNEL 0      // PWM channel for the buzzer
#define BUZZER_RESOLUTION 8   // Resolution of the PWM signal (8 bits)
#define BUZZER_FREQUENCY 2000 // Frequency of the buzzer in Hz 
#define BUZZER_DURATION 300   // Duration of the buzzer sound in milliseconds
#define BUZZER_VOLUME 127     // Volume level (0-255)
#define BUZZER_OFF 0          // Volume level for turning off the buzzer


void SetupBuzzer()
{
    pinMode(BUZZER_PIN, OUTPUT); // Set the buzzer pin as output
    digitalWrite(BUZZER_PIN, LOW); // Ensure the buzzer is off initially
}

void Buzz() 
{
    //Slog.printf("Buzzing for %d milliseconds\n", BUZZER_DURATION);
    tone(BUZZER_PIN, BUZZER_FREQUENCY, BUZZER_DURATION); // Start the buzzer sound
}