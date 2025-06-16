#include <Arduino.h>
#include <ESP32RotaryEncoder.h>

#include "log.h"
#include "rotary.h"


#define  DI_ENCODER_SW         18   // Pushbutton of the rotary encoder
#define  DI_ENCODER_DT         17   // Might be labeled A
#define  DI_ENCODER_CLK        16   // Might be labeled B
#define  DI_ENCODER_WRAP       true // Wrap around when turning the knob past the min/max values


// A button-press is considered "long" if
// it's held for more than two seconds
#define LONG_PRESS 2 * 1000

volatile bool rotaryEncoderButtonPressed = false;

RotaryEncoder rotaryEncoder(DI_ENCODER_DT, DI_ENCODER_CLK, DI_ENCODER_SW);

void buttonShortPress()
{
	Slog.printf(PSTR("boop!\r\n"));
    rotaryEncoderButtonPressed = true; // Set the flag to indicate button press
}

void buttonLongPress()
{
	Slog.printf(PSTR("BOOOOOOOOOOP!\r\n"));
}

void knobCallback(long value)
{
    // This gets executed every time the knob is turned

    Slog.printf(PSTR("Rotary Value: %i\r\n"), value );
}

void buttonCallback(unsigned long duration)
{
    // This gets executed every time the pushbutton is pressed
    Slog.printf(PSTR("boop! button was down for %u ms\r\n"), duration );
    	if (duration > LONG_PRESS)
	{
		buttonLongPress();
	}
	else
	{
		buttonShortPress();
	}
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

bool GetRotaryPushButtonState()
{
    // This returns the state of the pushbutton
    // and resets the flag after reading it
    bool buttonPressed = rotaryEncoderButtonPressed;
    rotaryEncoderButtonPressed = false; // Reset the flag
    return buttonPressed;
}