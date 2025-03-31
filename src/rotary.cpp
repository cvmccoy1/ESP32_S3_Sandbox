#include <Arduino.h>
#include <ESP32RotaryEncoder.h>

#include "log.h"
#include "rotary.h"


#define  DI_ENCODER_SW         46   // Pushbutton of the rotary encoder
#define  DI_ENCODER_DT         16   // Might be labeled A
#define  DI_ENCODER_CLK        17   // Might be labeled B
#define  DI_ENCODER_WRAP       true // Wrap around when turning the knob past the min/max values

RotaryEncoder rotaryEncoder(DI_ENCODER_DT, DI_ENCODER_CLK, DI_ENCODER_SW);

void knobCallback(long value)
{
    // This gets executed every time the knob is turned

    Slog.printf(PSTR("Rotary Value: %i\r\n"), value );
}

void buttonCallback(unsigned long duration)
{
    // This gets executed every time the pushbutton is pressed
    Slog.printf(PSTR("boop! button was down for %u ms\r\n"), duration );
}

void SetupRotaryEncoder()
{
    // This tells the library that the encoder has its own pull-up resistors
    rotaryEncoder.setEncoderType(EncoderType::HAS_PULLUP);

    // Range of values to be returned by the encoder: minimum is 1, maximum is 100
    // The encoder will wrap around when the maximum value is reached
    rotaryEncoder.setBoundaries(1, 100, DI_ENCODER_WRAP);

    // The function specified here will be called every time the knob is turned
    // and the current value will be passed to it
    rotaryEncoder.onTurned(&knobCallback);

    // The function specified here will be called every time the button is pushed and
    // the duration (in milliseconds) that the button was down will be passed to it
    rotaryEncoder.onPressed(&buttonCallback);

    // This is where the inputs are configured and the interrupts get attached
    rotaryEncoder.begin();
}

long GetRotaryEncoderValue()
{
    // This returns the current value of the rotary encoder
    return rotaryEncoder.getEncoderValue();
}   