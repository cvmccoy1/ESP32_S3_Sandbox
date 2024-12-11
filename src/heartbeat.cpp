#include <Arduino.h>
#include "heartbeat.h"

Heartbeat::Heartbeat()
{
    // Initialize the Heartbeat LED pin as an output
    pinMode(LED_BUILTIN, OUTPUT);
    heartbeatFlag = LOW;
    timer = nullptr;
}

Heartbeat::~Heartbeat()
{
    StopHeartbeatLED();
}

Heartbeat& Heartbeat::Instance()
{
    static Heartbeat instance;
    return instance;
}


// Timer interrupt handler function
void IRAM_ATTR onTimer(void* arg) 
{
    bool* ledState = (bool *)arg;
    *ledState = !(*ledState);
    // Set the Heartbeat LED state
    digitalWrite(LED_BUILTIN, *ledState);
    //Serial.printf("One second has passed!  ledState = %s (%d)\n", *ledState ? "HIGH" : "LOW", *ledState);
}

void Heartbeat::StartHeartbeatLED()
{  
    StopHeartbeatLED();  // Stop any previously started timer
    // Create an ESP timer that triggers every 1 second (1000000 microseconds)
    esp_timer_create_args_t timer_args =
    {
        .callback = onTimer,
        .arg = (void *)&heartbeatFlag,
        .name = "one_second_timer"
    };
    // Initialize the timer
    esp_timer_create(&timer_args, &timer);

    // Start the timer to trigger every 1 second (1,000,000 microseconds)
    esp_timer_start_periodic(timer, 1000000); // 1 second interval
}

void Heartbeat::StopHeartbeatLED()
{
    if (timer != nullptr)
    {
        esp_timer_stop(timer);
        timer = nullptr;
    }
}
