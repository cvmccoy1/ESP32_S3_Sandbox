#include <Arduino.h>
#include <SPI.h>
#include "max6675.h"
#include "thermo.h"
#include "log.h"


#define  SPI_SCLK 40 
#define  SPI_MISO 39
#define  SPI_MOSI -1 // No MOSI needed for MAX6675
#define  SPI_CS   38

#if (USE_HARDWARE_SPI == 1)

// Create an instance of the SPIClass for the MAX6675
SPIClass SPIbus(HSPI);

void SetupThermo() 
{
    // Set CS pin as output
    pinMode(SPI_CS, OUTPUT);
    digitalWrite(SPI_CS, HIGH);  // Deselect device

    // Begin SPI3 on custom pins
    SPIbus.begin(SPI_SCLK, SPI_MISO, SPI_MOSI, SPI_CS);
}

float GetThermocoupleTemperature()
{
    float temperature;

    // Using transaction to avoid conflict with other devices that is HSPI (i.e. TFT Display)
    SPIbus.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));  // 4.0 MHz, MSB first, clock low when idle; sample on rising edge
    // NOTE: According to the MAX6675 datasheet, the 16 output bits should be read on the falling edge of the clock (SPI_MODE1). However,
    //   for some reason, the ESP32-S3 does not latch the data on the falling edge of the clock.  Instead, there is some internal delay (at 
    //   least 30ns) causing the data to be read well after the clock goes low and the data has changed to the next bit value.  So we have to
    //   use SPI_MODE0 to read the data correctly. It seems to be reliable even if technically "wrong".
 
    // Select the MAX6675
    digitalWrite(SPI_CS, LOW);
    //delayMicroseconds(10);  // Allow time for the chip to respond

    // Read 2 bytes of data from the MAX6675
    uint16_t value = SPIbus.transfer16(0x00);

    // Deselect the MAX6675
    digitalWrite(SPI_CS, HIGH);
    SPIbus.endTransaction();

    //Slog.printf(PSTR("GetThermocoupleTemperature([h/w]): raw value = 0x%04X\r\n"), value);
    // Check for fault (bit 2 is set if there is a fault)
    // The MAX6675 returns a 12-bit temperature value in the upper 12 bits of the 16-bit value
    // The lower 4 bits are not used
    if (value & 0x4) {
        Slog.printf(PSTR("Thermocouple Error\r\n"));
        return NAN;  // Return NaN if there is an error
    }

    // Convert to Celsius (the MAX6675 returns temperature in degrees Celsius)
    temperature = value >> 3;  // Shift right to remove the fault bit and convert to Celsius
    //Slog.printf(PSTR("GetThermocoupleTemperature([h/w]): raw temperature = 0x%04X\r\n"), (int)temperature);
    temperature *= 0.25;       // Scale to get the actual temperature

    Slog.printf(PSTR("GetThermocoupleTemperature([h/w]): Temperature = %.2f\r\n"), temperature);
    
    return temperature;
}
#else  // Use bit-banged SPI (Software SPI) for MAX6675

// Create an instance of the MAX6675 class  
MAX6675 thermocouple = MAX6675(SPI_SCLK, SPI_CS, SPI_MISO);

void SetupThermo() 
{
}

float GetThermocoupleTemperature()
{
    // Read temperature from the thermocouple
    float temperature = thermocouple.readCelsius(); // Read temperature in Celsius
    Slog.printf(PSTR("GetThermocoupleTemperature([s/w]): Temperature = %.2f\r\n"), temperature);
    return temperature;
}

#endif
