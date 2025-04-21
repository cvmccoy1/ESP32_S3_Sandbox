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

#define MAX_FAN_DUTY_CYCLE 100
#define MIN_FAN_DUTY_CYCLE 0
#define FAN_DUTY_CYCLE_INCREMENT 2

esp_timer_handle_t main_loop_timer = nullptr;
volatile bool updateFlag = false; // Flag to indicate timer fired

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

  // Start the timer to trigger every 1/4 second (250,000 microseconds)
  esp_timer_start_periodic(main_loop_timer, 250000); // 1/4 second interval
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
    int rpm = GetFanRPM();
    float temperature = GetThermocoupleTemperature();
    long rotaryValue = GetRotaryEncoderValue(); // Read the rotary encoder value  
    UpdateDisplay(rpm, dutyCycle, temperature, rotaryValue); // Update the LCD display
    UpdateTFT(); // Update the TFT display

    dutyCycle += dutyCycleIncrement;
    if (dutyCycle > MAX_FAN_DUTY_CYCLE) {
      dutyCycleIncrement = -FAN_DUTY_CYCLE_INCREMENT;
      dutyCycle = MAX_FAN_DUTY_CYCLE;  // Fan at full speed
      Buzz(); // Buzz the buzzer when the fan speed changes
    } else if (dutyCycle < MIN_FAN_DUTY_CYCLE) {
      dutyCycleIncrement = FAN_DUTY_CYCLE_INCREMENT; // Fan at minimum speed   
      dutyCycle = MIN_FAN_DUTY_CYCLE;
      Buzz(); // Buzz the buzzer when the fan speed changes
    }
    SetFanDutyCycle(dutyCycle);
    //Slog.printf(PSTR(">Fan_RPM:%d,Duty_Cycle:%d\r\n"), rpm, dutyCycle);
    bool pushButtonState = GetPushButtonState(); // Read the push button state
    SetSSRState(pushButtonState); // Set the SSR state based on the push button state
    Slog.printf(PSTR(">Temperature1:%.1f,State:%d\r\n"), temperature, pushButtonState);
  }
}