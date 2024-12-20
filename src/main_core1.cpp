// Include Arduino Framework Library
#include <Arduino.h>

#include "log.h"
#include "heartbeat.h"

// Declare a handle for the task running on Core 1
TaskHandle_t taskHandle = nullptr;

// Local Forward References
void setupTask();
void loopTask();
void IRAM_ATTR secondaryTask(void *parameter);

void StartSecondaryCore()
{
  // Create a task that will run on Core 0 (Main App is running on Core 1)
  // The task will use the secondaryTask function and run on Core 0
  xTaskCreatePinnedToCore(
    secondaryTask,       // Task function
    PSTR("SecondaryTask"),   // Task name
    10000,             // Stack size (in words)
    NULL,              // Task parameters
    0,                 // Task priority
    &taskHandle,       // Task handle
    0                  // Core 0 (use Core 0 for this task)
  );
}

// Function that will run on Core 1
void IRAM_ATTR secondaryTask(void *parameter)
{
  setupTask();
  while(true) 
  {
    loopTask();
  }
}

// Setup Function for Secondary Core - Add Setup code for here
void setupTask()
{
  // Print the current core ID
  Slog.printf(PSTR("Running Secordary App on core: %d\n"), xPortGetCoreID());
  Heartbeat::Instance().StartHeartbeatLED();
}

// Loop Function for Secondary Core - Add Execution code for here
void loopTask()
{
  //Slog.println(PSTR("Running on Secondary Core)");
}