#include <Arduino.h>

#include "driver/pcnt.h"
#include "fan.h"
#include "log.h"

#define FAN_TACH_PIN 4  // Connect to the fan's tachometer pin (Yellow wire)
#define PCNT_UNIT PCNT_UNIT_0
#define PCNT_H_LIM_VAL 32767  // High limit value
#define PCNT_L_LIM_VAL -32768 // Low limit value
#define PCNT_INPUT_SIG_IO  FAN_TACH_PIN  // Pulse Input GPIO

#define FAN_PWM_PIN 5   // Connect to the fan's PWM pin (Blue wire)
#define PWM_CHANNEL 2   // LEDC channel (Channel 2 uses LEDC_TIMER_1 by default)
#define PWM_FREQ 25000  // 25 kHz frequency
#define PWM_RES_BITS 8  // 8-bit resolution (0-255)
#define PWM_RES_MAX  (1 << PWM_RES_BITS) - 1 // Maximum value for resolution

#define FAN_TACH_TIMER     0    // H/W Timer for reading the fan tachometer

volatile int16_t pulseCount = 0;   // Tachometer pulse counter
hw_timer_t *timer = NULL;          // Timer for RPM calculation

void IRAM_ATTR timerISR() {
    pcnt_get_counter_value(PCNT_UNIT, (int16_t *)&pulseCount);
    pcnt_counter_clear(PCNT_UNIT);  // Reset counter after reading
}

pcnt_config_t pcnt_config = {
    .pulse_gpio_num = PCNT_INPUT_SIG_IO,
    .ctrl_gpio_num = PCNT_PIN_NOT_USED,
    .lctrl_mode = PCNT_MODE_KEEP,
    .hctrl_mode = PCNT_MODE_KEEP,
    .pos_mode = PCNT_COUNT_INC,   // Count both rising and falling edges
    .neg_mode = PCNT_COUNT_DIS,
    .counter_h_lim = PCNT_H_LIM_VAL,
    .counter_l_lim = PCNT_L_LIM_VAL,
    .unit = PCNT_UNIT,
    .channel = PCNT_CHANNEL_0,
};

void SetupFan()
{
    // Configure the fan PWM pin
    ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RES_BITS);
    ledcAttachPin(FAN_PWM_PIN, PWM_CHANNEL);
  
    // Set initial fan speed to off (0%)
    SetFanDutyCycle(0);

    // Configure the PCNT peripheral
    pcnt_unit_config(&pcnt_config);

    // Initialize the PCNT counter
    pcnt_counter_pause(PCNT_UNIT);
    pcnt_counter_clear(PCNT_UNIT);

    // Enable the PCNT counter
    pcnt_counter_resume(PCNT_UNIT);

    // Create a timer to read the PCNT counter
    timer = timerBegin(FAN_TACH_TIMER, 80, true); // Timer #, prescaler 80 (1 tick = 1µs)
    timerAttachInterrupt(timer, &timerISR, true);
    timerAlarmWrite(timer, 1000000, true); // 1 second
    timerAlarmEnable(timer);
}

int GetFanRPM()
{
    return pulseCount * 60 / 2;  // 2 pulses per revolution
}

void SetFanDutyCycle(int dutyCycle)
{
    long mappedDutyCycle = map(dutyCycle, 0, 100, 0, PWM_RES_MAX);
    ledcWrite(PWM_CHANNEL, mappedDutyCycle);
    //Slog.printf(PSTR("SetFanDutyCycle(): dutyCycle = %d, mappedDutyCycle = %d\r\n"), dutyCycle, mappedDutyCycle);
}