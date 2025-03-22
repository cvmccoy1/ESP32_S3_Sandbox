#include <Arduino.h>

#include "log.h"
#include "main_core1.h"
#include "my_wifi.h"
#include "lcd.h"
#include "utilities.h"

#define FAN_TACH_PIN 4  // Connect to the fan's tachometer pin (Yellow wire)
#define FAN_PWM_PIN 5   // Connect to the fan's PWM pin (Blue wire)
#define PWM_CHANNEL 0   // LEDC channel
#define PWM_FREQ 25000  // 25 kHz frequency
#define PWM_RES 8       // 8-bit resolution (0-255)
int dutyCycle = 255;  // Fan ON at startup
int dutyCycleIncrement = 5;
volatile int pulseCount = 0;  // Tachometer pulse counter
unsigned long lastRPMCheck = 0;
int rpm = 0;

// Interrupt for tachometer signal
void IRAM_ATTR tachInterrupt() 
{
  pulseCount++;
  //Slog.printf(PSTR("tachInterrupt: pulseCount = %d\n"), pulseCount);
}

void setup()
{
  // Start Serial Monitor                                                 
  Slog.begin(115200); 
  delay(3000);    // Not sure why, but the ESP32-S3 seems to require around a three second delay before the serial port works properly


  // Print the current core ID
  Slog.printf(PSTR("Running Main App on core: %d\n"), xPortGetCoreID());

  ReportChipInfo();

  SetupDisplay();
  
  SetupWifi();

  StartSecondaryCore();  // The Heartbeat LED runs on the Secondary Core so that hangs/crashes on Main App Core don't affect it
  

  // Configure the fan PWM pin
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RES);
  ledcAttachPin(FAN_PWM_PIN, PWM_CHANNEL);
  
  // Set initial speed (e.g., 50% duty cycle)
  ledcWrite(PWM_CHANNEL, dutyCycle); 

  // Set up tachometer input
  pinMode(FAN_TACH_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(FAN_TACH_PIN), tachInterrupt, FALLING);
}

void loop()
{
  unsigned long now = millis();  
    
  // Measure RPM every second
  if (now - lastRPMCheck >= 1000) {
    noInterrupts();
    int count = pulseCount;
    pulseCount = 0;
    interrupts();
        
    // RPM calculation (3-pin fans usually output 2 pulses per revolution)
    rpm = (count * 30);  // (count / 2) * 60        
    lastRPMCheck = now;

    UpdateDisplay(rpm, dutyCycle);

    dutyCycle += dutyCycleIncrement;
    if (dutyCycle > 255) {
      dutyCycleIncrement = -5;
      dutyCycle = 255;  // Fan at full speed
    } else if (dutyCycle < 0) {
      dutyCycleIncrement = 5; // Fan at minimum speed   
      dutyCycle = 0;
    }
    ledcWrite(PWM_CHANNEL, dutyCycle);  // Set the fan speed
    //delay(1000);
  }
}