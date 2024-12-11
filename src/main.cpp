#include <Arduino.h>

#include "main_core1.h"
#include "utilities.h"


void setup()
{
  // Start Serial Monitor                                                 
  Serial.begin(115200); 

  Serial.println("This is a test A");
  delay(3000);    // Not sure why, but the ESP32-S3 seems to require a three second delay before the serial port works
  Serial.println("This is a test B");

  // Print the current core ID
  Serial.printf("Running Main App on core: %d", xPortGetCoreID());

  ReportChipInfo();

  StartSecondaryCore();  // The Heartbeat LED runs on the Secondary Core so that hangs/crashes on Main App Core don't affect it
}

void loop()
{

}