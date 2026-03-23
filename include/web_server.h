#pragma once

#include <Arduino.h>

void SetupWebServer();
void UpdateWebServer(float temperature, float fanDutyCycle, int fanRpm, bool ssrOn, const char* stageName);
