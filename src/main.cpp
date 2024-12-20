#include <Arduino.h>

#include "log.h"
#include "main_core1.h"
#include "my_wifi.h"
#include "lcd.h"
#include "utilities.h"


void setup()
{
  // Start Serial Monitor                                                 
  Slog.begin(9600); 
  delay(3000);    // Not sure why, but the ESP32-S3 seems to require around a three second delay before the serial port works properly


  // Print the current core ID
  Slog.printf(PSTR("Running Main App on core: %d\n"), xPortGetCoreID());

  ReportChipInfo();

  SetupDisplay();
  
  SetupWifi();

  StartSecondaryCore();  // The Heartbeat LED runs on the Secondary Core so that hangs/crashes on Main App Core don't affect it
}

void loop()
{
  UpdateDisplay();
  delay(1000);
}