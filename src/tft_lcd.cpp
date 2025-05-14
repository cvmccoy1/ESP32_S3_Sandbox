#include <Arduino.h>
#include <TFT_eSPI.h> 
#include <SPI.h>
#include "log.h"

#define GRAPH_WIDTH  320
#define GRAPH_HEIGHT 100
#define GRAPH_X      0
#define GRAPH_Y      140

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

uint16_t graphPos = 0;
float lastTempY = 0, lastSetpointY = 0;
float graphMin = 0, graphMax = 250;

enum ReflowStage {PREHEAT, SOAK, REFLOW, COOLING, COMPLETE};
ReflowStage currentStage = PREHEAT;

void drawGraphFrame();
void updateInfoPanel(double temp, double setpoint, bool heating, ReflowStage stage, int countdown);
void updateGraph(float temp, float setpoint);

void SetupTFT(void) 
{
  tft.init();
  //tft.setSwapBytes(true); // Set to true for ESP32, false for ESP8266 

  //tft.setWindow(0, 0, 240, 320); // Set window size to full screen

  //Slog.printf("tft.setRotation()\r\n");
  tft.setRotation(1); // Set rotation to 1 for landscape mode

  //Slog.printf("tft.fillScreen()\r\n");
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);

  tft.setCursor(10, 10);
  tft.println("Reflow Controller");

  drawGraphFrame();
}
  
  
void UpdateTFT(float temp, float setpoint)
{
  updateInfoPanel(temp, setpoint, true, currentStage, 300);
  updateGraph(temp, setpoint);
}

void drawGraphFrame() 
{
  tft.drawRect(GRAPH_X, GRAPH_Y, GRAPH_WIDTH, GRAPH_HEIGHT, TFT_WHITE);

  for (int i = 0; i <= 5; i++) 
  {
    int y = GRAPH_Y + GRAPH_HEIGHT - (i * GRAPH_HEIGHT / 5);
    tft.drawFastHLine(GRAPH_X, y, GRAPH_WIDTH, TFT_DARKGREY);
    tft.setCursor(0, y - 8);
    float tempVal = graphMin + (i * (graphMax - graphMin) / 5);
    tft.printf("%.0f", tempVal);
  }
}

void updateInfoPanel(double temp, double setpoint, bool heating, ReflowStage stage, int countdown) 
{
  tft.fillRect(0, 40, 320, 20, TFT_BLACK);
  tft.setCursor(10, 40);
  tft.printf("Temp: %.1f  Set: %.1f ", temp, setpoint);

  tft.fillRect(0, 60, 320, 20, TFT_BLACK);
  tft.setCursor(10, 60);
  tft.print("Stage: ");
  switch (stage) {
    case PREHEAT: tft.print("PREHEAT"); break;
    case SOAK:    tft.print("SOAK"); break;
    case REFLOW:  tft.print("REFLOW"); break;
    case COOLING: tft.print("COOLING"); break;
    case COMPLETE:tft.print("COMPLETE"); break;
  }

  tft.fillRect(0, 80, 320, 20, TFT_BLACK);
  tft.setCursor(10, 80);
  tft.printf("Time Left: %ds", countdown);
}

void updateGraph(float temp, float setpoint) {
  if (temp < graphMin || temp > graphMax || setpoint > graphMax || setpoint < graphMin) 
  {
    graphMin = min(graphMin, min(temp, setpoint)) - 5;
    graphMax = max(graphMax, max(temp, setpoint)) + 5;
    tft.fillRect(GRAPH_X + 1, GRAPH_Y + 1, GRAPH_WIDTH - 2, GRAPH_HEIGHT - 2, TFT_BLACK);
    drawGraphFrame();
    graphPos = 0;
    lastTempY = lastSetpointY = GRAPH_Y + GRAPH_HEIGHT;
    return;
  }

  int tempY = GRAPH_Y + GRAPH_HEIGHT - map(temp, graphMin, graphMax, 0, GRAPH_HEIGHT);
  int setY  = GRAPH_Y + GRAPH_HEIGHT - map(setpoint, graphMin, graphMax, 0, GRAPH_HEIGHT);

  if (graphPos > 0 && graphPos < GRAPH_WIDTH) {
    tft.drawLine(GRAPH_X + graphPos - 1, lastTempY, GRAPH_X + graphPos, tempY, TFT_RED);
    tft.drawLine(GRAPH_X + graphPos - 1, lastSetpointY, GRAPH_X + graphPos, setY, TFT_BLUE);
  }

  lastTempY = tempY;
  lastSetpointY = setY;
  graphPos++;

  if (graphPos >= GRAPH_WIDTH) {
    graphPos = 0;
    tft.fillRect(GRAPH_X + 1, GRAPH_Y + 1, GRAPH_WIDTH - 2, GRAPH_HEIGHT - 2, TFT_BLACK);
    drawGraphFrame();
  }
}