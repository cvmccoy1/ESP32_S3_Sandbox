#include <Arduino.h>

#include "log.h"
#include "main_core1.h"
#include "my_wifi.h"
#include "lcd.h"
#include "fan.h"
#include "thermo.h"
#include "rotary.h"
#include "ssr.h"
#include "push_button.h"
#include "tft_lcd.h"
#include "buzzer.h"
#include "utilities.h"

#define MAIN_LOOP_INTERVAL 500000      // 1/2 second interval in microseconds

#define MAX_FAN_DUTY_CYCLE        100  // In Percent
#define MIN_FAN_DUTY_CYCLE        20   // In Percent
#define FAN_DUTY_CYCLE_INCREMENT  10   // In Percent
#define FAN_UPDATE_COUNTER        10 * 1000000 / MAIN_LOOP_INTERVAL  // Number of times through the main loop before updating the fan speed every 10 seconds

esp_timer_handle_t main_loop_timer = nullptr;
volatile bool updateFlag = false; // Flag to indicate timer fired

int fanUpdateCounter = 0; // Counter to track fan speed updates

// Timer interrupt handler function
void IRAM_ATTR onMainLoopTimer(void* arg) {
  updateFlag = true;
}

void setupMainLoopTimer()
{
  // Create a one-second timer for updating the display
  // Create an ESP timer that triggers every 1 second (1000000 microseconds)
  esp_timer_create_args_t timer_args ={
    .callback = onMainLoopTimer,
    .arg = (void *)0,
    .name = "main_loop_timer" 
  };
  // Initialize the timer
  esp_timer_create(&timer_args, &main_loop_timer);

  // Start the timer to trigger every 1/2 second (500,000 microseconds)
  esp_timer_start_periodic(main_loop_timer, MAIN_LOOP_INTERVAL); 
}

void setup()
{
  // Start Serial Monitor                                                 
  Slog.begin(115200); 
  delay(3000);    // Not sure why, but the ESP32-S3 seems to require around a three second delay before the serial port works properly


  // Print the current core ID
  Slog.printf(PSTR("Running Main App on core: %d\n"), xPortGetCoreID());

  ReportChipInfo();

  Slog.printf("SetupDisplay()\r\n");
  SetupDisplay();
  Slog.printf("SetupTFT()\r\n");
  SetupTFT();
  Slog.printf("SetupFan()\r\n");
  SetupFan();
  Slog.printf("SetupWifi()\r\n");
  SetupWifi();
  Slog.printf("SetupRotaryEncoder()\r\n");
  SetupRotaryEncoder();
  Slog.printf("SetupSSR()\r\n");
  SetupSSR();
  Slog.printf("SetupThermo()\r\n"); 
  SetupThermo();
  Slog.printf("SetupPushButton()\r\n");
  SetupPushButton();
  Slog.printf("SetupBuzzer()\r\n");
  SetupBuzzer();
  Slog.printf("SetupSecondaryCore()\r\n");
  StartSecondaryCore();  // The Heartbeat LED runs on the Secondary Core so that hangs/crashes on Main App Core don't affect it
  Slog.printf("SetupMainLoopTimer()\r\n");
  setupMainLoopTimer();
  Slog.printf("Setup() done\r\n");
}

void loop()
{ 
  static int dutyCycle = MIN_FAN_DUTY_CYCLE;
  static int dutyCycleIncrement = FAN_DUTY_CYCLE_INCREMENT;   

  if (updateFlag) {
    updateFlag = false;

    float temperature = GetThermocoupleTemperature();
    long rotaryValue = GetRotaryEncoderValue(); // Read the rotary encoder value 
    bool pushButtonState = GetPushButtonState(); // Read the push button state 
    SetSSRState(pushButtonState); // Set the SSR state based on the push button state

    if (++fanUpdateCounter >= FAN_UPDATE_COUNTER) {
      fanUpdateCounter = 0; // Reset the counter
      if (dutyCycle >= MAX_FAN_DUTY_CYCLE) {
        dutyCycleIncrement = -FAN_DUTY_CYCLE_INCREMENT;
        Buzz(); // Buzz the buzzer when the fan speed starts to decrease
      }
      else if (dutyCycle <= MIN_FAN_DUTY_CYCLE) {
        dutyCycleIncrement = FAN_DUTY_CYCLE_INCREMENT; // Fan at minimum speed   
        Buzz(); // Buzz the buzzer when the fan speed starts to increase
      }
      dutyCycle += dutyCycleIncrement;
      SetFanDutyCycle(dutyCycle);
    }
    int rpm = GetFanRPM();
    UpdateDisplay(rpm, dutyCycle, temperature, rotaryValue); // Update the LCD display
    UpdateTFT(temperature, 250); // Update the TFT display
    //Slog.printf(PSTR(">Fan_RPM:%d,Duty_Cycle:%d\r\n"), rpm, dutyCycle);
    //Slog.printf(PSTR(">Temperature1:%.1f\r\n"), temperature);
  }
}