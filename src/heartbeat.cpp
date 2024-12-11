#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "heartbeat.h"

// Define the pin connected to the RGB LED
#define LED_PIN 48
// Define the number of LEDs (1 for the onboard RGB LED)
#define NUM_LEDS 1

// Create an instance of the NeoPixel library
Adafruit_NeoPixel rgbLED(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Define the LED color state 
int colorState = 0;

esp_timer_handle_t timer = nullptr;

Heartbeat::Heartbeat()
{
    // Initialize the Heartbeat LED pin as an output
    // Initialize the NeoPixel library
    rgbLED.begin();
    rgbLED.show(); // Initialize all LEDs to 'off'
    colorState = 0;
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
    static constexpr std::array<int, 3> colors = { 0xFF0000, 0x00FF00, 0x0000FF };   // { Red, Green, Blue }

    if (++colorState >= colors.size())
        colorState = 0;

    int color = colors[colorState];
    //Serial.printf("One second has passed!  colorState = %d; color = 0x%06X\n", colorState, color);
    
    rgbLED.setPixelColor(0, rgbLED.Color((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF ));
    rgbLED.show();
}

void Heartbeat::StartHeartbeatLED()
{  
    StopHeartbeatLED();  // Stop any previously started timer
    // Create an ESP timer that triggers every 1 second (1000000 microseconds)
    esp_timer_create_args_t timer_args =
    {
        .callback = onTimer,
        .arg = (void *)0,
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
