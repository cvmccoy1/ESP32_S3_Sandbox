#pragma once

#define COLOR_RED     0xFF0000
#define COLOR_GREEN   0x00FF00
#define COLOR_BLUE    0x0000FF
#define COLOR_YELLOW  0xFFFF00
#define COLOR_CYAN    0x00FFFF
#define COLOR_MAGENTA 0xFF00FF
#define COLOR_WHITE   0xFFFFFF
#define COLOR_OFF     0x000000

#define ON_OFF_LED_INDEX    0 // Index of the LED used for on/off state indication
#define TEMP_LED_INDEX      1 // Index of the LED used for temperature indication
#define NUM_RGB_LEDS        2 // Number of LEDs in the RGB LED string

void SetupRGBLED();
void SetRGBLEDColor(int ledIndex, int color);
