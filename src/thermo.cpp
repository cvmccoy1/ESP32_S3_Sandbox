#include <Arduino.h>
#include <SPI.h>
#include "max6675.h"
#include "thermo.h"
#include "log.h"

int thermoDO  = 13;
int thermoCLK  = 12;
int thermoCS_1 = 10;
int thermoCS_2 = 11;


MAX6675 thermocouple[2] = { MAX6675(thermoCLK, thermoCS_1, thermoDO),
                            MAX6675(thermoCLK, thermoCS_2, thermoDO) };

float GetThermocoupleTemperature(u_int thermocoupleIndex)
{
    if (thermocoupleIndex >= NUMBER_OF_THERMOCOUPLES)  {
        Slog.printf(PSTR("ERROR>>> Invalid thermocouple index: %d\r\n"), thermocoupleIndex);
        return 0.0; // Invalid thermocouple index
    }
    
    // Read temperature from the thermocouple
    float temperature = thermocouple[thermocoupleIndex].readCelsius(); // Read temperature in Celsius
    return temperature;
}
