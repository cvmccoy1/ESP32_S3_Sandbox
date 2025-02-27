#include <Arduino.h>

#include "log.h"
#include "main_core1.h"
#include "my_wifi.h"
#include "lcd.h"
#include "utilities.h"


#define FAN_TACH_PIN 35       // Fan tachometer signal (Input)
volatile int pulseCount = 0;  // Tachometer pulse counter
unsigned long lastRPMCheck = 0;
int rpm = 0;

// Interrupt for tachometer signal
void IRAM_ATTR tachInterrupt() {
  pulseCount++;
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
  
  // Set up tachometer input
  pinMode(FAN_TACH_PIN, INPUT_PULLUP);
  attachInterrupt(FAN_TACH_PIN, tachInterrupt, FALLING);
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

     UpdateDisplay(rpm);
     //delay(1000);
  }
}