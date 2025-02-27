#pragma once

// Define LCD I2C Address - Change If Required
const int lcdI2Caddr = 0x27;

// Set the LCD number of columns and rows
const int lcdColumns = 20;
const int lcdRows = 4;

void SetupDisplay();
void UpdateIpAddress(const char* ipAddr);
void UpdateDisplay(int rpm);