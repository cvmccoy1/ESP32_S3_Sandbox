#include <Arduino.h>
#include <SPI.h>
#include "max6675.h"
#include "thermo.h"
#include "log.h"


int thermoCLK  = 40; 
int thermoSO   = 39;
int thermoCS   = 38;

MAX6675 thermocouple = MAX6675(thermoCLK, thermoCS, thermoSO);

float GetThermocoupleTemperature()
{
    // Read temperature from the thermocouple
    float temperature = thermocouple.readCelsius(); // Read temperature in Celsius
    return temperature;
}
